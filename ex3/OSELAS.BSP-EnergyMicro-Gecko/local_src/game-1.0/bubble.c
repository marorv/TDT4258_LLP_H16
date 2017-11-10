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

struct Ball { //Remember that now we always have to write "struct Ball" when making using this type
	int pos_x;
	int pos_y;
	int direction;		
	uint16_t colour; 	//like 0x7BE0
};

void shooter(void);
void drawPointer(int direction);
int end_x_calc(int start_x, int line_length, int direction);
int end_y_calc(int start_y, int line_length, int direction);
double deg_rad(int angle); //Converts angle degrees to radians
struct Ball moveBall(struct Ball ball);

//From  game.c
void black_screen();
void update_display(int in_dx, int in_dy, int in_width, int in_height);
void drawCircle();
void writeRowCol2array();


int main(int argc, char *argv[])
{	
	printf("Hi\n");
	struct Ball testball;
	testball.pos_x=100;
	testball.pos_y=100;
	testball.direction=45;
	printf("Testball's position: (%d, %d)\n", testball.pos_x, testball.pos_y);
	testball = moveBall(testball);
	printf("Testball's new position: (%d, %d)\n", testball.pos_x, testball.pos_y);

}

void drawCircle() {}

void writeRowCol2array() {}

void update_display(int in_dx, int in_dy, int in_width, int in_height) {}

//Put a new ball on the platform to shoot
void shooter()
{
	//Ball to fire should be sat at middle of screen, from bottom
	int init_x = WIDTH/2;
	int init_y = HEIGHT-(10+12); //10 off the ground pluss half the width og the ball of width 24.

	//Direction is the output angle at which ball is shot out. 180 is straight left, 90 up, 0 straight right
	//direction should be larger than 0 and smaller than 180
	int direction = 90; //Start straight up.
	if (direction < 0) direction = 0;
	if (direction > 180) direction = 180;

	//Place a ball pointing straight up at init_position

	//Push buttons to go left and right, up to fire
}

double deg_rad(int angle)
{
	//x degrees/360 degrees = n rad/2*PI rad
	double rad = angle*2*M_PI/360;
	return rad;
}

//Helper functions to determine where end of line is
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
	int start_y = origin_y + (12);
	//End of line is at line_length distance away at angle direction
	int end_x = end_x_calc(start_x, line_length, direction);
	int end_y = end_y_calc(start_y, line_length, direction);

	//Draw the line
	int i = 0;
	for(i = 0; i < line_length; i++)
	{
		int end_x = end_x_calc(start_x, line_length, direction);
		int end_y = end_y_calc(start_y, line_length, direction);
		writeRowCol2array(end_x, end_y, Red);
	}

	//Find minimally updateable square
	int min_x, min_y, max_x, max_y;
	min_x = start_x - line_length;	//Pointer horizontally left, start - length
	min_y = start_y - line_length;	//Pointer straight up, start- length
	max_x = start_x + line_length;	//Pointer horizontally right, start + length
	max_y = start_y;				//Pointer horizontally out from center, start (always the case)

	if (end_x < start_x) {
		min_x = end_x;
		max_x = start_x;
	} else { 
		min_x = start_x;
		max_x = end_x;
	}

	if (end_y < start_y) {
		min_y = end_y;
	} else { //Pointer is horizontally flat
		min_y = start_y;
	}

	update_display(min_x, min_y, max_x-min_x, max_y-min_y);
}

struct Ball moveBall(struct Ball ball)
{
	//Moves ball speed pixels per call in direction
	struct Ball ret_ball = ball;
	int speed = 5;
	double angle = deg_rad(ball.direction);
	int new_x = ball.pos_x + speed*cos(angle);
	int new_y = ball.pos_y + speed*sin(angle);
	ret_ball.pos_x = new_x;
	ret_ball.pos_y = new_y;

	return ret_ball;
}