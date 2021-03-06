#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

int main()
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = 0;
    int x = 0, y = 0;
    long int location = 0;
    int img_width=720;
    int img_height=576;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    int i;
    
    FILE *fp;
    fp=fopen("input.txt","r");
    unsigned char *dst_buf;
    dst_buf=(unsigned char *)malloc(img_width*img_height*3*sizeof(unsigned char));

    printf("1\n");
    for(i=0;i<img_width*img_height*3;i++){

	fscanf(fp,"%d\n",&dst_buf[i]);

    }
    fclose(fp);
    
printf("2\n");

    i=0;
	
    for (y = 0; y < 576; y++){
        for (x = 0; x < 720; x++) {

            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                       (y+vinfo.yoffset) * finfo.line_length;

            if (vinfo.bits_per_pixel == 32) {
                *(fbp + location)     =dst_buf[i+2];                // red
                *(fbp + location + 1) =dst_buf[i+1];                // green
                *(fbp + location + 2) =dst_buf[i];                // blue
                *(fbp + location + 3) =0xff;                // alpha- No transparency
        //location += 4;
            }
            i+=3;
       }
   }

   printf("3\n");
    munmap(fbp, screensize);
    close(fbfd);
    return 0;

}
