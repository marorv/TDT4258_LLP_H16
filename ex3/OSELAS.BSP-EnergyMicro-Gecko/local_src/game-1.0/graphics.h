#ifndef GRAPHICS_H
#define GRAPHICS_H

#define Black           0x0000
#define White           0xFFFF
#define DarkGrey        0x7BEF
#define LightGrey       0xC618
#define Yellow          0xFFE0
#define Orange          0xFD20
#define Red             0xF800
#define Maroon          0x7800
#define Pink            0xF81F
#define GreenYellow     0xAFE5
#define Green           0x07E0
#define DarkGreen       0x03E0
#define DarkCyan        0x03EF
#define Cyan            0x07FF
#define Blue            0x001F
#define Navy            0x000F
#define Purple          0x780F
#define Olive 			0x7BE0

#define FILEPATH "/dev/fb0"
#define WIDTH 320
#define HEIGHT 240
#define NUMPIXELS  (76799) //320*240-1
#define FILESIZE (153598) //16 bits per pixel

void black_screen();
void drawBigCircle(int row, int column, int raduis, uint16_t colour);
void update_display(int in_dx, int in_dy, int in_width, int in_height);
void writeRowCol2array(int row, int col, int16_t colour);
void drawPlatform(int);

void shooter(void);
void drawPointer(int direction);
int end_x_calc(int start_x, int line_length, int direction);
int end_y_calc(int start_y, int line_length, int direction);
double deg_rad(int angle); //Converts angle degrees to radians
struct Ball moveBall(struct Ball ball);

//From bubble.c
struct Ball { //Remember that now we always have to write "struct Ball" when making using this type
	double pos_x;		//Positios should be doubles, and only be converted to ints wen putting them on the board
	double pos_y;
	int direction;		
	uint16_t colour; 	//like 0x7BE0
	int radius;			//Counting the middle pixel. Should be odd to have single center pixel
};

struct fb_copyarea area;
uint16_t *screen;
int fd;

int gpio_fd;
struct stat sb;
uint16_t read_buf;

#endif

