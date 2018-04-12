// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2018, ForgeRock Limited
 */

#include <platform_config.h>
#include <trace.h>
#include <console.h>
#include <drivers/pl011.h>
// no good #include <drivers/gic.h>
#include <kernel/generic_boot.h>
#include <kernel/panic.h>
#include <kernel/pm_stubs.h>
#include <mm/core_memprot.h>
#include <stdint.h>
#include <tee/entry_fast.h>
#include <tee/entry_std.h>

static void main_fiq(void);
void time_source_init(void);
void banner(void);
 
static const struct thread_handlers handlers = {
	.std_smc = tee_entry_std,
	.fast_smc = tee_entry_fast,
	.nintr = main_fiq,
/*
	.cpu_on = pm_do_nothing,
	.cpu_off = pm_do_nothing,
	.cpu_suspend = pm_do_nothing,
	.cpu_resume = pm_do_nothing,
	.system_off = pm_do_nothing,
	.system_reset = pm_do_nothing,
*/
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

static struct pl011_data console_data;
/*
 * Register the physical memory area for peripherals and registers
 * such as UART (type PL011) console, etc
 */
register_phys_mem(MEM_AREA_IO_NSEC, CONSOLE_UART_BASE, PL011_REG_SIZE);

register_nsec_ddr(DRAM0_BASE, DRAM0_SIZE_NSEC);

// no good register_phys_mem(MEM_AREA_IO_SEC, GIC_BASE, CORE_MMU_DEVICE_SIZE);

void console_init(void)
{
	pl011_init(&console_data, CONSOLE_UART_BASE, CONSOLE_UART_CLK_IN_HZ,
		   CONSOLE_BAUDRATE);
	register_serial_console(&console_data.chip);
	banner();
}

void time_source_init(void) {
	MSG("TODO: this function sets up the trusted time source ... TBD");
}


void main_init_gic(void)
{
	MSG("TODO: this function sets up the general interrupt controller ... TBD");
	/* no good 
	vaddr_t gicc_base;
	vaddr_t gicd_base;

	gicc_base = core_mmu_get_va(GIC_BASE + GICC_OFFSET, MEM_AREA_IO_SEC);
	gicd_base = core_mmu_get_va(GIC_BASE + GICD_OFFSET, MEM_AREA_IO_SEC);
	FMSG("Initialising Generic Interrupt Controller");

	if (!gicc_base || !gicd_base)
		panic();

	gic_init(&gic_data, gicc_base, gicd_base);
	itr_init(&gic_data.chip);
*/
}

void banner(void)
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

void debug_reset_primary(int smc_call)
{
	FMSG("Enabled Secure Monitor interrupts, SMC = 0x%X", smc_call);
}

