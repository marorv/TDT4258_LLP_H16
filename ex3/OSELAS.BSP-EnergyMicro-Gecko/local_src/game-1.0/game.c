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

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{		

	printf("Hello World, I'm game!\n");

	init_devices();

	//This could be put somewhere else, like in a init game function?
	black_screen();
	hits = 0;
	hitGoal();

	j = 90;
	drawPointer(j);
	drawPlatform(WIDTH/2 - 15);	

	//Actual game playing
	while (1)
	{
		__asm("wfi");
	}
	//exit and uninitialise in exit_main, called by SW8, so DW about warnings about this

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
    	handle_error("Could not register signal handler.\n");
    }

	//fileno(device) gets the int that is the filedescriptor of the file stream pointer device
    if (fcntl(fileno(device), F_SETOWN, getpid()) == -1) {
    	handle_error("Could not set PID as owner.\n");
    }
    long oflags = fcntl(fileno(device), F_GETFL);
    if (fcntl(fileno(device), F_SETFL, oflags | FASYNC) == -1) {
    	handle_error("Could not set FASYNC flag.\n");
    }
    
}

void sigio_handler(int signo)
{
	play();
}

void deinit_devices()
{
	munmap(screen, FILESIZE);
	close(fd);
	fclose(device);	
}

void exit_main()
{
	deinit_devices();
	exit(EXIT_SUCCESS);
}
