#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/timeb.h>
#include <jpeglib.h>
 
#define JPEG_QUALITY 95 //图片质量
 
int Bmp2Jpg(const char *bmp_file, const char *jeg_file, const int width, const int height)
{
    FILE *fd;
    int ret;
    unsigned char *data;
    long sizeImage;
    int depth = 3;
    JSAMPROW * row_pointer;
    long rgb_index = 0;
    int i=0;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile;                       
    ;
    // Read bmp image data
    sizeImage = width*height*3;
    data = (unsigned char*)malloc(sizeImage);
    fd = fopen(bmp_file, "rb");
    if(!fd)
    {
        printf("ERROR1: Can not open the image.\n");
        free(data);
        return -1;
    }
   
    fseek(fd, 54, SEEK_SET); 
    ret = fread(data, sizeof(unsigned char)*sizeImage, 1, fd);
    if(ret == 0)
    {
        if(ferror(fd))
        {
            printf("\nERROR2: Can not read the pixel data.\n");
            free(data);
            fclose(fd);
            return -1;
        }
    }
 
    //Convert BMP to JPG
    cinfo.err = jpeg_std_error(&jerr);
    //* Now we can initialize the JPEG compression object.
    jpeg_create_compress(&cinfo);
 
    if ((outfile = fopen(jeg_file, "wb")) == NULL)
    {
        fprintf(stderr, "can't open %s\n", jeg_file);
        return -1;
    }
    jpeg_stdio_dest(&cinfo, outfile);
 
    cinfo.image_width = width;             //* image width and height, in pixels
    cinfo.image_height = height;
    cinfo.input_components = depth;    //* # of color components per pixel
    cinfo.in_color_space = JCS_RGB;     //* colorspace of input image
    jpeg_set_defaults(&cinfo);
    
     //Now you can set any non-default parameters you wish to.
     //Here we just illustrate the use of quality (quantization table) scaling:
 
    jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE ); //* limit to baseline-JPEG values
    jpeg_start_compress(&cinfo, TRUE);
    
    //一次写入
    int j=0;
    row_pointer = malloc(height*width*3);
    char * line[300];
 
    for(i=0;i<height;i++)
    {    
        unsigned char * lineData = NULL;
        lineData = malloc(width*3);
        line[i]=lineData;
        for(j=0;j<width;j++)
        {
            lineData[j*3+2] = data[rgb_index];
            rgb_index ++;
            lineData[j*3+1] = data[rgb_index];
            rgb_index ++;
            lineData[j*3+0] = data[rgb_index];
            rgb_index ++;
        }
        row_pointer[height-i-1] = lineData;      
    }
    jpeg_write_scanlines(&cinfo, row_pointer, height);
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    for (i=0; i<height; i++)
    {
        free(line[i]);
    }  
    free(row_pointer);
    free(data); 
    fclose(fd);
    fclose(outfile);
 
    return 0;
}