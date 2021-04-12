// save general registers to stack
.macro save_all
    sub sp, sp, 32 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
.endm

// load general registers from stack
.macro load_all
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.endm


.section ".text.boot"

__start:
    ldr     	x1, =0x31000000
    str     	x0, [x1]            // store dt loading address to 0x31000000

//cpu init
	mrs x1, mpidr_el1//read cpu id
	and x1, x1, #3
	cbz x1, master
	
proc_hang:
    wfe
	b 		    proc_hang

master:

//exception level init
	bl from_el2_to_el1

//exception table init
	ldr x0, =exception_vector_table
	msr vbar_el1, x0
	
	ldr			x0, =__bss_begin
	ldr			x1, =__bss_end
	sub			x1, x1, x0
	ldr    		x1, =__prog_end
	mov    		sp, x1
//jump to kernel_main	
	bl			kernel_main
//for failsafe
	b 			proc_hang		

//el2->el1
from_el2_to_el1:
	mov x0, (1 << 31) // EL1 uses aarch64
	msr hcr_el2, x0
	mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
	msr spsr_el2, x0
	msr elr_el2, x30//x30 = link register

	// disable SIMD traps: built-ins of uart_printf will use SIMD
	// https://github.com/bztsrc/raspi3-tutorial/tree/master/12_printf
	mov x0, #(3 << 20)
	msr cpacr_el1, x0
	eret // return to EL1
	
.align 11 // vector table should be aligned to 0x800
exception_vector_table:

  b exception_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b exception
  .align 7
  b exception
  .align 7
  b exception
  
  .align 7
  b exception
  .align 7
  b exception
  .align 7
  b exception
  .align 7
  b exception
  
  .align 7
  b lowerSynchronous
  .align 7
  b lowerIRQ
  .align 7
  b exception
  .align 7
  b exception
  
  .align 7
  b exception
  .align 7
  b exception
  .align 7
  b exception
  .align 7
  b exception

  
//Synchronous from lower level
lowerSynchronous:
	save_all
	bl exception_handler
	load_all
	eret
	
//IRQ from lower level
lowerIRQ:
	save_all
	bl interrupt_handler
	load_all
	eret
	
exception:
    save_all
    bl error_handler
    load_all
    eret	
    

.globl memzero    
memzero:
    str 		xzr, [x0], #8
    subs 		x1, x1, #8
    b.gt 		memzero
    ret
    
.globl get_el
get_el:
	mrs 		x0, CurrentEL
	lsr 		x0, x0, #2
	ret
	