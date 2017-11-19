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

#include <signal.h>
#include <linux/ioctl.h>

#include "graphics.h"

#define LEFT_BUTTON 0xFE00
#define RIGHT_BUTTON 0xFB00
#define SHOOT_BUTTON 0xBF00

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)


void deinit_devices();
void init_devices();
uint16_t GPIO_handler();

void sigio_handler(int signo);
void play(void);

int main(int argc, char *argv[])
{		

	printf("Hello World, I'm game!\n");

	init_devices();

	black_screen();
	hits = 0;
	hitGoal();

	j = 90;
	drawPointer(j);
	drawPlatform(WIDTH/2 - 15);	
	
	/*
	printf("Reading buttons\n");
	buttons_pressed = GPIO_handler();
	printf("Buttons: %x\n", buttons_pressed);
	printf("done reading buttons\n");
	*/
	
	while(1);


	deinit_devices();
	exit(EXIT_SUCCESS);

}

uint16_t GPIO_handler()
{
	uint16_t read_buf;

	read(gpio_fd, &read_buf, sizeof(read_buf));

	return (read_buf << 8); 
}

void init_devices()
{
	device = fopen("/dev/GPIO_buttons", "rb");
	if (!device)
	   handle_error("open");
	gpio_fd = fileno(device);

	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1)
	   handle_error("open");

	if (fstat(fd, &sb) == -1)           /* To obtain file size */
	   handle_error("fstat");


	screen = (uint16_t*)mmap(NULL, FILESIZE, PROT_READ, MAP_PRIVATE, fd, 0);
	if (screen == MAP_FAILED)
	   handle_error("mmap");

	if (signal(SIGIO, &sigio_handler) == SIG_ERR) {
        printf("Could not register signal handler.\n");
        //return EXIT_FAILURE;
    }

	//fileno(device) gets the int that is the filedescriptor of the file stream pointer device
    if (fcntl(fileno(device), F_SETOWN, getpid()) == -1) {
        printf("Could not set PID as owner.\n");
        //return EXIT_FAILURE;
    }
    long oflags = fcntl(fileno(device), F_GETFL);
    if (fcntl(fileno(device), F_SETFL, oflags | FASYNC) == -1) {
        printf("Could not set FASYNC flag.\n");
        //return EXIT_FAILURE;
    }
    
}

void sigio_handler(int signo)
{
	/*
	printf("Signal handler\n");
	printf("Signal : %d\n", signo);
	uint16_t buttons_pressed = GPIO_handler();
	printf("Buttons: %x\n", buttons_pressed);
	printf("sigio done\n");
	*/
	play();

}

void play()
{
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
			default:
				break;		
		}
	}
}

void deinit_devices()
{
	munmap(screen, FILESIZE);
	close(fd);
	fclose(device);	
}