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
#include <math.h>

#include "graphics.h"

#define WIDTH 320
#define HEIGHT 240
#define NUMPIXELS  (76799) //320*240-1
#define FILESIZE (153598) //16 bits per pixel

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

void shooter(void);
void drawPointer(int direction);
void black_screen();
void update_display(int in_dx, int in_dy, int in_width, int in_height);
double deg_rad(int angle); //Converts angle degrees to radians
void drawCircle();
void writeRowCol2array();


int main(int argc, char *argv[])
{	
	printf("Hi\n");
}

void drawCircle() {}

void writeRowCol2array() {}

//Put a new ball on the platform to shoot
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

//Draw a line (pointer) of length 10 (plus 12, because beginning behind ball) that points in shooting direction
void drawPointer(int direction)
{
	int line_length = 10;
	//Line originates at HEIGHT-(10+12), center of ball
	int origin_x = HEIGHT - (10+12);
	int origin_y = WIDTH/2;
	//Start of line is outside of ball's radius
	int start_x = HEIGHT - (10+24);
	int start_y = origin_y;
	//End of line is at 10 distance away at angle direction
	int end_x = start_x - (int)line_length * cos(deg_rad(direction));
	int end_y = start_y - (int)line_length * sin(deg_rad(direction));

	//Draw the line
	int i = 0;
	for(i=0; i<line_length; i++)
	{
		writeRowCol2array(start_x - (int)(i * cos(deg_rad(direction))), start_y - (int)(i * sin(deg_rad(direction))));
	}

	//Debug printing
	/*
	printf("Start of line: %d, %d with direction %d degrees \n", start_x, start_y, direction);
	printf("End of line: %d, %d with direction %d degrees \n", end_x, end_y, direction);
	*/

} 