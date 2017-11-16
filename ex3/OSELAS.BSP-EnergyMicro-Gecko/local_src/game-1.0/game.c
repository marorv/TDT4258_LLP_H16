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

#define LEFT_BUTTON 0xFE00
#define RIGHT_BUTTON 0xFB00
#define SHOOT_BUTTON 0xBF00

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)


void deinit_devices();
void init_devices();
void hitGoal();
uint16_t GPIO_handler();

int hits;


int main(int argc, char *argv[])
{		

	printf("Hello World, I'm game!\n");


	/*The following program prints part of the file specified in its first
       command-line argument to standard output.  The range of bytes to be
       printed is specified via offset and length values in the second and
       third command-line arguments.  The program creates a memory mapping
       of the required pages of the file and then uses write(2) to output
       the desired bytes.
	*/
	init_devices();

	black_screen();
	hits = 0;

	hitGoal();

	struct Ball ball;
	ball.pos_x=WIDTH/2;
	ball.pos_y=HEIGHT-5;
	ball.direction=0;
	ball.radius = 9;
	ball.moving = 0;
	ball.colour = Pink;

	struct Square square;
	square.pos_x=WIDTH/2;
	square.pos_y=HEIGHT-5;
	square.direction=0;
	square.moving = 0;
	square.colour = Pink;

	struct Square prev_square;
	prev_square = square;
	prev_square.colour = Black;

	int j;
	j = 90;
	drawPointer(j);
	drawPlatform(WIDTH/2 - 15);	

	uint16_t buttons_pressed;
	while(1)
	{
		buttons_pressed = GPIO_handler();

		switch(buttons_pressed){

			case LEFT_BUTTON:
				j -= 5;
				if (j <= 0) j = 1;
				drawPointer(j);
				//printf("Left button pressed\n");
				break;
			case RIGHT_BUTTON:
				j += 5;
				if (j >= 180) j = 179; 
				drawPointer(j);
				//printf("Right button pressed\n");
				break;

			case SHOOT_BUTTON:
				//printf("Shoot button pressed\n");
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
						//printf("Hit at %d, %d, %d\n", (int)square.pos_y * WIDTH + (int)square.pos_x, (int)square.pos_x, (int)square.pos_y);
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
			default:
				break;
				//printf("Buttons pressed: %x\n", buttons_pressed);
		
		}

	}


	deinit_devices();
	exit(EXIT_SUCCESS);

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

uint16_t GPIO_handler()
{
	uint16_t read_buf;

	read(gpio_fd, &read_buf, sizeof(read_buf));

	return (read_buf << 8); 
}

void init_devices()
{
	gpio_fd = open("/dev/GPIO_buttons", O_RDWR);
	if (gpio_fd == -1)
	   handle_error("open");

	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1)
	   handle_error("open");

	if (fstat(fd, &sb) == -1)           /* To obtain file size */
	   handle_error("fstat");


	screen = (uint16_t*)mmap(NULL, FILESIZE, PROT_READ, MAP_PRIVATE, fd, 0);
	if (screen == MAP_FAILED)
	   handle_error("mmap");
}

void deinit_devices()
{
	munmap(screen, FILESIZE);
	close(fd);
	close(gpio_fd);	
}