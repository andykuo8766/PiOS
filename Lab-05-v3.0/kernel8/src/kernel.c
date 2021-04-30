#include "uart.h"
#include "mbox.h"
#include "shell.h"
#include "mm.h"
#include "exce.h"
#include "sched.h"
#include "cpio.h"


void kernel_main(){
    // Get the amount of RAM
    mbox_vc_memory();
    mbox_arm_memory();
	
	//arm_core_timer_enable();
    // set up uart
    uart_init();
	
	cpio_init();
	// init memory system
    mm_init();




	


    // start shell
    shell();
	
    //while (1) {
        // Once we call schedule for the first time, since current points to the
        // init task, we become the init task. So, everytime init runs, it's
        // actually running this while loop. Here, we're voluntarily giving up
        // the cpu.
        //schedule();
    //}
}
