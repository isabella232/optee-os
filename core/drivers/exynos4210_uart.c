// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2018, Linaro Limited
 */
#include <assert.h>
#include <drivers/exynos4210_uart.h>
#include <io.h>
#include <keep.h>
#include <kernel/dt.h>
#include <stdlib.h>
#include <trace.h>
#include <types_ext.h>
#include <util.h>

/* register info from Samsung 'Pulbicmanual_Exynos_4_Dual_45nm_Ver00-2.pdf'
 * Note that Samsung has (at present) a typo in the name of the doc */

#define UART_LCON	0x0000 /* line control register    */
#define UART_CON	0x0004 /* control register         */
#define UART_FCON	0x0008 /* FIFO Control             */
#define UART_MCON	0x000C /* Modem Control            */
#define UART_TRSTAT	0x0010 /* Tx/Rx Status             */
#define UART_ERSTAT	0x0014 /* UART Error Status        */
#define UART_FSTAT	0x0018 /* FIFO Status              */
#define UART_MSTAT	0x001C /* Modem Status             */
#define UART_TXH	0x0020 /* Transmit Buffer          */
#define UART_RXH	0x0024 /* Receive Buffer           */
#define UART_BRDIV	0x0028 /* Baud Rate Divisor        */
#define UART_FRAC	0x002C /* Divisor Fractional Value */
#define UART_INTP	0x0030 /* Interrupt Pending        */
#define UART_INTSP	0x0034 /* Interrupt Source Pending */
#define UART_INTM	0x0038 /* interrupt mask set/clear */

/* UART Line Control register UART_LCON */
#define UART_LINE_MODE_NORMAL	(0 << 6)
#define UART_LINE_MODE_INFRARED	(1 << 6)
#define UART_PARITY_NONE		(0 << 3)
#define UART_PARITY_ODD			(4 << 3)
#define UART_PARITY_EVEN		(5 << 3)
#define UART_PARITY_FORCED_1	(6 << 3)
#define UART_PARITY_FORCED_0	(7 << 3)
#define UART_STOP_1BIT			(0 << 2) 
#define UART_STOP_2BIT			(1 << 2) 
#define UART_DATA_LEN_5BIT		0
#define UART_DATA_LEN_6BIT		1
#define UART_DATA_LEN_7BIT		2
#define UART_DATA_LEN_8BIT		3

/* UART Control register UART_CON */
#define UART_TX_INT_LEVEL	(1 << 9)
#define UART_RX_INT_LEVEL	(1 << 8)
#define UART_RX_TIMEOUT_DIS	(0 << 7)
#define UART_RX_TIMEOUT_EN	(1 << 7)
#define UART_RX_ERR_INT_DIS	(0 << 6)
#define UART_RX_ERR_INT_EN	(1 << 6)
#define UART_OP_NORMAL		(0 << 5)
#define UART_OP_LOOPBACK	(1 << 5)
#define UART_TX_NORMAL		(0 << 4)
#define UART_TX_BREAK		(1 << 4)
#define UART_TX_MODE_DIS	(0 << 2)
#define UART_TX_MODE_POLL	(1 << 2)
#define UART_TX_MODE_DMA	(2 << 2)
#define UART_RX_MODE_DIS	0
#define UART_RX_MODE_POLL	1
#define UART_RX_MODE_DMA	2

/* UART FIFO Control register UART_UFCON */
/* Tx FIFO trigger level (chan0 = 32, chan1 & chan4 = 8, chan2 & chan3 = 2) */
#define UART_TX_FIFO_TRIG_LEVEL	(1 << 8)
/* Rx FIFO trigger level (chan0 = 64, chan1 & chan4 = 16, chan2 & chan3 = 4) */
#define UART_RX_FIFO_TRIG_LEVEL	(1 << 4)
/* Auto-clears after resetting Tx FIFO: */
#define UART_TX_FIFO_NORMAL		(0 << 2)
#define UART_TX_FIFO_RESET		(1 << 2)
/* Auto-clears after resetting Rx FIFO: */
#define UART_RX_FIFO_NORMAL		(0 << 1)
#define UART_RX_FIFO_RESET		(1 << 1)
#define UART_FIFO_DIS			0
#define UART_FIFO_EN			1

