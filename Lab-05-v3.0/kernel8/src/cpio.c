#include "cpio.h"

extern unsigned long INITRAMFS_ADDR;
extern unsigned long INITRAMFS_ADDR_END;

void cpio_init(){
    const unsigned int *prop = dtb_get_node_prop_addr("chosen", NULL);
    if(!prop){
        uart_puts("Error: dtb has no 'chosen' node!!!\n");
        return;
    }
    unsigned int value;
    int n = dtb_get_prop_value("linux,initrd-start", prop, &value);
    if(n == -1){
        uart_puts("Error: 'chosen' node has no 'linux,initrd-start' property!!!\n");
        return;
    }
    if(n != 4){
        //printf("Error: 'linux,initrd-start' property gives size of %d!!!" NEW_LINE, n);
        uart_puts("Error: 'linux,initrd-start' property gives size of not 4!!!\n");
        return;
    }
    INITRAMFS_ADDR = (unsigned long)bswap32(value);

    n = dtb_get_prop_value("linux,initrd-end", prop, &value);
    if(n == -1){
        uart_puts("Error: 'chosen' node has no 'linux,initrd-end' property!!!\n");
        return;
    }
    if(n != 4){
        //printf("Error: 'linux,initrd-end' property gives size of %d!!!" NEW_LINE, n);
        uart_puts("Error: 'linux,initrd-end' property gives size of not 4!!!\n");
        return;
    }
    INITRAMFS_ADDR_END = (unsigned long)bswap32(value);
}

















unsigned long align(unsigned long  reminder, unsigned long  base){
    return (base - (reminder % base)) % base; 
}


void extract_header(cpio_newc_header *cpio_addr,cpio_info *size_info){
    size_info->file_size = hextoint(cpio_addr->c_filesize, 8);
    size_info->file_align = align(size_info->file_size, 4);
    size_info->name_size = hextoint(cpio_addr->c_namesize, 8);
    size_info->name_align = align(size_info->name_size + CPIO_SIZE, 4);
    size_info->offset = CPIO_SIZE + size_info->file_size + size_info->file_align + size_info->name_size + size_info->name_align;
}



void cpio_list(){
    char *now_ptr = CPIO_ADDR;
    cpio_newc_header *cpio_addr = (cpio_newc_header* )now_ptr;
    cpio_info size_info;
    while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + CPIO_SIZE);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        uart_puts(pathname);
        uart_puts("\r\n");
        now_ptr += size_info.offset;//next_addr_offset;
        cpio_addr = (cpio_newc_header* )now_ptr;
           
    }
}

void cpio_cat(char *args){
    char *now_ptr = CPIO_ADDR;
    cpio_newc_header *cpio_addr = (cpio_newc_header* )now_ptr;
    cpio_info size_info;
    int flag = 0;
    while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + CPIO_SIZE);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        if(strcmp(args, pathname) == 0){
			uart_puts("CPIO address :");
			uart_put_hex((unsigned long)cpio_addr);
			uart_puts("\r\n\r\n");
            uart_puts_bySize((char*)((char*)cpio_addr + CPIO_SIZE + size_info.name_size + size_info.name_align), size_info.file_size);
            uart_puts("\r\n");
            flag = 1;
        }
        now_ptr += size_info.offset;
        cpio_addr = (cpio_newc_header* )now_ptr;
    }
    if(flag == 0){
        uart_puts("No such file: ");
        uart_puts(args);
        uart_puts("\r\n");
    }
}

char* find_app_addr(char* target){
    char *now_ptr = CPIO_ADDR;
    cpio_newc_header *cpio_addr = (cpio_newc_header* )now_ptr;
    cpio_info size_info;
	while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + CPIO_SIZE);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        if(strcmp(target, pathname) == 0){
			//uart_puts("CPIO address :");
			//uart_put_hex((unsigned long)cpio_addr);
			//uart_puts("\r\n\r\n");
            //uart_puts_bySize((char*)((char*)cpio_addr + CPIO_SIZE + size_info.name_size + size_info.name_align), size_info.file_size);
            //uart_puts("\r\n");
            return (char*)((char*)cpio_addr + CPIO_SIZE + size_info.name_size + size_info.name_align);
        }
        now_ptr += size_info.offset;
        cpio_addr = (cpio_newc_header* )now_ptr;
	}
	return NULL;
}

int find_app_size(char* target){
    char *now_ptr = CPIO_ADDR;
    cpio_newc_header *cpio_addr = (cpio_newc_header* )now_ptr;
    cpio_info size_info;
	while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + CPIO_SIZE);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        if(strcmp(target, pathname) == 0){
			//uart_puts("CPIO address :");
			//uart_put_hex((unsigned long)cpio_addr);
			//uart_puts("\r\n\r\n");
            //uart_puts_bySize((char*)((char*)cpio_addr + CPIO_SIZE + size_info.name_size + size_info.name_align), size_info.file_size);
            //uart_puts("\r\n");
            return size_info.file_size;
        }
        now_ptr += size_info.offset;
        cpio_addr = (cpio_newc_header* )now_ptr;
	}
	return 0;
}

