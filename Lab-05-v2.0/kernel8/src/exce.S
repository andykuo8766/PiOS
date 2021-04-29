#include "exce.h"

.macro handle_invalid_entry el, type
	kernel_entry \el
	mov	x0, #\type
	mrs	x1, esr_el1
	mrs	x2, elr_el1
	bl	show_invalid_entry_message
	b	err_hang
.endm

.macro	kernel_entry, el
	sub	sp, sp, #S_FRAME_SIZE
	stp	x0, x1, [sp, #16 * 0]
	stp	x2, x3, [sp, #16 * 1]
	stp	x4, x5, [sp, #16 * 2]
	stp	x6, x7, [sp, #16 * 3]
	stp	x8, x9, [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]

	.if	\el == 0
	mrs	x21, sp_el0
	.else
	add	x21, sp, #S_FRAME_SIZE
	.endif /* \el == 0 */

	mrs	x22, elr_el1
	mrs	x23, spsr_el1

	stp	x30, x21, [sp, #16 * 15] 
	stp	x22, x23, [sp, #16 * 16]
.endm

.macro	kernel_exit, el
	ldp	x22, x23, [sp, #16 * 16]
	ldp	x30, x21, [sp, #16 * 15] 

	.if	\el == 0
	msr	sp_el0, x21
	.endif /* \el == 0 */

	msr	elr_el1, x22			
	msr	spsr_el1, x23

	ldp	x0, x1, [sp, #16 * 0]
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	add	sp, sp, #S_FRAME_SIZE		
	eret
.endm

.macro exception_entry label
    .align 7
    b     \label                    // branch to a handler function.
    
.endm	

.align 11 // vector table should be aligned to 0x800
exception_vector_table:
	exception_entry	sync_invalid_el1t			// Synchronous EL1t
	exception_entry	irq_invalid_el1t			// IRQ EL1t
	exception_entry	fiq_invalid_el1t			// FIQ EL1t
	exception_entry	error_invalid_el1t			// Error EL1t

	exception_entry	sync_el1_h  	        		// Synchronous EL1h (this one)
	exception_entry	el1_irq_h						// IRQ EL1h
	exception_entry	fiq_invalid_el1h			// FIQ EL1h
	exception_entry	error_invalid_el1h			// Error EL1h

	exception_entry	sync_el0_64					// Synchronous 64-bit EL0
	exception_entry	irq_el0_64					// IRQ 64-bit EL0
	exception_entry	fiq_invalid_el0_64			// FIQ 64-bit EL0
	exception_entry	error_invalid_el0_64		// Error 64-bit EL0

	exception_entry	sync_invalid_el0_32			// Synchronous 32-bit EL0
	exception_entry	irq_invalid_el0_32			// IRQ 32-bit EL0
	exception_entry	fiq_invalid_el0_32			// FIQ 32-bit EL0
	exception_entry	error_invalid_el0_32		// Error 32-bit EL0

sync_invalid_el1t:
	handle_invalid_entry  1, SYNC_INVALID_EL1t
irq_invalid_el1t:
	handle_invalid_entry  1, IRQ_INVALID_EL1t
fiq_invalid_el1t:
	handle_invalid_entry  1, FIQ_INVALID_EL1t
error_invalid_el1t:
	handle_invalid_entry  1, ERROR_INVALID_EL1t
sync_invalid_el1h:
	handle_invalid_entry  1, SYNC_INVALID_EL1h
irq_invalid_el1h:
    handle_invalid_entry  1, IRQ_INVALID_EL1h
fiq_invalid_el1h:
	handle_invalid_entry  1, FIQ_INVALID_EL1h
error_invalid_el1h:
	handle_invalid_entry  1, ERROR_INVALID_EL1h
sync_invalid_el0_64:
	handle_invalid_entry  0, SYNC_INVALID_EL0_64
irq_invalid_el0_64:
	handle_invalid_entry  0, IRQ_INVALID_EL0_64
fiq_invalid_el0_64:
	handle_invalid_entry  0, FIQ_INVALID_EL0_64
error_invalid_el0_64:
	handle_invalid_entry  0, ERROR_INVALID_EL0_64
sync_invalid_el0_32:
	handle_invalid_entry  0, SYNC_INVALID_EL0_32
irq_invalid_el0_32:
	handle_invalid_entry  0, IRQ_INVALID_EL0_32
fiq_invalid_el0_32:
	handle_invalid_entry  0, FIQ_INVALID_EL0_32
error_invalid_el0_32:
	handle_invalid_entry  0, ERROR_INVALID_EL0_32


sync_el0_64:
	kernel_entry 0
    bl handle_sync_el0  
	str x0,	[sp, #0] //return value
	kernel_exit 0
    
sync_el1_h:
	kernel_entry 1
	bl	handle_sync_el1
	kernel_exit 1
	
irq_el0_64:
	kernel_entry 0
	mrs x0, esr_el1
	mrs x1, elr_el1
	bl	handle_el0_irq
	kernel_exit 0
	
el1_irq_h:
	kernel_entry 1
	bl	handle_el1_irq
	kernel_exit 1
	
.globl err_hang
err_hang: b err_hang

.global set_exception_vector_table
set_exception_vector_table:
    adr x0, exception_vector_table
    msr vbar_el1, x0
    ret




.globl get_el
get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret	


// enable interrupt
.globl irq_enable
irq_enable:
    msr DAIFClr, 0xf
    ret
    
// disable interrupt
.globl irq_disable
irq_disable:
    msr DAIFSet, 0xf
    ret

