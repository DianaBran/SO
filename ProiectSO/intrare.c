#include <stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>

//stocam header-ul BMP

typedef struct{
    unsigned int fileSize;
    unsigned int dataOffset
    unsigned int width;
    unsigned int height;
    
}BMPInfo

int main( int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s <file.bmp>\n", argv[0]);
        return 1;
    }

    //deschidere fisier

    int bmpFile = open(argv[1],O_RDONLY);

    if(bmpFile < 0)
    {
        perror("Eroare deschidere fisier\n");
        return 1;
    }

    lseek(bmpFile,10, SEEK_SET);

    BMPInfo bmpInfo;
    //citim offset date img
    if(read(bmpFile, &bmpInfo.dataOffset, 4)!=4)
    {
        perror("Eroare citire data offset");
        close(bmpFile);
        return 1;
    }

    lseek(bmpFile,4, SEEK_SET);

    //citim latimea si inaltimea 

    if(read(bmpFile, &bmpInfo.width, 4)!= 4 || read( bmpFile, &bmpInfo.height, 4)!= 4)
    {
        
    }


}