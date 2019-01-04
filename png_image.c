#include "png_image.h"

#include <setjmp.h>
#include <png.h>



#define PNG_READ_HEADER 4

int read_chunk_callback(png_structp ptr, png_unknown_chunkp chunk)
{
	return 1;
}

void read_row_callback(png_structp ptr, png_uint_32 row, int pass)
{

}

int png_read(const char* filename, png_datap data)
{
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) return PNG_ERROR_NO_FILE;

	fseek(fp, 0, SEEK_END);
	int fSize = ftell(fp);
	rewind(fp);
	unsigned char* header = (unsigned char*)malloc(PNG_READ_HEADER);
	fread(header, 1, PNG_READ_HEADER, fp);
	int is_png = !png_sig_cmp(header, 0, PNG_READ_HEADER);
	if (!is_png) return PNG_ERROR_NOT_PNG;

	png_structp png_ptr;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) return PNG_ERROR_CREATE_PNG_STRUCT;

	png_infop info_ptr;
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		printf("create info struct failed!\n");
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return PNG_ERROR_CREATE_INFO_STRUCT;
	}
	png_infop end_info;
	end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		printf("create end info failed!\n");
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return PNG_ERROR_CREATE_INFO_STRUCT;
	}
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		return PNG_ERROR_JMP;
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, PNG_READ_HEADER);
	// png_voidp user_chunk_ptr = png_get_user_chunk_ptr(png_ptr);
	// png_set_read_user_chunk_fn(png_ptr,user_chunk_ptr,read_chunk_callback);
	// png_set_keep_unknown_chunks(png_ptr,PNG_HANDLE_CHUNK_NEVER,NULL,0);

	//png_set_read_status_fn(png_ptr,read_row_callback);

	//low lever read
	png_read_info(png_ptr, info_ptr);
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
	// printf("width:%d,height:%d,bit_depth:%d,color_type:%d",width,height,bit_depth,color_type);

	png_bytep raw_data = (png_bytep)malloc(width*height * 4 * sizeof(png_byte));
	// printf("row bytes:%d\n",png_get_rowbytes(png_ptr,info_ptr));

	png_bytep *row_pointers = (png_bytep*)malloc(height * sizeof(png_bytep));
	for (int i = 0; i < height; i++)
	{
		row_pointers[i] = raw_data + i* (width * 4);
	}
	png_read_image(png_ptr, row_pointers);

	data->width = width;
	data->height = height;
	data->data = (char*)raw_data;
	// printf("png_read_success\n");

	//clean
	fclose(fp);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(row_pointers);

	return PNG_READ_SUCCESS;
}