void load_app(char *args){
	char *new_addr_ptr = NEW_ADDR;
	char *new_sp_ptr = NEW_SP;
	char* app_addr = find_app_addr(args);
	if(!app_addr){
		uart_puts("App not found!\n");
		return;
	}
	int app_size = find_app_size(args);
	uart_puts("APP address : ");
	uart_put_hex((unsigned long)app_addr);
	uart_puts("\r\n");
	uart_puts("APP size : ");
	uart_put_int(app_size);
	uart_puts("\r\n");
	//uart_puts_bySize((char*)app_addr, find_app_size(args));
	char* target=NEW_ADDR;
	while(app_size--){
		*target=*app_addr;
		target++;
		app_addr++;
	}
	//uart_puts_bySize(NEW_ADDR, find_app_size(args));
	
	uart_puts("loading app...\n");
	unsigned long target_addr = (unsigned long)new_addr_ptr;
	unsigned long target_sp = (unsigned long)new_sp_ptr;
	arm_core_timer_enable();
	//change exception level
	//asm volatile("mov x0, 0x3c0			\n");//disable interrupt
	asm volatile("mov x0, 0x340			\n");//enable interrupt
	asm volatile("msr spsr_el1, x0		\n");
	asm volatile("msr elr_el1, %0		\n"::"r"(target_addr));
	asm volatile("msr sp_el0, %0		\n"::"r"(target_sp));
	
	//enable the core timer’s interrupt
	//asm volatile("mov x0, 1				\n");
	//asm volatile("msr cntp_ctl_el0, x0	\n");
	//asm volatile("mrs x0, cntfrq_el0	\n");
	//asm volatile("add x0, x0, x0		\n");
	//asm volatile("msr cntp_tval_el0, x0	\n");
	//asm volatile("mov x0, 2				\n");
	//asm volatile("ldr x1, =0x40000040	\n");//#define CORE0_TIMER_IRQ_CTRL 0x40000040
	//asm volatile("str w0, [x1]			\n");
	

	asm volatile("eret					\n");


}

void *load_setup_argv(char *sp, char *const argv[]){
    if(!argv){
        sp -= 16;
        unsigned long *sp_u64 = (unsigned long *)sp;
        sp_u64[0] = 0;
        sp_u64[1] = 0;
        return sp;
    }

    unsigned long argc = 0;
    char *arg = argv[argc];
    while(arg){
        argc++;
        arg = argv[argc];
    }
    char *argv_temp[argc];
    for(unsigned int i=1; i<=argc; i++){
        unsigned int len = strlen(argv[argc - i]) + 1;
        sp -= ROUNDUP_MUL8(len);
        for(unsigned int j=0; j<len; j++){
            sp[j] = argv[argc - i][j];
        }
        argv_temp[argc - i] = sp;
    }
    sp -= ROUNDUP_MUL16((argc + 3) * 8);
    unsigned long *sp_u64 = (unsigned long *)sp;
    sp_u64[0] = argc;
    sp_u64[1] = (unsigned long)&sp_u64[2];
    for(unsigned int i=0; i<argc; i++){
        sp_u64[i+2] = (unsigned long)argv_temp[i];
    }
    sp_u64[argc+2] = 0;
    return sp;
}


static void load(const char *find_filename, char *const argv[], Load_return *ret){
    ret->npage = 0;

    if(!find_filename){
        uart_puts("Usage: load <filename>\n");
        return;
    }

    if(INITRAMFS_ADDR == 0){
        uart_puts("Error: initramfs not loaded!!!\n");
        return;
    }

    cpio_newc_header *blk = (cpio_newc_header *)INITRAMFS_ADDR;
    if(strncmp(blk->c_magic, CPIO_HEADER_MAGIC, sizeof(blk->c_magic))){
        uart_puts("Error: Invalid cpio's New ASCII Format!!!\n");
        return;
    }

    while(1){
        unsigned int namesize = hextoint(blk->c_namesize, sizeof(blk->c_namesize));
        char *name = ((char *)blk + sizeof(cpio_newc_header));
        if(!strcmp("TRAILER!!!", name)){
            break;
        }

        unsigned int filesize = hextoint(blk->c_filesize, sizeof(blk->c_filesize));
        char *file = (char *)ROUNDUP_MUL4((unsigned long)name + namesize);

        unsigned int mode = hextoint(blk->c_mode, sizeof(blk->c_mode));
        if(mode & CPIO_MODE_FILE){
            if(!strcmp(find_filename, name)){
                if(filesize){
                    unsigned long start_offset = elf_start_offset(file);
                    if(start_offset >= 0){
                        ret->npage = (((filesize + 0xFFFU) & ~(0xFFFU)) / PAGE_SIZE) + 1;
                        char *p = buddy_alloc(ret->npage);
                        if(p){
                            for(unsigned int i=0; i<filesize; i++){
                                p[i] = file[i];
                            }
                            ret->user_stack = p;
                            ret->sp = load_setup_argv((char *)p + (ret->npage * PAGE_SIZE), argv);
                            ret->pc = (void *)p + start_offset;
                        }else{
                            ret->npage = 0;
                            uart_puts("Error: Load failed!!!\n");
                        }
                    }else{
                        uart_puts("Error: Invalid ELF format!!!\n");
                    }
                }else{
                    uart_puts("\n");
                }
                return;
            }
        }

        blk = (cpio_newc_header *)ROUNDUP_MUL4((unsigned long)file + filesize);
    }
    uart_puts("No such file\n");
}


unsigned long exec(const char* name, char *const argv[]){
    Load_return ret;
    load(name, argv, &ret);
    if(ret.npage == 0){
        return -1;
    }

    unsigned long pid = fork();
    if(pid < 0){
        kfree(ret.user_stack);
        return -1;
    }

    if(pid == 0){
        Thread *child = current_thread();
        child->user_stack_npage = ret.npage;
        child->user_stack = ret.user_stack;
        from_el1_to_el0(ret.pc, load_template, ret.sp);
    }

    return 0;
}

