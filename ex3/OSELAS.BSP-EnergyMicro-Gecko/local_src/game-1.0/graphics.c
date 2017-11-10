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


	update_display((start_col - radius), (start_row - radius), 2*(radius+1), 2*(radius+1));
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

void drawPlatform(int origin_x)
{
	//int origin_x;
	int origin_y;
	int platf_width;
	int platf_height;
	//origin_x = WIDTH/2 - 15;
	origin_y = HEIGHT - 5;
	platf_width = 30;
	platf_height = 5;

	int i;
	int j;
	for(i = origin_y; i < origin_y + platf_height; i++)
	{
		for(j = origin_x; j < origin_x + platf_width; j++)
		{
			writeRowCol2array(i, j, Red);
		}
	}

	update_display(origin_x, origin_y, platf_width, platf_height);

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
	int speed = 2*ret_ball.radius;
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