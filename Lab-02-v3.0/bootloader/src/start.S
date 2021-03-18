.section ".text.boot"

.global _start

_start:
    mrs 		  x0, mpidr_el1
    and 		  x0, x0, #3
    cbz 		  x0, main_cpu
proc_hang:
	b 		      proc_hang
	
main_cpu:

clear_bss_loop:
    adr x0, __bss_start
    adr x1, __bss_end
    sub x1, x1, x0
    bl  memzero
    mov sp, #4194304

	//Jump to main.c 
    bl            kernel_main
    b             proc_hang


.global branch_to_address
branch_to_address:
    br x0
    ret // We never reach this point


memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt memzero
    ret


