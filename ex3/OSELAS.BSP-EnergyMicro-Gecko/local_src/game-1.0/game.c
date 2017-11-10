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
uint16_t GPIO_handler();



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


	drawBigCircle(30, 50, 10, Pink);
	
	drawBigCircle(120, 70, 15, Green);

	drawBigCircle(220, 70, 50, Navy);


	struct Ball testball;
	testball.pos_x=100;
	testball.pos_y=100;
	testball.direction=45;
	testball.radius = 5;
	testball.colour = Pink;

	struct Ball prev_testball;
	
	drawPlatform(WIDTH/2 - 15);
	drawPointer(testball.direction);

	drawBigCircle((int)testball.pos_x, (int)testball.pos_y, testball.radius, testball.colour);



	int k;
	int m, n;
	for(k=0; k<10; k++)
	{

		prev_testball = testball;
		prev_testball.colour = Black;
		testball=moveBall(testball);
		printf("Drawing testball at %d %d \n", (int)testball.pos_x, (int)testball.pos_y);
		drawBigCircle((int)prev_testball.pos_y, (int)prev_testball.pos_x, prev_testball.radius, prev_testball.colour);
		drawBigCircle((int)testball.pos_y, (int)testball.pos_x, testball.radius, testball.colour);

	}
	k = 0;
	uint16_t buttons_pressed;
	while(1)
	{
		buttons_pressed = GPIO_handler();

		switch(buttons_pressed){

			case LEFT_BUTTON:
				k += 10;
				printf("Left button pressed\n");
				break;
			case RIGHT_BUTTON:
				k -= 10;
				printf("Right button pressed\n");
				break;
			default:
				printf("Buttons pressed: %x\n", buttons_pressed);
		
		}
		if (k >= 90) k = 89;
		if (k <= -90) k = -89;
		drawPointer(k);

	}


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