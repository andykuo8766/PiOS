#include "dtb.h"

extern unsigned long DTB_ADDR;

int dtb_node_name_strcmp(char *find,char *from){
    while(*find){
        if (*find != *from){
            break;
        }
        find++;
        from++;
    }
    if(*find == '\0' && *from == '@'){
        return *(unsigned char*)(find - 1) - *(unsigned char*)(from - 1);
    }
    return *(unsigned char*)find - *(unsigned char*)from;
}

void dtb_print_raw_token(unsigned int  *token){
    char buffer[10];
    uitoa(bswap32(*token), buffer, sizeof(buffer), 16);
    uart_puts("0x");
    uart_puts(buffer);
}

unsigned int  dtb_print_node_name(char *str, unsigned int  printing){
    unsigned int  len = 0;
    while(1){
        if(str[len] == '\0'){
            if(printing && !len){  //root node
				uart_send('/');
            }
            return len + 1;
        }
        if(printing){
            uart_send(str[len]);
        }
        len++;
    }
}

void dtb_print_string_list(char *str, unsigned int len){
    uart_send('\'');
    for(unsigned int   i=1; i<len; i++){
        if(str[i-1] == '\0'){
            if(str[i-2] != '\0'){
                uart_puts("\", \"");
            }
        }else{
            uart_send(str[i-1]);
        }
    }
    uart_puts("\"");
}

void dtb_print_prop(char *name, unsigned int *value, unsigned int len){
    if(!strcmp(name, "serial-number") ||
        !strcmp(name, "compatible") ||
        !strcmp(name, "model") ||
        !strcmp(name, "status") ||
        !strcmp(name, "bootargs") ||
        !strcmp(name, "pinctrl-names") ||
        !strcmp(name, "clock-names") ||
        !strcmp(name, "dma-names") ||
        !strcmp(name, "clock-output-names") ||
        !strcmp(name, "phy-names") ||
        !strcmp(name, "interrupt-names") ||
        !strcmp(name, "reg-names") ||
        !strcmp(name, "enable-method") ||
        !strcmp(name, "device_type") ||
        !strcmp(name, "label") ||
        !strcmp(name, "default-state") ||
        !strcmp(name, "linux,default-trigger") ||
        !strcmp(name, "regulator-name")
    ){
        dtb_print_string_list((char *)value, len);
        uart_send(';');
    }else{
        unsigned int ntoken = ROUNDUP_MUL4(len) >> 2;
        while(ntoken--){
            dtb_print_raw_token(value);
            uart_puts("  ");
            value++;
        }
    }
}

void dtb_traverse_node(unsigned int  *token,char *str_blk,char *find_node){
    unsigned int space = 0;
    char *node_name;
    while(1){
        if(bswap32(*token) == 0x1){  //begin node
            token++;
            if(space){
                uart_puts("\n");
                node_name = (char *)token;
                token += ROUNDUP_MUL4(dtb_print_node_name((char *)token, space)) >> 2;
				uart_puts(" {\n");
                space += 4;
            }else{
                if(!dtb_node_name_strcmp(find_node, (char *)token)){
                    space += 4;
                    node_name = (char *)token;
                    token += ROUNDUP_MUL4(dtb_print_node_name((char *)token, space)) >> 2;
                    uart_puts(" {\n");
                }else{
                    token += ROUNDUP_MUL4(dtb_print_node_name((char *)token, space)) >> 2;
                }
            }
        }else if(bswap32(*token) == 0x2){  //end node
            token++;
            if(space){
                space -= 4;
                uart_puts("};\n");
            }
        }else if(bswap32(*token) == 0x9){  //end tree
            break;
        }else if(bswap32(*token) == 0x3){  //property
            token++;
            fdt_prop_entry *prop_desc = (fdt_prop_entry *)token;
            token += sizeof(fdt_prop_entry) >> 2;
            if(space){
				uart_puts(str_blk);
				uart_puts(bswap32(prop_desc->nameoff));
                uart_puts(" = ");
                if(!strcmp(node_name, "aliases") ||
                    !strcmp(node_name, "__symbols__")
                ){
                    dtb_print_string_list((char *)token, bswap32(prop_desc->len));
                    uart_send(';');
                }else if(!strcmp(node_name, "__overrides__")){
                    dtb_print_raw_token(token);
                    if(bswap32(prop_desc->len) > 4){
                        uart_send(' ');
                        dtb_print_string_list((char *)(token + 1), bswap32(prop_desc->len) - 4);
                    }
                }else{
                    dtb_print_prop(str_blk + bswap32(prop_desc->nameoff), token, bswap32(prop_desc->len));
                }
                uart_puts("\n");
            }
            token += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
        }else{
            uart_puts("Error: Unknown FDT node!!!  " );
            dtb_print_raw_token(token);
            uart_puts("\n");
            token++;
        }
    }
}

void dtb_dump(char *find_node){
    fdt_header *blk = (fdt_header *)DTB_ADDR;
    if(bswap32(blk->magic) != DTB_MAGIC_NUM){
        uart_puts("Error: Invalid dtb Format!!!\n");
        return;
    }
    dtb_traverse_node((unsigned int *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct)), (char *)DTB_ADDR + bswap32(blk->off_dt_strings), find_node);
}

