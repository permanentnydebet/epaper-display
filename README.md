# epaper-display

Supporting set of function to simply put somethong in e-paper dispaly (3 colors black/white/red).

# Draw mode
This set of function support displaing fonts and pictures in following rule:
- font/image is splited horizontally - 1 bit per pixel

# How to use
To create new image simply use:

    EpdImage *EPD_NewImage(uint16_t resolution_x, uint16_t resolution_y, EpdOrientation orientation, EpdColor color);

It will be return a pointer to new image.
Drawing in that image enething from list e.g.:
- point
- line
- rectangle
- char
- string
- picture

Use function:

    void EPD_DrawPoint(...);
    void EPD_DrawLine(...);
    void EPD_DrawRectangle(...);
    void EPD_ShowChar(...);
    void EPD_ShowString(...);
    void EPD_ShowPicture(...);

# Font
To show a char or string user need to pass a font to drawing function. Font should be defined as struct:

    typedef struct {
        const uint8_t *table;
        uint16_t width;
        uint16_t hight;
        uint16_t distance_size;
    } EpdFont;

I'm using [STM32-LCD_Font_Generator](https://github.com/zst-embedded/STM32-LCD_Font_Generator) with little changes to generate font.`distance_size` should be set empirically.

# Image generator
To generate image is easy to use [image2cpp](https://javl.github.io/image2cpp/):
- **Draw mode** -> **Horizontal - 1 bit per pixel**,
- **Swap bits in byte** -> **Off**

