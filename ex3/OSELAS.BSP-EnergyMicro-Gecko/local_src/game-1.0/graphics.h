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

#define LEFT_BUTTON 0xFE00
#define RIGHT_BUTTON 0xFB00
#define SHOOT_BUTTON 0xBF00
#define END_BUTTON 0x7F00

void black_screen();
void drawSquare(int row, int column, uint16_t colour);
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
struct Square moveSquare(struct Square square);

void hitGoal();
int hits;

//From bubble.c
struct Ball { //Remember that now we always have to write "struct Ball" when making using this type
	double pos_x;		//Positios should be doubles, and only be converted to ints wen putting them on the board
	double pos_y;
	int direction;		
	uint16_t colour; 	//like 0x7BE0
	int radius;			//Counting the middle pixel. Should be odd to have single center pixel
	int moving; 		//boolean value to indicate whether ball moving or not (whether shoot button pressed)
};

struct Square { //Remember that now we always have to write "struct Ball" when making using this type
	double pos_x;		//Positios should be doubles, and only be converted to ints wen putting them on the board
	double pos_y;
	int direction;		
	uint16_t colour; 	//like 0x7BE0
	int radius;			//Counting the middle pixel. Should be odd to have single center pixel
	int moving; 		//boolean value to indicate whether ball moving or not (whether shoot button pressed)
};

struct fb_copyarea area;
uint16_t *screen;
int fd;

int gpio_fd;
FILE* device;
struct stat sb;
uint16_t read_buf;

void deinit_devices();
void init_devices();
uint16_t GPIO_handler();

void sigio_handler(int signo);
void initGame(void);
void play(void);
void exit_main(void);

int j;
uint16_t buttons_pressed;

struct Square square;
struct Square prev_square;

#endif