unsigned int dtb_is_valid_node(unsigned int *node){
    fdt_header *blk = (fdt_header *)DTB_ADDR;
    if(bswap32(blk->magic) != DTB_MAGIC_NUM){
        uart_puts("Error: Invalid dtb Format!!!\n");
        return 0;
    }
    unsigned int *struct_blk = (unsigned int *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct));
    unsigned int *struct_blk_end = (unsigned int *)((char *)struct_blk + bswap32(blk->size_dt_struct));

    if(((unsigned long)node - DTB_ADDR) & 0x3){
        uart_puts("Error: Invalid dtb node address!!!");
        return 0;
    }
    if((node < struct_blk) || (node > struct_blk_end)){
        uart_puts("Error: Invalid dtb node address!!!");
        return 0;
    }
    return 1;
}

unsigned int *dtb_get_node_prop_addr(char *node_name, unsigned int *starting_token){
    if(!starting_token){
        fdt_header *blk = (fdt_header *)DTB_ADDR;
        if(bswap32(blk->magic) != DTB_MAGIC_NUM){
            uart_puts("Error: Invalid dtb Format!!!\n");
            return NULL;
        }
        starting_token = (unsigned int *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct));
    }else{
        if(!dtb_is_valid_node(starting_token)){
            return NULL;
        }
        while(bswap32(*starting_token) != 0x2 && bswap32(*starting_token) != 0x9){  //end node or end tree
            if(bswap32(*starting_token) == 0x3){
                starting_token++;
                fdt_prop_entry *prop_desc = (fdt_prop_entry *)starting_token;
                starting_token += sizeof(fdt_prop_entry) >> 2;
                starting_token += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
            }else{
                starting_token++;
            }
        }
    }

    unsigned int found = 0;
    while(1){
        if(bswap32(*starting_token) == 0x1){  //begin node
            starting_token++;
            if(!dtb_node_name_strcmp(node_name, (char *)starting_token)){
                found = 1;
            }
            starting_token += ROUNDUP_MUL4(dtb_print_node_name((char *)starting_token, 0)) >> 2;
        }else if(bswap32(*starting_token) == 0x3){  //property
            if(found){
                return starting_token;
            }
            starting_token++;
            fdt_prop_entry *prop_desc = (fdt_prop_entry *)starting_token;
            starting_token += sizeof(fdt_prop_entry) >> 2;
            starting_token += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
        }else if(bswap32(*starting_token) == 0x9){  //end tree
            return NULL;
        }else{
            starting_token++;
        }
    }
}

int dtb_get_prop_value(char *prop_name,unsigned int *prop_addr, void *data){
    if(!dtb_is_valid_node(prop_addr)){
        return -1;
    }
    char *str_blk = (char *)DTB_ADDR + bswap32(((fdt_header *)DTB_ADDR)->off_dt_strings);

    while(bswap32(*prop_addr) != 0x2 && bswap32(*prop_addr) != 0x9){  //end node or end tree
        prop_addr++;
        fdt_prop_entry *prop_desc = (fdt_prop_entry *)prop_addr;
        prop_addr += sizeof(fdt_prop_entry) >> 2;
        if(!strcmp(str_blk + bswap32(prop_desc->nameoff), prop_name)){
            strncpy(data, (char *)prop_addr, bswap32(prop_desc->len));
            return bswap32(prop_desc->len);
        }
        prop_addr += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
    }
    return -1;
}

int dtb_get_node_depth(unsigned int *node){
    if(!dtb_is_valid_node(node)){
        return -1;
    }
    fdt_header *blk = (fdt_header *)DTB_ADDR;
    unsigned int *token = (unsigned int *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct));

    int depth = 0;
    while(1){
        if(bswap32(*token) == 0x1){  //begin node
            token++;
            depth++;
            token += ROUNDUP_MUL4(dtb_print_node_name((char *)token, 0)) >> 2;
        }else if(bswap32(*token) == 0x2){  //end node
            token++;
            depth--;
        }else if(bswap32(*token) == 0x3){  //property
            if(token == node){
                return depth;
            }
            token++;
            fdt_prop_entry *prop_desc = (fdt_prop_entry *)token;
            token += sizeof(fdt_prop_entry) >> 2;
            token += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
        }else if(bswap32(*token) == 0x9){  //end tree
            break;
        }else{
            token++;
        }
    }
    return -1;
}

int dtb_get_parent_prop_value(char *prop_name,unsigned int *child, void *data){
    if(!dtb_is_valid_node(child)){
        return -1;
    }

    int node_depth = dtb_get_node_depth(child);
    if(node_depth == -1){
        return -1;
    }

    fdt_header *blk = (fdt_header *)DTB_ADDR;
    char *str_blk = (char *)DTB_ADDR + bswap32(blk->off_dt_strings);
    unsigned int *token = (unsigned int *)((char *)DTB_ADDR + bswap32(blk->off_dt_struct));

    int len, depth = 0;
    while(1){
        if(bswap32(*token) == 0x1){  //begin node
            token++;
            depth++;
            token += ROUNDUP_MUL4(dtb_print_node_name((char *)token, 0)) >> 2;
        }else if(bswap32(*token) == 0x2){  //end node
            token++;
            depth--;
        }else if(bswap32(*token) == 0x3){  //property
            if(token == child){
                return len;
            }
            token++;
            fdt_prop_entry *prop_desc = (fdt_prop_entry *)token;
            token += sizeof(fdt_prop_entry) >> 2;
            if(depth == node_depth - 1){
                if(!strcmp(str_blk + bswap32(prop_desc->nameoff), prop_name)){
                    strncpy(data, (char *)token, bswap32(prop_desc->len));
                    len = bswap32(prop_desc->len);
                }
            }
            token += ROUNDUP_MUL4(bswap32(prop_desc->len)) >> 2;
        }else if(bswap32(*token) == 0x9){  //end tree
            break;
        }else{
            token++;
        }
    }
    return -1;
}
