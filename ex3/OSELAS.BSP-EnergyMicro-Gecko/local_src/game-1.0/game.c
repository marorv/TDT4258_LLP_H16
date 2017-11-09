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
#include <inttypes.h> // for printing hex numbers

#define FILEPATH "/dev/fb0"
#define WIDTH 320
#define HEIGHT 240
#define NUMPIXELS  (76799) //320*240-1
#define FILESIZE (153598) //16 bits per pixel

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{		

	printf("Hello World, I'm game!\n");

	uint16_t *map;
	int fd;
	int gpio_fd;
	struct stat sb;

	struct fb_copyarea area;
	uint16_t read_buf;


	/*The following program prints part of the file specified in its first
       command-line argument to standard output.  The range of bytes to be
       printed is specified via offset and length values in the second and
       third command-line arguments.  The program creates a memory mapping
       of the required pages of the file and then uses write(2) to output
       the desired bytes.
	*/
	gpio_fd = open("/dev/GPIO_buttons", O_RDWR);
	if (gpio_fd == -1)
	   handle_error("open");
	printf("%s\n", "Opened GPIO_buttons");

	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1)
	   handle_error("open");

	if (fstat(fd, &sb) == -1)           /* To obtain file size */
	   handle_error("fstat");


	map = (uint16_t*)mmap(NULL, FILESIZE, PROT_READ, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED)
	   handle_error("mmap");

	int k = 1;
	int j = 0;
	int i;
	while(j++ < 1000)
	{
		read(gpio_fd, &read_buf, sizeof(read_buf));

		read_buf = read_buf << 8;

		printf ("The data in the device is %x\n", read_buf);
		if (read_buf == 0xfe00)
		{
			printf("%s\n", "Making a line");
			for (i = 0; i < WIDTH*k - 1; i++)
			{
				map[i] = 0xF800;
			}
			k++;

			//Update display
			printf("%s\n", "Updating display");
			area.dx = 0;
			area.dy = 0;
			area.width = WIDTH;
			area.height = HEIGHT;
			ioctl(fd, 0x4680, &area);
		}	
	}




	munmap(map, FILESIZE);
	close(fd);
	close(gpio_fd);

	exit(EXIT_SUCCESS);

}
