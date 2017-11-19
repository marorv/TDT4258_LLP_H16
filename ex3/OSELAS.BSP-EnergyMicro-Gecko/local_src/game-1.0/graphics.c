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

void hitGoal()
{
	if(hits % 2 == 0) 
	{
		//Hit the circle, erase it, draw new one
		drawBigCircle(220, 70, 30, Black);
		drawBigCircle(50, 70, 30, Orange);
	} 
	else
	{
		drawBigCircle(50, 70, 30, Black);
		drawBigCircle(220, 70, 30, Orange);
	}
	hits++;
}

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
	int origin_y;
	int platf_width;
	int platf_height;

	origin_y = HEIGHT - 5;
	platf_width = 30;
	platf_height = 5;

	int i, j;
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

//Draw a line (pointer) of length line_length that points in shooting direction
void drawPointer(int direction)
{
	//Erase old pointer before drawing new one
	int i, j; 
	for (i = WIDTH/2-50; i < (WIDTH/2-50)+100; i++)
	{
		for (j = HEIGHT-55; j < HEIGHT-5; j++) 
		{
			writeRowCol2array(i, j, Black);
		}
	}

	int line_length = 50;
	int origin_x = WIDTH/2;
	int origin_y = HEIGHT - 5;

	//Draw the line
	i = 0;
	for(i = 0; i < line_length; i++)
	{
		int end_x = end_x_calc(origin_x, i, direction);
		int end_y = end_y_calc(origin_y, i, direction);
		writeRowCol2array(end_x, end_y, Red);
	}
	update_display(WIDTH/2-50, HEIGHT-70, 100, 70);
}

struct Ball moveBall(struct Ball ball) //NOT IN USE
{
	//Moves ball speed pixels in direction per call
	struct Ball ret_ball = ball;
	int speed = 1;

	double angle = deg_rad(ret_ball.direction);
	ret_ball.pos_x -= speed*cos(angle);
	ret_ball.pos_y -= speed*sin(angle);

	//Handle encountering a wall
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
	//Draw the square
	int i, k;
	for(i = 0; i < 5; i++)
	{
		for(k = 0; k < 5; k++)
		{
			writeRowCol2array(start_row + i, start_col + k, colour);
		}	
	}
	update_display(start_row, start_col, 6, 6);
}

struct Square moveSquare(struct Square square)
{
	//Moves square speed pixels in direction per call
	struct Square ret_square = square;
	int speed = 1;

	if(ret_square.direction <= 0)
	{
		ret_square.direction = 1;
	}

	if (ret_square.direction >= 180)
	{
		ret_square.direction = 179;
	}

	double angle = deg_rad(ret_square.direction);
	ret_square.pos_x -= speed*cos(angle);
	ret_square.pos_y -= speed*sin(angle);

	//Handle encountering a wall
	if (ret_square.pos_x <= 0 || ret_square.pos_x >= WIDTH)
	{
		ret_square.direction = 180 - ret_square.direction;
	}
	return ret_square;
}

void play()
{
	//This probably doesn't have to happen every time play is called, make a init ball funciton?
	struct Square square;
	square.pos_x=WIDTH/2;
	square.pos_y=HEIGHT-5;
	square.direction=0;
	square.moving = 0;
	square.colour = Pink;

	struct Square prev_square;
	prev_square = square;
	prev_square.colour = Black;

	while(1)
	{
		buttons_pressed = GPIO_handler();

		switch(buttons_pressed){

			case LEFT_BUTTON:
				j -= 5;
				if (j <= 0) j = 1;
				drawPointer(j);
				break;

			case RIGHT_BUTTON:
				j += 5;
				if (j >= 180) j = 179; 
				drawPointer(j);
				break;

			case SHOOT_BUTTON:
				square.direction = j;
				square.moving = 1;
				while(square.moving == 1)
				{
					prev_square.pos_x = square.pos_x;
					prev_square.pos_y = square.pos_y;
					square = moveSquare(square);

					//Check if goal hit
					if(screen[(int)square.pos_y * WIDTH + (int)square.pos_x] == Orange)
					{
						hitGoal();
						drawSquare((int)prev_square.pos_x, (int)prev_square.pos_y, prev_square.colour);
						//Put back to start position
						square.moving = 0;
						square.pos_x=WIDTH/2;
						square.pos_y=HEIGHT-5;
						drawSquare((int)square.pos_x, (int)square.pos_y, square.colour);
					}

					drawSquare((int)prev_square.pos_x, (int)prev_square.pos_y, prev_square.colour);
					//printf("Drawing ball at %d %d \n", (int)prev_ball.pos_y, (int)prev_ball.pos_x);
					drawSquare((int)square.pos_x, (int)square.pos_y, square.colour);
					
					//Check if top or bottom of screen reached
					if (square.pos_y <= 0 || square.pos_y >= HEIGHT) {
						drawSquare((int)prev_square.pos_x, (int)prev_square.pos_y, prev_square.colour);
						//Put back to start position
						square.moving = 0;
						square.pos_x=WIDTH/2;
						square.pos_y=HEIGHT-5;
						drawSquare((int)square.pos_x, (int)square.pos_y, square.colour);

					}
				}
				drawPointer(j);	
				drawPlatform(WIDTH/2 - 15);
				break;
			
			case END_BUTTON:
				printf("Ending game. Bye!\n");
				exit_main();
				break;
			
			default:
				break;		
		}
	}
}