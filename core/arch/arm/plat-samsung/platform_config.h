/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2018, ForgeRock Limited
 */

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

#define STACK_ALIGNMENT			64

/* ARTIK 520/530/530S has 512MB DRAM (Note 530S-1GB doubles this size to 1GB) */
#define DRAM0_SIZE		0x1F000000
/* 64MB at top of memory map for factory info etc, also add memory for TEE */
#define DRAM0_SIZE_NSEC	0x17000000

/* Platform specific defines */
#if defined(PLATFORM_FLAVOR_artik520)
#  define DRAM0_BASE	0x40000000

   /* ARTIK520 has dual core Cortex-A7 */
#  define CFG_TEE_CORE_NB_CORE 2

   /* s3c2410_serial */
#  define BASEADDR_UART0			0x13800000
   /* s3c2410_serial /dev/ttySAC1 zigbee daemon */
#  define BASEADDR_UART1			0x13810000
   /* s3c2410_serial /dev/ttySAC2 debug console */
#  define BASEADDR_UART2			0x13820000
   /* s3c2410_serial */
#  define BASEADDR_UART3			0x13830000
   /* console UART defined for /dev/ttySAC2 */
#  define CONSOLE_UART_BASE			BASEADDR_UART2
#  define CONSOLE_BAUDRATE			115200
   /* EXYNOS4210 UART clocks set to 50Mz */
#  define CONSOLE_UART_CLK_IN_HZ	50000000

#elif defined(PLATFORM_FLAVOR_artik530)
#  define DRAM0_BASE	0x91000000

   /* ARTIK530 has quad core Cortex-A9 */
#  define CFG_TEE_CORE_NB_CORE 4

   /* dma (O), modem(X), UART0_MODULE */
#  define BASEADDR_UART0			0xC00A1000
   /* dma (O), modem(O), pl01115_Uart_modem_MODULE */
#  define BASEADDR_UART1			0xC00A0000
   /* dma (O), modem(X), UART1_MODULE */
#  define BASEADDR_UART2			0xC00A2000
   /* dma (X), modem(X), pl01115_Uart_nodma0_MODULE */
#  define BASEADDR_UART3			0xC00A3000
   /* dma (X), modem(X), pl01115_Uart_nodma1_MODULE */
#  define BASEADDR_UART4			0xC006D000
   /* dma (X), modem(X), pl01115_Uart_nodma2_MODULE */
#  define BASEADDR_UART5			0xC006F000
   /* console UART defined for /dev/ttyAMA3 */
#  define CONSOLE_UART_BASE			BASEADDR_UART3
#  define CONSOLE_BAUDRATE			115200
   /* UART clocks set to 50 MHz (50000000 Hz) */
#  define CONSOLE_UART_CLK_IN_HZ	0x02FAF080

#else
#  error "Unknown ARTIK platform PLATFORM_FLAVOR"
#endif

#define GIC_BASE			0x00A00000
#define GICD_OFFSET			0x1000
#define GICC_OFFSET			0x100

/*
 * Trusted Execution Environment / Trusted Zone memory layout:
 *
 *  +--------------------------------+ <-- DRAM0_BASE + DRAM0_SIZE
 *  | NSec Linux Factory Info 64MB   |      ^ 0x04000000
 *  +--------------------------------+ <----v
 *  | Unused                         |
 *  +--------------------------------+ <-----
 *  |                    |      TEE  |      ^
 *  | TEE/TZ and NSec    |   4MB ----|      | CFG_SHMEM_SIZE
 *  |   shared memory    |     NSec  |      v
 *  +--------------------------------+ <-- CFG_SHMEM_START         ^
 *  |                    |           |      ^                      |
 *  |                    |  TA_RAM   |      | CFG_TA_RAM_SIZE      |
 *  |                    |  16MB     |      v                      | TZDRAM_SIZE
 *  | TEE/TZ secure RAM  +-----------+ <-----                      |
 *  |                    |  TEE R/W  |      ^                      |
 *  |                    |   1MB ----|      | CFG_TEE_RAM_VA_SIZE  |
 *  |                    |  TEE R/X  |      v                      |
 *  +--------------------------------+ <-- TZDRAM_BASE             v
 *  |                                |      ^
 *  | NSec Linux, FDT, rootfs, etc   |      | DRAM0_SIZE_NSEC
 *  |                                |      v
 *  +--------------------------------+ <-- DRAM0_BASE
 *
 *  TEE RAM   :  1 MB
 *  TA RAM    : 16 MB
 *  SHMEM RAM :  4 MB
 *  Total     : 21 MB (allocated 64MB)
 */

/* define the secure/shared/not-secure memory areas */
#define CFG_TEE_RAM_VA_SIZE  (1024 * 1024)
#define CFG_TEE_RAM_PH_SIZE  (CFG_TEE_RAM_VA_SIZE)
#define CFG_TA_RAM_SIZE      (16 * 1024 * 1024)
#define TZDRAM_BASE          ((DRAM0_BASE) + (DRAM0_SIZE_NSEC))
#define TZDRAM_SIZE          ((CFG_TEE_RAM_PH_SIZE) + (CFG_TA_RAM_SIZE))
#define CFG_SHMEM_START      ((TZDRAM_BASE) + (TZDRAM_SIZE))
#define CFG_SHMEM_SIZE       (4 * 1024 * 1024)
/* Note TEE_RAM must start at reserved DDR start addr */
#define CFG_TEE_RAM_START    (TZDRAM_BASE)
#define CFG_TA_RAM_START     ((CFG_TEE_RAM_START) + (CFG_TEE_RAM_PH_SIZE))
#define CFG_TEE_LOAD_ADDR    (TZDRAM_BASE)

/* make sure the ARTIK flavour has been defined correctly */
#ifndef DRAM0_BASE
#  error "No TEE/TZ reserved DDR start address because DRAM0_BASE undefined"
#endif

/* ARTIK devices do not have SRAM, so ensure none is configured */
#ifdef TZSRAM_BASE
#  error "Invalid TZSRAM_BASE, ARTIK devices do not have SRAM "
#endif
#ifdef TZSRAM_SIZE
#  error "Invalid TZSRAM_SIZE, ARTIK devices do not have SRAM "
#endif

/* Full GlobalPlatform test suite requires CFG_SHMEM_SIZE to be at least 2MB */
#if (CFG_SHMEM_SIZE < (4 * 1024 * 1024))
#  error "Invalid CFG_SHMEM_SIZE: >= 4MB required for GlobalPlatform test suite"
#endif

#ifndef ASM
	/* ToDo: this is temporary debug */
	void debug_reset_primary(int smc_call);
#endif

#endif /*PLATFORM_CONFIG_H*/
