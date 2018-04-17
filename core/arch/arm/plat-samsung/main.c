// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2018, ForgeRock Limited
 */

#include <platform_config.h>
#include <trace.h>
#include <console.h>
#include <drivers/pl011.h>
#include <drivers/exynos4210_uart.h>
#include <kernel/generic_boot.h>
#include <kernel/panic.h>
#include <kernel/pm_stubs.h>
#include <kernel/time_source.h>
#include <mm/core_memprot.h>
#include <stdint.h>
#include <tee/entry_fast.h>
#include <tee/entry_std.h>

static void main_fiq(void);
static void funky_banner(void);
 
static const struct thread_handlers handlers = {
	.std_smc = tee_entry_std,
	.fast_smc = tee_entry_fast,
	.nintr = main_fiq,
	.cpu_on = pm_panic,
	.cpu_off = pm_panic,
	.cpu_suspend = pm_panic,
	.cpu_resume = pm_panic,
	.system_off = pm_panic,
	.system_reset = pm_panic,
};

const struct thread_handlers *generic_boot_get_handlers(void)
{
	return &handlers;
}

static void main_fiq(void)
{
	panic();
}

/*
 * Register the physical memory areas for peripherals
 * such as UART console, etc
 */
register_nsec_ddr(DRAM0_BASE, DRAM0_SIZE_NSEC);

#if defined(PLATFORM_FLAVOR_artik520)
// ARTIK520 has UART type Samsung Exynos 4210 to provide console output
static struct exynos4210_uart_data console_data;
register_phys_mem(MEM_AREA_IO_NSEC, CONSOLE_UART_BASE, EXYNOS4210_UART_REG_SIZE);

void console_init(void)
{
	exynos4210_uart_init(&console_data,
			CONSOLE_UART_BASE,
			CONSOLE_UART_CLK_IN_HZ,
			CONSOLE_BAUDRATE);
	register_serial_console(&console_data.chip);
	funky_banner();
}
#elif defined(PLATFORM_FLAVOR_artik530)
// ARTIK530 has UART type PL011 to provide console output
static struct pl011_data console_data;
register_phys_mem(MEM_AREA_IO_NSEC, CONSOLE_UART_BASE, PL011_REG_SIZE);

void console_init(void)
{
	pl011_init(&console_data,
			CONSOLE_UART_BASE,
			CONSOLE_UART_CLK_IN_HZ,
			CONSOLE_BAUDRATE);
	register_serial_console(&console_data.chip);
	funky_banner();
}
#else
#  error "Unknown ARTIK platform PLATFORM_FLAVOR - no console defined"
#endif

static void funky_banner(void)
{
	IMSG("\nMaking TEE funky... and making funky OP-TEE mildly more funkier");
	DMSG("DRAM0_BASE:          0x%08X", DRAM0_BASE);
	DMSG("DRAM0_SIZE_NSEC:     0x%08X", DRAM0_SIZE_NSEC);
	DMSG("DRAM0_SIZE:          0x%08X", DRAM0_SIZE);
	DMSG("TZDRAM_BASE:         0x%08X", TZDRAM_BASE);
	DMSG("TZDRAM_SIZE:         0x%08X", TZDRAM_SIZE);
	DMSG("CFG_TEE_RAM_START:   0x%08X", CFG_TEE_RAM_START);
	DMSG("CFG_TEE_LOAD_ADDR:   0x%08X", CFG_TEE_LOAD_ADDR);
	DMSG("CFG_TEE_RAM_VA_SIZE: 0x%08X", CFG_TEE_RAM_VA_SIZE);
	DMSG("CFG_TEE_RAM_PH_SIZE: 0x%08X", CFG_TEE_RAM_PH_SIZE);
	DMSG("CFG_TA_RAM_START:    0x%08X", CFG_TA_RAM_START);
	DMSG("CFG_TA_RAM_SIZE:     0x%08X", CFG_TA_RAM_SIZE);
	DMSG("CFG_SHMEM_START:     0x%08X", CFG_SHMEM_START);
	DMSG("CFG_SHMEM_SIZE:      0x%08X", CFG_SHMEM_SIZE);
}

void time_source_init(void) {
    IMSG("NOTE: not sure why time_source_init(void) is needed");
}

void debug_reset_primary(int smc_call)
{
	FMSG("Enabled Secure Monitor interrupts, SMC = 0x%X", smc_call);
}

