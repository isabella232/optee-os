/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2017, Texas Instruments
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
 * SUBSTITUTE GOODS OR SERVICES// LOSS OF USE, DATA, OR PROFITS// OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Entry points for the A9 init.
 * It is assumed no stack is available when these routines are called.
 * It is assumed each routine is called with return address in LR
 * and with ARM registers R0, R1, R2, R3 being scratchable.
 */

#include <arm.h>
#include <arm32.h>
#include <arm32_macros.S>
#include <arm32_macros_cortex_a9.S>
#include <asm.S>
#include <kernel/tz_ssvce_def.h>
#include <kernel/unwind.h>
#include <platform_config.h>

.section .text
.balign 4
.code 32

/*
 * Cortex A9 early configuration
 *
 * No stack usage
 * LR store return address
 * Trap CPU in case of error
 */
FUNC plat_cpu_reset_early , :
UNWIND(	.fnstart)

	/*
	 * Disallow NSec to mask FIQ [bit4: FW=0]
	 * Allow NSec to manage Imprecise Abort [bit5: AW=1]
	 * Imprecise Abort trapped to Abort Mode [bit3: EA=0]
	 * In Sec world, FIQ trapped to FIQ Mode [bit2: FIQ=0]
	 * IRQ always trapped to IRQ Mode [bit1: IRQ=0]
	 * Secure World [bit0: NS=0]
	 */
	mov r0, #SCR_AW

	/* ToDo: this can not be done unless U-Boot has launched OPTEE in
	 * secure mode. 'Normal' U-Boot jumps to supervisor mode, so we can
	 * not execute this instruction from this normal world - and we need
	 * a modified secure U-Boot */
	//ToDo write_scr r0		/* write Secure Configuration Register */

	mov pc, lr
UNWIND(	.fnend)
END_FUNC plat_cpu_reset_early

/* overload with ARTIK platform specific function to calculate core position */
FUNC get_core_pos_mpidr , :
UNWIND(	.fnstart)
	/* Note sure why yet, but if we do standard calculation
	 * Calculate CorePos = (ClusterId * (cores/cluster)) + CoreId 
	 * then we get the wrong core position from the ARTIK board
	 * Hence only calculate CorePos = CoreId 
	*/
	and	r0, r0, #MPIDR_CPU_MASK
	bx	lr
UNWIND(	.fnend)
END_FUNC get_core_pos_mpidr
