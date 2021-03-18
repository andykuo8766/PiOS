.global _start

.section .text

_start:
    adr 	x20, __bss_start
    adr 	x21, __bss_end
# clear bss
clear_bss_loop:
  	cmp 	x20, x21
  	b.eq 	loop_end
  	str 	xzr, [x20]
  	add 	x20, x20, #8
  	b 		clear_bss_loop
  	
loop_end:
  	ldr 	x20, =_start
  	mov 	sp, x20

  	bl 		main


