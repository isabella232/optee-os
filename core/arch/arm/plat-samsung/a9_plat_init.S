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
	// for now do nothing and return from this function call
	bx	lr
UNWIND(	.fnend)
END_FUNC plat_cpu_reset_early

/* ARTIK platform specific function to calculate core position. */
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


FUNC debug_break , :
UNWIND(	.fnstart)

	mov r0, #SCR_AW
	write_scr r0
	mov_imm r0, 0x00004000
	write_sctlr r0

	mov_imm r0, 0x00000041
	write_actlr r0

	mov_imm r0, 0x00020C00
	write_nsacr r0

	read_pcr r0
	orr r0, r0, #0x1
	write_pcr r0

	mov pc, lr
UNWIND(	.fnend)
END_FUNC debug_break