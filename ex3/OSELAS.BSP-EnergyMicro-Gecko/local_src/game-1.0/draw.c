#define FB 0x4680   //Location?

int fbfd;           //Framebuffer f..?
uint16_t* fbp;      //Framebuffer pointer
int screensize_pixels;
int screensize_bytes;

struct fb_var_screeninfo vinfo;
struct fb_copyarea screen;

#define COLOR 0xF800; //Red

int init_framebuffer()
{
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        printf("Error: can't open framebuffer.\n");
        return EXIT_FAILURE;
    }

    //Acquiring screen info
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        printf("Error: unable to get screen info.\n");
        return EXIT_FAILURE;
    }
    printf("Screeninfo: %d x %d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    screen.dx = 0;
    screen.dy = 0;
    //screen.width = vinfo.xres;
    screen.width = 320;
    //screen.height = vinfo.yres;
    screen.height = 240;

    screensize_pixels = screen.width * screen.height;
    screensize_bytes = screensize_pixels * vinfo.bits_per_pixel / 8;

    fbp = (uint16_t*)mmap(NULL, screensize_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == MAP_FAILED) {
        printf("Error: failed to mem map framebuffer.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < screensize_pixels; i++) {
        fbp[i] = COLOR;
    }

    draw_string_at("CURRENT SCORE:", 14, font_small, TEXT_X_ANCHOR, 0);
    draw_string_at("HIGH SCORE:", 11, font_small, TEXT_X_ANCHOR, 30);

    draw_string_at("Press SW6 twice to", 18, font_small, TEXT_X_ANCHOR, 220);
    draw_string_at("start a new game.", 17, font_small, TEXT_X_ANCHOR, 230);

    return EXIT_SUCCESS;
}

void deinit_framebuffer()
{
    munmap(fbp, screensize_bytes);
    close(fbfd);
}

void refresh_fb()
{
    ioctl(fbfd, FB_DRAW, &screen);
}

void draw_circle()
{
    int i = 0;
}