#ifndef __DTB_H
#define __DTB_H

#include "uart.h"

#define DTB_MAGIC_NUM 0xd00dfeed
#define bswap32(num) __builtin_bswap32(num)
#define ROUNDUP_MUL4(num) ((num + 0x3) & ~0x3)

typedef struct {
    unsigned int magic;
    unsigned int totalsize;
    unsigned int off_dt_struct;     //offset in bytes of the structure block
    unsigned int off_dt_strings;    //offset in bytes of the strings block
    unsigned int off_mem_rsvmap;    //offset in bytes of the memory reservation block
    unsigned int version;
    unsigned int last_comp_version;
    unsigned int boot_cpuid_phys;
    unsigned int size_dt_strings;   //length in bytes of the strings block
    unsigned int size_dt_struct;    //length in bytes of the structure block
} fdt_header;

typedef struct {
    unsigned long address;
    unsigned long size;
} fdt_reserve_entry;

typedef struct {
    unsigned int len;
    unsigned int nameoff;
} fdt_prop_entry;

int dtb_node_name_strcmp(char *find, char *from);
void dtb_print_raw_token(unsigned int *token);
unsigned int dtb_print_node_name(char *str, unsigned int printing);
void dtb_print_string_list(char *str, unsigned int len);
void dtb_print_prop(char *name, unsigned int *value, unsigned int len);
void dtb_traverse_node(unsigned int *token,  char *str_blk,  char *find_node);
void dtb_dump(char *find_node);
unsigned int dtb_is_valid_node(unsigned int *node);
unsigned int *dtb_get_node_prop_addr(char *node_name, unsigned int *token);
int dtb_get_prop_value(char *prop_name, unsigned int *prop_addr, void *data);
int dtb_get_node_depth( unsigned int *node);
int dtb_get_parent_prop_value(char *prop_name, unsigned int *child, void *data);

#endif
