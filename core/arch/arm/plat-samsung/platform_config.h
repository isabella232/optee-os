/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2018, ForgeRock Limited
 */

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

#define STACK_ALIGNMENT			64

/* Platform specific defines */
#if defined(PLATFORM_FLAVOR_artik520)
#  define DRAM0_BASE                    0x40000000
#  define CFG_DDR_TEETZ_RESERVED_START  0x57000000
   /* ARTIK520 has dual core Cortex-A7 */
#  define CFG_TEE_CORE_NB_CORE          2
   /* s3c2410_serial */
#  define BASEADDR_UART0		           (0x13800000)
   /* s3c2410_serial /dev/ttySAC1 zigbee daemon */
#  define BASEADDR_UART1		           (0x13810000)
   /* s3c2410_serial /dev/ttySAC2 debug console */
#  define BASEADDR_UART2		           (0x13820000)
   /* s3c2410_serial */
#  define BASEADDR_UART3		           (0x13830000)
   /* console UART defined for /dev/ttySAC2 */
#  define CONSOLE_UART_BASE             BASEADDR_UART2
#  define CONSOLE_BAUDRATE              115200
   /* UART clocks set to 147.5 MHz (147500000 Hz) ToDo: Check This!*/
#  define CONSOLE_UART_CLK_IN_HZ        0x08caabe0

#elif defined(PLATFORM_FLAVOR_artik530)
#  define DRAM0_BASE                    0x91000000
#  define CFG_DDR_TEETZ_RESERVED_START  0xA8000000
   /* ARTIK530 has quad core Cortex-A9 */
#  define CFG_TEE_CORE_NB_CORE          4
   /* dma (O), modem(X), UART0_MODULE */
#  define BASEADDR_UART0		           (0xC00A1000)
   /* dma (O), modem(O), pl01115_Uart_modem_MODULE */
#  define BASEADDR_UART1		           (0xC00A0000)
   /* dma (O), modem(X), UART1_MODULE */
#  define BASEADDR_UART2		           (0xC00A2000)
   /* dma (X), modem(X), pl01115_Uart_nodma0_MODULE */
#  define BASEADDR_UART3		           (0xC00A3000)
   /* dma (X), modem(X), pl01115_Uart_nodma1_MODULE */
#  define BASEADDR_UART4		           (0xC006D000)
   /* dma (X), modem(X), pl01115_Uart_nodma2_MODULE */
#  define BASEADDR_UART5		           (0xC006F000)
   /* console UART defined for /dev/ttyAMA3 */
#  define CONSOLE_UART_BASE             BASEADDR_UART3
#  define CONSOLE_BAUDRATE              115200
   /* UART clocks set to 50 MHz (50000000 Hz) */
#  define CONSOLE_UART_CLK_IN_HZ        0x02FAF080

#else
#error "Unknown platform PLATFORM_FLAVOR"
#endif

/* memory 63 + 1 MB */
#define CFG_DDR_TEETZ_RESERVED_SIZE  0x03F00000
#define CFG_TEE_RAM_VA_SIZE          (1024 * 1024)
#define CFG_PUB_RAM_SIZE             (1024 * 1024)

/* ARTIK 520/530/530S has 512MB DRAM (Note that 530S-1GB doubles this size) */
#define DRAM0_SIZE           0x1F000000
#define DDR_PHYS_START       DRAM0_BASE
#define DDR_SIZE             DRAM0_SIZE
#define CFG_DDR_START        DDR_PHYS_START
#define CFG_DDR_SIZE         DDR_SIZE

#ifndef CFG_DDR_TEETZ_RESERVED_START
#error "TEETZ reserved DDR start address undef: CFG_DDR_TEETZ_RESERVED_START"
#endif
#ifndef CFG_DDR_TEETZ_RESERVED_SIZE
#error "TEETZ reserved DDR size undefined: CFG_DDR_TEETZ_RESERVED_SIZE"
#endif

/*
 * TEE/TZ RAM layout:
 *
 *  +-----------------------------------------+  <- DRAM0_BASE
 *  |  Linux memory                           |
 *  +-----------------------------------------+  <- CFG_DDR_TEETZ_RESERVED_START
 *  | TEETZ private RAM  |  TEE_RAM  1MiB     |   ^
 *  |                    +--------------------+   |
 *  |                    |  TA_RAM   61MiB    |   |
 *  +-----------------------------------------+   | CFG_DDR_TEETZ_RESERVED_SIZE
 *  |                    |      teecore alloc |   |
 *  |  TEE/TZ and NSec   |  PUB_RAM  1MiB ----|   |
 *  |   shared memory    |         NSec alloc |   |
 *  +-----------------------------------------+   v
 *  |  Linux memory (Factory info 64MiB)      |
 *  +-----------------------------------------+  <- DRAM0_BASE + DRAM0_SIZE
 *  
 *  TEE_RAM : 1MByte
 *  PUB_RAM : 1MByte
 *  TA_RAM  : 61MByte
 *  Total   : 63MiB
 */

/* define the several memory area sizes */
#if (CFG_DDR_TEETZ_RESERVED_SIZE < (4 * 1024 * 1024))
#error "Invalid CFG_DDR_TEETZ_RESERVED_SIZE: at least 4MB expected"
#endif

/* Full GlobalPlatform test suite requires CFG_SHMEM_SIZE to be at least 2MB */
#define CFG_TEE_RAM_PH_SIZE  (CFG_TEE_RAM_VA_SIZE)
#define CFG_TA_RAM_SIZE      (CFG_DDR_TEETZ_RESERVED_SIZE - \
                                CFG_TEE_RAM_PH_SIZE - CFG_PUB_RAM_SIZE)

/* define the secure/unsecure memory areas */
#define TZDRAM_BASE          (CFG_DDR_TEETZ_RESERVED_START)
#define TZDRAM_SIZE          (CFG_TEE_RAM_PH_SIZE + CFG_TA_RAM_SIZE)

#define CFG_SHMEM_START      (TZDRAM_BASE + TZDRAM_SIZE)
#define CFG_SHMEM_SIZE       (CFG_PUB_RAM_SIZE)

/* define the memory areas (TEE_RAM must start at reserved DDR start addr) */
#define CFG_TEE_RAM_START    (TZDRAM_BASE)
#define CFG_TA_RAM_START     (CFG_TEE_RAM_START + CFG_TEE_RAM_PH_SIZE)
#ifndef CFG_TEE_LOAD_ADDR
#define CFG_TEE_LOAD_ADDR    (CFG_TEE_RAM_START)
#endif

#endif /*PLATFORM_CONFIG_H*/
