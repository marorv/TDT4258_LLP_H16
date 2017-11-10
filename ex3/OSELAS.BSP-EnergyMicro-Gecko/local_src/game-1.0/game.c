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
void drawBigCircle(int row, int column, int raduis, uint16_t colour);
void update_display(int in_dx, int in_dy, int in_width, int in_height);
void writeRowCol2array(int row, int col, int16_t colour);

//From bubble.c
struct Ball { //Remember that now we always have to write "struct Ball" when making using this type
	double pos_x;		//Positios should be doubles, and only be converted to ints wen putting them on the board
	double pos_y;
	int direction;		
	uint16_t colour; 	//like 0x7BE0
	int radius;			//Counting the middle pixel. Should be odd to have single center pixel
};

void shooter(void);
void drawPointer(int direction);
int end_x_calc(int start_x, int line_length, int direction);
int end_y_calc(int start_y, int line_length, int direction);
double deg_rad(int angle); //Converts angle degrees to radians
struct Ball moveBall(struct Ball ball);


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


	drawBigCircle(30, 50, 10, Pink);

	drawBigCircle(120, 70, 15, Green);

	drawBigCircle(220, 70, 50, Navy);

	int k;

	/*
	for (k=-90; k<90; k++)
		drawPointer(k);
	*/

	struct Ball testball;
	testball.pos_x=100;
	testball.pos_y=100;
	testball.direction=45;
	testball.radius = 5;
	testball.colour = Pink;


	drawBigCircle((int)testball.pos_x, (int)testball.pos_y, testball.radius, testball.colour);

	for(k=0; k<100; k++)
	{

		//drawBigCircle((int)testball.pos_x, (int)testball.pos_y, testball.radius, Black); //Erase previous ball
		testball=moveBall(testball);
		printf("Drawing testball at %d %d \n", (int)testball.pos_x, (int)testball.pos_y);
		drawBigCircle((int)testball.pos_y, (int)testball.pos_x, testball.radius, testball.colour);

	}

	/*

	int j, i;

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

void writeRowCol2array(int row, int col, int16_t colour)
{
	screen[(row * WIDTH + 1) + col] = colour;

}

//First value is y-value
void drawBigCircle(int start_row, int start_col, int radius, uint16_t colour)
{
	//Draw the circle
	int i;
	for(i = 0; i <= 360; i++)
	{
		int end_x = end_x_calc(start_row, radius, i);
		int end_y = end_y_calc(start_col, radius, i);
		writeRowCol2array(end_x, end_y, colour);
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
	drawBigCircle(init_x, init_y, 5, Red);

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

struct Ball moveBall(struct Ball ball)
{
	//Moves ball speed pixels in direction per call
	struct Ball ret_ball = ball;
	int speed = 1;
	double angle = deg_rad(ret_ball.direction);
	ret_ball.pos_x += speed*cos(angle);
	ret_ball.pos_y -= speed*sin(angle);

	//Handle encountering a wall or reacing top of screen.
	//Reached left wall
	if (ret_ball.pos_x - ret_ball.radius <= 0)
	{
		ret_ball.direction = 180 - ret_ball.direction;
	}
	//Reach right wall
	if (ret_ball.pos_x + ret_ball.radius >= WIDTH)
	{
		ret_ball.direction += 90;
	}

	return ret_ball;
}