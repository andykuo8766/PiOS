#ifndef _CPIO_H_
#define _CPIO_H_
#include "uart.h"
#include "utils.h"
#include "timer.h"
#include "mm.h"
#include "dtb.h"
#include "elf.h"

#define CPIO_ADDR ((char*)0x8000000)    //QEMU(0x8000000)
#define NEW_ADDR ((char*)0x8100000)    //QEMU(0x8000000)
#define NEW_SP ((char*)0x8200000) 
#define ROUNDUP_MUL4(num) ((num + 0x3) & ~0x3)
#define ROUNDUP_MUL8(num) ((num + 0x7) & ~0x7)
#define ROUNDUP_MUL16(num) ((num + 0xF) & ~0xF)

//#define CPIO_ADDR ((char*)0x30000000)


/* Magic identifiers for the "cpio" file format. */
#define CPIO_HEADER_MAGIC "070701"
#define CPIO_FOOTER_MAGIC "TRAILER!!!"
#define CPIO_ALIGNMENT 4
#define CPIO_MODE_FILE 0100000
#define CPIO_MODE_DIR 0040000


#define CPIO_SIZE 110

typedef struct  {
    char c_magic[6];      /* Magic header '070701'. */
    char c_ino[8];        /* "i-node" number. */
    char c_mode[8];       /* Permisions. */
    char c_uid[8];        /* User ID. */
    char c_gid[8];        /* Group ID. */
    char c_nlink[8];      /* Number of hard links. */
    char c_mtime[8];      /* Modification time. */
    char c_filesize[8];   /* File size. */
    char c_devmajor[8];   /* Major dev number. */
    char c_devminor[8];   /* Minor dev number. */
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];   /* Length of filename in bytes. */
    char c_check[8];      /* Checksum. */
}cpio_newc_header;

typedef struct{
    unsigned long file_size; 
	unsigned long file_align;
	unsigned long name_size; 
	unsigned long name_align;
    unsigned long offset;

} cpio_info ;

typedef struct {
	unsigned int npage;
    void *user_stack;
	void *pc;
	void *sp;
} Load_return;

void cpio_init();

unsigned long align(unsigned long  size, unsigned long  multiplier);
void extract_header(cpio_newc_header *cpio_addr, cpio_info *size_info);
void cpio_list();
void cpio_cat(char *args);
char* find_app_addr(char* target);
int find_app_size(char* target);
void load_app(char *args);
unsigned long exec(const char* name, char *const argv[]);


void from_el1_to_el0(void (*main)(), void (*load_t)(), void *sp);
void load_template(void (*main)(), unsigned long *sp);

#endif /* _CPIO_H_ */

