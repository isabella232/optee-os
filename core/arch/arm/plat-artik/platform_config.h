/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

#define STACK_ALIGNMENT			64

/* console uart define */
#define CONSOLE_UART_BASE        UART0_BASE
#define CONSOLE_BAUDRATE         115200
#define CONSOLE_UART_CLK_IN_HZ   200000000

/* Platform specific defines */
#define CFG_DDR_TEETZ_RESERVED_START  0xA8000000
/* memory 63 + 1 MB */
#define CFG_DDR_TEETZ_RESERVED_SIZE   0x03F00000
#define CFG_TEE_RAM_VA_SIZE          (1024 * 1024)
#define CFG_PUB_RAM_SIZE             (1024 * 1024)
#define CFG_TEE_CORE_NB_CORE          2

/* ARTIK has 512MB DRAM */
#define DRAM0_BASE           0x91000000
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
 *  |                    |  TA_RAM            |   |
 *  +-----------------------------------------+   | CFG_DDR_TEETZ_RESERVED_SIZE
 *  |                    |      teecore alloc |   |
 *  |  TEE/TZ and NSec   |  PUB_RAM  1MiB ----|   |
 *  |   shared memory    |         NSec alloc |   |
 *  +-----------------------------------------+   v
 *  |  Linux memory (Factory info)            |
 *  +-----------------------------------------+  <- DRAM0_BASE + DRAM0_SIZE
 *  
 *  TEE_RAM : 1MByte
 *  PUB_RAM : 1MByte
 *  TA_RAM  : all what is left (at least 2MByte !)
 */

/* define the several memory area sizes */
#if (CFG_DDR_TEETZ_RESERVED_SIZE < (4 * 1024 * 1024))
#error "Invalid CFG_DDR_TEETZ_RESERVED_SIZE: at least 4MB expected"
#endif

/* Full GlobalPlatform test suite requires CFG_SHMEM_SIZE to be at least 2MB */
#define CFG_TEE_RAM_PH_SIZE		CFG_TEE_RAM_VA_SIZE
#define CFG_TA_RAM_SIZE			(CFG_DDR_TEETZ_RESERVED_SIZE - \
					 CFG_TEE_RAM_PH_SIZE - CFG_PUB_RAM_SIZE)

/* define the secure/unsecure memory areas */
#define TZDRAM_BASE			(CFG_DDR_TEETZ_RESERVED_START)
#define TZDRAM_SIZE			(CFG_TEE_RAM_PH_SIZE + CFG_TA_RAM_SIZE)

#define CFG_SHMEM_START			(TZDRAM_BASE + TZDRAM_SIZE)
#define CFG_SHMEM_SIZE			 CFG_PUB_RAM_SIZE

/* define the memory areas (TEE_RAM must start at reserved DDR start addr) */
#define CFG_TEE_RAM_START	TZDRAM_BASE
#define CFG_TA_RAM_START		(CFG_TEE_RAM_START + CFG_TEE_RAM_PH_SIZE)
#ifndef CFG_TEE_LOAD_ADDR
#define CFG_TEE_LOAD_ADDR	CFG_TEE_RAM_START
#endif

#endif /*PLATFORM_CONFIG_H*/