/* UART Tx/Rx Status register UART_TRSTAT */
#define UART_RX_TIMEOUT			(1 << 3)
#define UART_TRANSMIT_EMPTY		(1 << 2)
#define UART_TX_BUFFER_EMPTY	(1 << 1)
#define UART_RX_BUFFER_READY	1

/* UART FIFO Status register UART_FSTAT */
#define UART_TX_FIFO_FULL		(1 << 24)
#define UART_TX_FIFO_CNT_SHIFT	16
#define UART_TX_FIFO_CNT		(0xFF << UART_TX_FIFO_CNT_SHIFT)
#define UART_RX_FIFO_FULL		(1 << 8)
#define UART_RX_FIFO_CNT		0xFF

static vaddr_t chip_to_base(struct serial_chip *chip)
{
	struct exynos4210_uart_data *pd =
		container_of(chip, struct exynos4210_uart_data, chip);

	return io_pa_or_va(&pd->base);
}

static bool exynos4210_uart_have_rx_data(struct serial_chip *chip)
{
	int32_t fifo_stat;
	vaddr_t base = chip_to_base(chip);
	
	fifo_stat = read32(base + UART_FSTAT);

	/* check Rx count */
	if ((fifo_stat & UART_RX_FIFO_CNT) > 0)
		return true;
	
	/* also check for full Rx FIFO (Rx count is 0 on Rx FIFO full) */
	return ((fifo_stat & UART_RX_FIFO_FULL) == UART_RX_FIFO_FULL);
}

static int exynos4210_uart_getchar(struct serial_chip *chip)
{
	vaddr_t base = chip_to_base(chip);

	while (!exynos4210_uart_have_rx_data(chip))
		;

	/* Read the character */
	return (read32(base + UART_RXH) & 0xff);
}

static bool exynos4210_uart_tx_full(struct serial_chip *chip)
{
	int32_t fifo_stat;
	vaddr_t base = chip_to_base(chip);
	
	fifo_stat = read32(base + UART_FSTAT);

	/* check for full Tx FIFO. Note that Tx count is 0 on Tx FIFO full */
	return ( (fifo_stat & UART_TX_FIFO_FULL) == UART_TX_FIFO_FULL );
}

static bool exynos4210_uart_have_tx_data(struct serial_chip *chip)
{
	int32_t fifo_stat;
	vaddr_t base = chip_to_base(chip);

	fifo_stat = read32(base + UART_FSTAT);

	/* check for full Tx FIFO, as Tx count is 0 on Tx FIFO full condition */
	if ((fifo_stat & UART_TX_FIFO_FULL) == UART_TX_FIFO_FULL)
		return true;

	/* now safe to check Tx count */
	return ( ((fifo_stat & UART_TX_FIFO_CNT) >> UART_TX_FIFO_CNT_SHIFT) > 0);
}

static void exynos4210_uart_flush(struct serial_chip *chip)
{
	/* wait for data to be transmitted */
	while (exynos4210_uart_have_tx_data(chip))
		;
}

static void exynos4210_uart_putc(struct serial_chip *chip, int ch)
{
	vaddr_t base = chip_to_base(chip);

	/* Wait until there is space in the FIFO */
	while (exynos4210_uart_tx_full(chip))
		;

	/* Send the character */
	write32( (ch & 0xff), (base + UART_TXH) );
}

static const struct serial_ops exynos4210_uart_ops = {
	.flush = exynos4210_uart_flush,
	.getchar = exynos4210_uart_getchar,
	.have_rx_data = exynos4210_uart_have_rx_data,
	.putc = exynos4210_uart_putc,
};
KEEP_PAGER(exynos4210_uart_ops);

