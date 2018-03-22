// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2018, ForgeRock Limited
 */

#include <platform_config.h>
#include <trace.h>
#include <console.h>
#include <drivers/pl011.h>
#include <kernel/generic_boot.h>
#include <kernel/panic.h>
#include <kernel/pm_stubs.h>
#include <mm/core_memprot.h>
#include <stdint.h>
#include <tee/entry_fast.h>
#include <tee/entry_std.h>

static void main_fiq(void);
void time_source_init(void);

static const struct thread_handlers handlers = {
	.std_smc = tee_entry_std,
	.fast_smc = tee_entry_fast,
	.nintr = main_fiq,
	.cpu_on = pm_do_nothing,
	.cpu_off = pm_do_nothing,
	.cpu_suspend = pm_do_nothing,
	.cpu_resume = pm_do_nothing,
	.system_off = pm_do_nothing,
/* 	.system_reset = pm_panic, */
	.system_reset = pm_do_nothing,
};

const struct thread_handlers *generic_boot_get_handlers(void)
{
	return &handlers;
}

static void main_fiq(void)
{
	panic();
}

static struct pl011_data console_data;
/*
 * Register the physical memory area for peripherals etc. Here we are
 * registering the UART (type PL011) console.
 */
register_phys_mem(MEM_AREA_IO_NSEC, CONSOLE_UART_BASE, PL011_REG_SIZE);

void console_init(void)
{
	pl011_init(&console_data, CONSOLE_UART_BASE, CONSOLE_UART_CLK_IN_HZ,
		   CONSOLE_BAUDRATE);
	register_serial_console(&console_data.chip);
	MSG("Making TEE funky... and making OP-TEE mildly more funky");
}

void time_source_init(void) {
	/* ToDo: this function sets up the trusted time source ... TBD */
}
