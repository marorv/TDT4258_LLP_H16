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

//row ~ WIDTH ~ x; col ~ HEIGHT ~ y
void writeRowCol2array(int row, int col, int16_t colour)
{
	screen[col * WIDTH + row] = colour;

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


	//update_display((start_col - radius), (start_row - radius), 2*(radius+1), 2*(radius+1));
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
	for(j = origin_y; j < origin_y + platf_height; j++)
	{
		for(i = origin_x; i < origin_x + platf_width; i++)
		{
			writeRowCol2array(i, j, Green);
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
	int origin_x = WIDTH/2;
	int origin_y = HEIGHT - 5;
	//Start of line is outside of ball's radius
	int start_x = origin_x;
	int start_y = origin_y;
	//End of line is at 10 distance away at angle direction
	int end_x = end_x_calc(start_x, line_length, direction);
	int end_y = end_y_calc(start_y, line_length, direction);

	//Draw the line
	int i = 0;
	for(i = 0; i < line_length; i++)
	{
		int end_x = end_x_calc(origin_x, i, direction);
		int end_y = end_y_calc(origin_y, i, direction);
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

	update_display(WIDTH/2-50, HEIGHT-50, 100, 50);
}

struct Ball moveBall(struct Ball ball)
{
	//90 is right, 0 is up and -90 is left
	//Moves ball speed pixels in direction per call
	struct Ball ret_ball = ball;
	int speed = ret_ball.radius;
	double angle = deg_rad(ret_ball.direction-90);
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