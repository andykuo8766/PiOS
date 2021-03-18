#include "uart.h"
#include "string.h"



typedef struct{
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
}__attribute__((packed)) cpio_newc_header ;

/* cpio hpodc format */
typedef struct {
    char magic[6];      /* Magic header '070707'. */
    char dev[6];        /* device number. */
    char ino[6];        /* "i-node" number. */
    char mode[6];       /* Permisions. */
    char uid[6];        /* User ID. */
    char gid[6];        /* Group ID. */
    char nlink[6];      /* Number of hard links. */
    char rdev[6];       /* device major/minor number. */
    char mtime[11];     /* Modification time. */
    char namesize[6];   /* Length of filename in bytes. */
    char filesize[11];  /* File size. */
} __attribute__((packed)) cpio_t;

/**
 * List the contents of an archive
 */
void initrd_list(char *buf){
    //uart_puts("Type     Offset   Size     Access rights\tFilename\n");
    //uart_puts(buf);
    
	uart_puts("Type 	Offset	 Size	  Access rights\tFilename\n");
	uart_puts("\n");
	// if it's a cpio archive. Cpio also has a trailer entry
    while(!memcmp(buf,"070707",6) && memcmp(buf+sizeof(cpio_t),"TRAILER!!",9)) {
        cpio_t *header = (cpio_t*)buf;
		
		uart_puts("header->c_namesize\n");
        int ns=oct2bin(header->namesize,6);
		uart_puts(header->namesize);
		uart_puts("\n");	
		uart_puts("header->c_filesize\n");		
        int fs=oct2bin(header->filesize,11);
		uart_puts(header->filesize);
		uart_puts("\n");
		
        // print out meta information
        uart_hex(oct2bin(header->mode,6));  // mode (access rights + type)
        uart_send(' ');
        uart_hex((unsigned int)((unsigned long)buf)+sizeof(cpio_t)+ns);
        uart_send(' ');
        uart_hex(fs);                       // file size in hex
        uart_send(' ');
        uart_hex(oct2bin(header->uid,6));   // user id in hex
        uart_send('.');
        uart_hex(oct2bin(header->gid,6));   // group id in hex
        uart_send('\t');
        uart_puts(buf+sizeof(cpio_t));      // filename
        uart_puts("\n");
        // jump to the next file
        buf+=(sizeof(cpio_t)+ns+fs);
    }
    // if it's a cpio archive. Cpio also has a trailer entry
	while(!memcmp(buf,"070701",6)){
		cpio_newc_header *header = (cpio_newc_header*)buf;
		uart_puts("buf\n");
		uart_puts(buf);
		uart_puts("\n");	

	
		uart_puts("header->c_namesize\n");
        int ns=oct2bin(header->c_namesize,8);
		uart_puts(header->c_namesize);
		uart_puts("\n");	
		uart_puts("header->c_filesize\n");		
        int fs=oct2bin(header->c_filesize,8);
		uart_puts(header->c_filesize);
		uart_puts("\n");
		
		buf+=(sizeof(cpio_newc_header)+ns+fs);
		//if(!memcmp(buf+sizeof(cpio_newc_header),"TRAILER!!",9))break;
	}
	/*
    while(!memcmp(buf,"070701",6) && memcmp(buf+sizeof(cpio_newc_header),"TRAILER!!",9)) {
        cpio_newc_header *header = (cpio_newc_header*)buf;
		uart_puts("header->c_namesize\n");
        int ns=oct2bin(header->c_namesize,8);
		uart_puts(header->c_namesize);
		uart_puts("\n");	
		uart_puts("header->c_filesize\n");		
        int fs=oct2bin(header->c_filesize,8);
		uart_puts(header->c_filesize);
		uart_puts("\n");
   
		
        // print out meta information
        uart_hex(oct2bin(header->c_mode,8));  // mode (access rights + type)
        uart_send(' ');
        uart_hex((unsigned int)((unsigned long)buf)+sizeof(cpio_newc_header)+ns);
        uart_send(' ');
        uart_hex(fs);                       // file size in hex
        uart_send(' ');
        uart_hex(oct2bin(header->c_uid,8));   // user id in hex
        uart_send('.');
        uart_hex(oct2bin(header->c_gid,8));   // group id in hex
        uart_send('\t');
        uart_puts(buf+sizeof(cpio_newc_header));      // filename
        uart_puts("\n");
        // jump to the next file
        buf+=(sizeof(cpio_newc_header)+ns+fs);
    }
    */
}
