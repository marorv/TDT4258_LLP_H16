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
	struct stat sb;
	off_t offset, pa_offset;
	struct fb_copyarea area;
	int i;

	/*The following program prints part of the file specified in its first
       command-line argument to standard output.  The range of bytes to be
       printed is specified via offset and length values in the second and
       third command-line arguments.  The program creates a memory mapping
       of the required pages of the file and then uses write(2) to output
       the desired bytes.
	*/

	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1)
	   handle_error("open");

	if (fstat(fd, &sb) == -1)           /* To obtain file size */
	   handle_error("fstat");

	offset = 0; 
	pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
	   /* offset for mmap() must be page aligned */

	/*
	if (offset >= sb.st_size) {
	   fprintf(stderr, "offset is past end of file\n");
	   exit(EXIT_FAILURE);
	}
	*/

	/* No length arg ==> display to end of file */
	//length = sb.st_size - offset;

	map = (uint16_t*)mmap(NULL, FILESIZE, PROT_READ, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED)
	   handle_error("mmap");

	/* DO NOT WRITE
	s = write(STDOUT_FILENO, map + offset - pa_offset, length);
	if (s != length) {
	   if (s == -1)
	       handle_error("write");

	   fprintf(stderr, "partial write");
	   exit(EXIT_FAILURE);
	}
	*/
	printf("%s\n", "Making a line");
	for (i = 0; i < WIDTH - 1; i++)
	{
		map[i] = 0xF800;
	}

	//Update display
	printf("%s\n", "Updating display");
	area.dx = 0;
	area.dy = 0;
	area.width = WIDTH;
	area.height = HEIGHT;

	ioctl(fd, 0x4680, &area);


	munmap(map, FILESIZE);
	close(fd);

	exit(EXIT_SUCCESS);
}
