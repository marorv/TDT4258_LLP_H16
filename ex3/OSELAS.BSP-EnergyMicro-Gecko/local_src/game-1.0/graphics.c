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


	update_display((start_row - radius-1), (start_col - radius-1), 2*(radius+1), 2*(radius+1));
	//update_display(0, 0, WIDTH, HEIGHT);
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
	//Erase old pointer before drawing new one
	int i, j; 
	for (i = WIDTH/2-50; i < (WIDTH/2-50)+100; i++)
	{
		for (j = HEIGHT-50; j < HEIGHT-5; j++) 
		{
			writeRowCol2array(i, j, Black);
		}
	}

	int line_length = 50;
	//Line originates at HEIGHT-(10+12), center of ball
	int origin_x = WIDTH/2;
	int origin_y = HEIGHT - 5;
	//Start of line is outside of ball's radius
	int start_x = origin_x;
	int start_y = origin_y;

	//Draw the line
	i = 0;
	for(i = 0; i < line_length; i++)
	{
		int end_x = end_x_calc(origin_x, i, direction);
		int end_y = end_y_calc(origin_y, i, direction);
		writeRowCol2array(end_x, end_y, Red);
	}

	update_display(WIDTH/2-50, HEIGHT-50, 100, 50);
}

struct Ball moveBall(struct Ball ball)
{
	//Moves ball speed pixels in direction per call
	struct Ball ret_ball = ball;
	int speed = 15;
	double angle = deg_rad(ret_ball.direction);
	ret_ball.pos_x -= speed*cos(angle);
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

void drawSquare(int start_row, int start_col, uint16_t colour)
{
	//Draw the circle
	int i, k;
	for(i = 0; i <= 5; i++)
	{
		for(k=0; k <= 5; k++){

			writeRowCol2array(start_row+i, start_col+k, colour);
		}	
		
	}


	update_display(start_row, start_col, start_row+6, start_col+6);

}

struct Square moveSquare(struct Square square)
{
	//Moves ball speed pixels in direction per call
	struct Square ret_square = square;
	int speed = 15;
	double angle = deg_rad(ret_square.direction);
	ret_square.pos_x -= speed*cos(angle);
	ret_square.pos_y -= speed*sin(angle);

	//Handle encountering a wall or reacing top of screen.
	//Reached left wall
	if (ret_square.pos_x <= 0)
	{
		ret_square.direction = 180 - ret_square.direction;
	}
	//Reach right wall
	if (ret_square.pos_x >= WIDTH)
	{
		ret_square.direction += 90;
	}

	return ret_square;
}