#include <stdio.h>
#include <stdlib.h>

#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> //For close
#include <fcntl.h> 	//For open
#include <sys/mman.h> //For memory map
#include <sys/ioctl.h> //For ioctl
#include <stdint.h> //For ioctl

#define WIDTH 320
#define HEIGHT 240
#define NUMPIXELS  (76799) //320*240-1
#define FILESIZE (153598) //16 bits per pixel

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

//Can screen and copyarea be global so that they don't have to be passed as parameters all the time???

struct fb_copyarea area;

void black_screen(uint16_t* screen, int in_fd, struct fb_copyarea in_area);
void update_display(uint16_t* screen, int in_dx, int in_dy, int in_width, int in_height, int in_fd, struct fb_copyarea in_area);

int main(int argc, char *argv[])
{		

	uint16_t *screen;
	int fd;
	struct stat sb;
	struct fb_copyarea area;
	uint32_t ch, write_buf[100], read_buf[10];
	//Can screen and copyarea be global so that they don't have to be passed as parameters all the time???

	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1)
	   handle_error("open");

	if (fstat(fd, &sb) == -1)           /* To obtain file size */
	   handle_error("fstat");

	screen = (uint16_t*)mmap(NULL, FILESIZE, PROT_READ, MAP_PRIVATE, fd, 0);
	if (screen == MAP_FAILED)
	   handle_error("mmap");


	munmap(screen, FILESIZE);
	close(fd);

	exit(EXIT_SUCCESS);

}

void black_screen(uint16_t* screen, int in_fd, struct fb_copyarea in_area)
{
	int i;
	//Draws the screen all black
	for (i = 0; i < WIDTH*HEIGHT; i++)
		{
			screen[i] = 0x0000;
		}

	update_display(screen, 0, 0, WIDTH, HEIGHT, in_fd, in_area);
}

void update_display(uint16_t* screen, int in_dx, int in_dy, int in_width, int in_height, int in_fd, struct fb_copyarea in_area)
{
	//Updates display
	struct fb_copyarea area = in_area;
	area.dx = in_dx;
	area.dy = in_dy;
	area.width = in_width;
	area.height = in_height;
	ioctl(in_fd, 0x4680, &area);
}