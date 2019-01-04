#pragma  once

typedef struct png_data_t {
	unsigned int width;
	unsigned int height;
	char* data;
}  *png_datap;

#define PNG_READ_SUCCESS 0
#define PNG_ERROR_NO_FILE 1
#define PNG_ERROR_NOT_PNG 2
#define PNG_ERROR_CREATE_PNG_STRUCT 3
#define PNG_ERROR_CREATE_INFO_STRUCT 4
#define PNG_ERROR_JMP 5

int png_read(const char* filename, png_datap data);