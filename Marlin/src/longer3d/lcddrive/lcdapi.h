#pragma once

#if ENABLED(LGT_LCD_TFT)
#include "stdint.h"

#define LCD_WIDTH 320u
#define LCD_HIGHT 240u
#define LCD_PIXELS_COUNT ((LCD_WIDTH - 1) * (LCD_HIGHT - 1))

#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F

class LgtLcdApi {
public:
    LgtLcdApi();
    uint8_t init();
    inline void clear(uint16_t color=WHITE) {
        fill(0, 0, LCD_WIDTH-1, LCD_HIGHT-1, color);
    }
    void fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);
    void backLightOff();
    void backLightOn();
    void setColor(uint16_t c) {
        m_color = c;
    }
    void setBgColor(uint16_t c) {
        m_bgColor = c;
    }
    void print(uint16_t x, uint16_t y, const char *text);

private:
    void prepareWriteRAM();
    void setCursor(uint16_t Xpos, uint16_t Ypos);
    void setWindow(uint16_t Xmin, uint16_t Ymin, uint16_t XMax=LCD_WIDTH-1, uint16_t Ymax=LCD_HIGHT-1);    

private:
    uint16_t m_lcdID;
    uint16_t m_color;
    uint16_t m_bgColor;

};

extern LgtLcdApi lgtlcd;

#endif