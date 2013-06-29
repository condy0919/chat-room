#ifndef _FILE_PACK_INCLUDED
#define _FILE_PACK_INCLUDED

#define MAX_FILE_PART_LENGTH (2 * 1024)
#define MAX_FILE_NAME_LENGTH (128)

/*
 *	compact: if the order is -1, it means end 
 *			 if the file_size is -1, same with the upon one
 */

struct file_pack_t {
	int order;
	int length;
	unsigned char file_content[MAX_FILE_PART_LENGTH];
};

struct file_info_t {
	char file_name[MAX_FILE_NAME_LENGTH];
	int file_size;
};

#endif