void exynos4210_uart_init(struct exynos4210_uart_data *pd, paddr_t pbase, 
		uint32_t uart_clk, uint32_t baud_rate)
{
	vaddr_t base;
	uint32_t div_val;

	pd->base.pa = pbase;
	pd->chip.ops = &exynos4210_uart_ops;

	base = io_pa_or_va(&pd->base);

	/* Configure line default 8 data bits, 1 stop bit, no parity, normal mode */
	write32(UART_DATA_LEN_8BIT |
			UART_STOP_1BIT |
			UART_PARITY_NONE |
			UART_LINE_MODE_NORMAL, 
			base + UART_LCON);

	exynos4210_uart_flush(&pd->chip);

	/* Set up clock division registers, but only if baudrate defined */
	if (baud_rate) {
		/* 
		 * DIV_VAL = (uart_clk / (baud_rate * 16)) - 1 
		 * DIV_VAL = UART_BRDIV + UART_FRAC/16
		 */
		div_val = (uart_clk / baud_rate) - 16;

		write32(div_val / 16, base + UART_BRDIV);
		write32(div_val % 16, base + UART_FRAC);
	}
	
	/* No hardware control used */
	write32(0, base + UART_MCON);

	/* Configure, reset and enable Tx/Rx FIFOs */
	write32(UART_TX_FIFO_TRIG_LEVEL |
			UART_RX_FIFO_TRIG_LEVEL |
			UART_TX_FIFO_RESET |
			UART_RX_FIFO_RESET |
			UART_FIFO_EN,
			base + UART_FCON);

	/* Configure UART and enable Tx/Rx */
	write32(UART_TX_INT_LEVEL |
			UART_RX_INT_LEVEL |
			UART_RX_TIMEOUT_EN |
			UART_RX_ERR_INT_EN |
			UART_OP_NORMAL |
			UART_TX_NORMAL |
			UART_TX_MODE_POLL |
			UART_RX_MODE_POLL,
			base + UART_CON);
}

#ifdef CFG_DT

static struct serial_chip *exynos4210_uart_dev_alloc(void)
{
	struct exynos4210_uart_data *pd = malloc(sizeof(*pd));

	if (!pd)
		return NULL;
	return &pd->chip;
}

static int exynos4210_uart_dev_init(struct serial_chip *chip, 
		const void *fdt, int offs, const char *parms)
{
	struct exynos4210_uart_data *pd = 
			container_of(chip, struct exynos4210_uart_data, chip);
	vaddr_t vbase;
	paddr_t pbase;
	size_t size;

	if (parms && parms[0])
		IMSG("Exynos-4210 UART: device parameters ignored (%s)", parms);

	if (dt_map_dev(fdt, offs, &vbase, &size) < 0)
		return -1;

	if (size != EXYNOS4210_UART_REG_SIZE) {
		EMSG("Exynos-4210 UART: unexpected register size: %zx", size);
		return -1;
	}

	pbase = virt_to_phys((void *)vbase);
	exynos4210_uart_init(pd, pbase, 0, 0);

	return 0;
}

static void exynos4210_uart_dev_free(struct serial_chip *chip)
{
	struct exynos4210_uart_data *pd = 
			container_of(chip, struct exynos4210_uart_data, chip);

	free(pd);
}

static const struct serial_driver exynos4210_uart_driver = {
	.dev_alloc = exynos4210_uart_dev_alloc,
	.dev_init = exynos4210_uart_dev_init,
	.dev_free = exynos4210_uart_dev_free,
};

static const struct dt_device_match exynos4210_uart_match_table[] = {
	{ .compatible = "samsung,exynos4210-uart" },
	{ 0 }
};

const struct dt_driver exynos4210_uart_dt_driver __dt_driver = {
	.name = "exynos4210_uart",
	.match_table = exynos4210_uart_match_table,
	.driver = &exynos4210_uart_driver,
};

#endif /* CFG_DT */
