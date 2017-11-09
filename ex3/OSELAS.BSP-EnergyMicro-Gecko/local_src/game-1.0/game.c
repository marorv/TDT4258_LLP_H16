#include <stdio.h>
#include <stdlib.h>

#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 	//close method
#include <fcntl.h> 		//open method
#include <sys/mman.h> 	//For memory map
#include <sys/ioctl.h> 	//For ioctl
#include <stdint.h> 	//For ioctl
#include <inttypes.h> 	// for printing hex numbers

#include "graphics.h"

#define FILEPATH "/dev/fb0"
#define WIDTH 320
#define HEIGHT 240
#define NUMPIXELS  (76799) //320*240-1
#define FILESIZE (153598) //16 bits per pixel


#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)


void black_screen();
void drawCircle(int row, int column, uint16_t colour);
void drawBigCircle(int row, int column, uint16_t colour);
void update_display(int in_dx, int in_dy, int in_width, int in_height);

struct fb_copyarea area;
uint16_t *screen;
int fd;

int main(int argc, char *argv[])
{		

	printf("Hello World, I'm game!\n");


	int gpio_fd;
	struct stat sb;
	uint16_t read_buf;


	/*The following program prints part of the file specified in its first
       command-line argument to standard output.  The range of bytes to be
       printed is specified via offset and length values in the second and
       third command-line arguments.  The program creates a memory mapping
       of the required pages of the file and then uses write(2) to output
       the desired bytes.
	*/
	gpio_fd = open("/dev/GPIO_buttons", O_RDWR);
	if (gpio_fd == -1)
	   handle_error("open");
	printf("%s\n", "Opened GPIO_buttons");

	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1)
	   handle_error("open");

	if (fstat(fd, &sb) == -1)           /* To obtain file size */
	   handle_error("fstat");


	screen = (uint16_t*)mmap(NULL, FILESIZE, PROT_READ, MAP_PRIVATE, fd, 0);
	if (screen == MAP_FAILED)
	   handle_error("mmap");

	black_screen();



	int k = 1;
	int j = 0;
	int i;

	for(k=0; k<5; k+=1) //Do 5 lines downwards
	{
		for(j=3; j < WIDTH; j+=7) //Start at 3 to get entire circle
		{
			drawCircle((k*6)+3, j, Pink);
		}
	}

	while(j++ < 1000)
	{
		read(gpio_fd, &read_buf, sizeof(read_buf));

		read_buf = read_buf << 8;

		printf ("The data in the device is %x\n", read_buf);
		if (read_buf == 0xfe00)
		{
			printf("%s\n", "Making a line");
			for (i = 0; i < WIDTH*k - 1; i++)
			{
				screen[i] = 0xF800;
			}

			k++;

			//Update display
			printf("%s\n", "Updating display");
			area.dx = 0;
			area.dy = 0;
			area.width = WIDTH;
			area.height = HEIGHT;
			ioctl(fd, 0x4680, &area);
		}	
	}




	munmap(screen, FILESIZE);
	close(fd);
	close(gpio_fd);

	exit(EXIT_SUCCESS);

}

void drawCircle(int row, int column, uint16_t colour)
{
	int middle = (row * WIDTH + 1) + column;
	int i;

	for (i = 0; i<3; i++)
	{
		screen[(middle - 3*WIDTH-1) + i] = colour;
		screen[(middle + 3*WIDTH-1) + i] = colour;
	}

	for (i = 0; i<5; i++)
	{
		if(i == 0 || i == 4)
		{
			screen[(middle - 2*WIDTH-2) + i] = colour;
			screen[(middle + 2*WIDTH-2) + i] = colour;
		}
	}

	for (i = -1; i<2; i++)
	{
		screen[(middle - i*WIDTH-3)] = colour;
		screen[(middle - i*WIDTH+3)] = colour;
	}

	update_display(0, 0, WIDTH, HEIGHT);

}

void drawBigCircle(int row, int column, uint16_t colour)
{
	//Someone do this
}

void black_screen()
{
	//Draws the screen all black
	int i;
	for (i = 0; i < WIDTH*HEIGHT; i++)
		{
			screen[i] = 0x0000;
		}

	update_display(0, 0, WIDTH, HEIGHT);
}

void update_display(int in_dx, int in_dy, int in_width, int in_height)
{
	//Updates display
	area.dx = in_dx;
	area.dy = in_dy;
	area.width = in_width;
	area.height = in_height;
	ioctl(fd, 0x4680, &area);
}
