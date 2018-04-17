/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2018, Linaro Limited
 */
#ifndef EXYNOS4210_UART_H
#define EXYNOS4210_UART_H

#include <types_ext.h>
#include <drivers/serial.h>

#define EXYNOS4210_UART_REG_SIZE 0x100

struct exynos4210_uart_data {
	struct io_pa_va base;
	struct serial_chip chip;
};

void exynos4210_uart_init(struct exynos4210_uart_data *pd, paddr_t base,
			  uint32_t uart_clk, uint32_t baud_rate);

#endif /* EXYNOS4210_UART_H */

