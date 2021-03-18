ENTRY(_start)
SECTIONS
{
    . = 0x60000;
    .text : { 
     	*(.bl_relo)
    	_loader_entry = .;
    	*(.text) 
    }
    .rodata : { 
    	*(.rodata) 
    }
    . = ALIGN(0x1000);
    .data : { 
    	*(.data*) 
    }
    . = ALIGN(0x1000);
    .bss:{
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    }
    
    . = ALIGN(0x1000);

}