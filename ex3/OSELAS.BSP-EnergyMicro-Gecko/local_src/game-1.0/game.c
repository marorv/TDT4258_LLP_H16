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
#include <math.h>

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
void writeRowCol2array(int row, int col, int16_t colour);
void shooter(void);
void drawPointer(int direction);
double deg_rad(int angle); //Converts angle degrees to radians
int end_x_calc(int start_x, int line_length, int direction);
int end_y_calc(int start_y, int line_length, int direction);

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

	drawBigCircle(30, 50, Pink);

	for (k=-90; k<90; k++)
		drawPointer(k);

	/*
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
	*/


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

void writeRowCol2array(int row, int col, int16_t colour)
{
	screen[(row * WIDTH + 1) + col] = colour;

}

void drawBigCircle(int start_row, int start_col, uint16_t colour)
{
	int row;
	int col;

	for(row = -10; row <= 10; row++)
	{
		for(col = -10; col <=10; col++)
		{
			if( (row <= 3 && row >=-3) && (abs(col) == 10) )
				writeRowCol2array((start_row + row), (start_col+col), colour);
			
			if( (col <= 3 && col >=-3) && (abs(row) == 10) )
				writeRowCol2array((start_row + row), (start_col+col), colour);
			
			if( (abs(row) == 5 || abs(row) == 4) && (abs(col) == 9) )
				writeRowCol2array((start_row + row), (start_col+col), colour);

			//if(((col <= 8 && col >= 6) || (col >= -8 && col <= -6)) && ((row <= 8 && row >= 6) || (row >= -8 && row <= -6)))
			//	writeRowCol2array((start_row + row), (start_col+col), Olive);

			if( (abs(col) == 4 || abs(col) == 5) && (abs(row) == 9) )
				writeRowCol2array((start_row + row), (start_col+col), colour);

			if( (abs(col) <= 6 && abs(col) >= 8) && (abs(row) <= 8 && abs(row) >= 6) )
				writeRowCol2array((start_row + row), (start_col+col), colour);



		}
	}


	update_display(0, 0, WIDTH, HEIGHT);
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

void shooter()
{
	//Ball to fire should be sat at middle of screen, from bottom
	int init_x = WIDTH/2;
	int init_y = HEIGHT-(10+12); //10 off the ground pluss half the width og the ball of width 24.

	//Direction is the output angle at which ball is shot out. 0 is straight left, 90 up, 180 straight right
	//direction should be larger than 0 and smaller than 180
	int direction = 90; //Start straight up.
	if (direction < 0) direction = 0;
	if (direction > 180) direction = 180;

	//Place a ball pointing straight up at init_position
	drawCircle(init_x, init_y, Red);

	//Push buttons to go left and right, up to fire
}

double deg_rad(int angle)
{
	//x degrees/360 degrees = n rad/2*PI rad
	double rad = angle*2*M_PI/360;
	return rad;
}

int end_x_calc(int start_x, int line_length, int direction)
{
	return start_x - (int)line_length * cos(deg_rad(direction));
}
int end_y_calc(int start_y, int line_length, int direction)
{
	return start_y - (int)line_length * sin(deg_rad(direction));
}

//Draw a line (pointer) of length 10 (plus 12, because beginning behind ball) that points in shooting direction
void drawPointer(int direction)
{
	int line_length = 50;
	//Line originates at HEIGHT-(10+12), center of ball
	int origin_x = HEIGHT - (10+12);
	int origin_y = WIDTH/2;
	//Start of line is outside of ball's radius
	int start_x = HEIGHT - (10+24);
	int start_y = origin_y;
	//End of line is at 10 distance away at angle direction
	int end_x = end_x_calc(start_x, line_length, direction);
	int end_y = end_y_calc(start_y, line_length, direction);

	//Draw the line
	int i = 0;
	for(i = 0; i < line_length; i++)
	{
		int end_x = end_x_calc(start_x, i, direction);
		int end_y = end_y_calc(start_y, i, direction);
		writeRowCol2array(end_x, end_y, Red);
	}

	//Find minimally updateable square
	int min_x, min_y, max_x, max_y;
	if (end_x < start_x) 
	{
		min_x = end_x;
		max_x = start_x;
	} 
	else
	{ 
		min_x = start_x;
		max_x = end_x;
	}

	if (end_y < start_y) min_y = end_y;
	else min_y = start_y;
	
	max_y = start_y; //Always at start anyways

	//update_display(min_x, min_y, max_x-min_x, max_y-min_y);

	update_display(0, 0, WIDTH, HEIGHT);
}