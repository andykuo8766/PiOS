.section ".text.boot"

_start:
	  adr x1, DTB_ADDR
	  str x0, [x1]
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
	bl set_exception_vector_table
	

	ldr			x0, =__bss_begin
	ldr			x1, =__bss_end
	sub			x1, x1, x0
	ldr    		x1, =__kernel_end
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
	msr elr_el2, x30
	eret // return to EL1
	
.global from_el1_to_el0
from_el1_to_el0:
	  msr spsr_el1, xzr
	  msr elr_el1, x1
	  msr sp_el0, x2
	  mov x1, x2
	  eret
	  
.global KERNEL_ADDR_END
.global INITRAMFS_ADDR
.global INITRAMFS_ADDR_END
.global DTB_ADDR

.data
  KERNEL_ADDR_END: .xword 0
  INITRAMFS_ADDR: .xword 0
  INITRAMFS_ADDR_END: .xword 0
  DTB_ADDR: .xword 0

