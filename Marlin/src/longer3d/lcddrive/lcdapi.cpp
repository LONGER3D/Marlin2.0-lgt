#include "lcdio.h"

#if ENABLED(LGT_LCD_TFT)
#include "lcdapi.h"
#include "ili9341.h"
#include "st7789v.h"
#include "lcdfont16.h"
#include "../w25qxx.h"

LgtLcdApi lgtlcd;

/**
 * consturctor
 */
LgtLcdApi::LgtLcdApi() :
    m_lcdID(0),
    m_color(BLACK),
    m_bgColor(WHITE)
{

}

uint8_t LgtLcdApi::init()
{
  // set pinmode output and write
  OUT_WRITE(LCD_BACKLIGHT_PIN, LOW);
  OUT_WRITE(LCD_RESET_PIN, LOW); // perform a clean hardware reset
  _delay_ms(5);
  OUT_WRITE(LCD_RESET_PIN, HIGH);
  _delay_ms(5);
  OUT_WRITE(LCD_BACKLIGHT_PIN, HIGH);
  // FSMC init
  LCD_IO_Init(FSMC_CS_PIN, FSMC_RS_PIN);
  
  m_lcdID= LCD_IO_ReadData(0x0000);  //read  id
  uint32_t getdata;
	if (m_lcdID == 0) 
	{
		// read ID1 register to get LCD controller ID, MOST of the time located in register 0x04
		getdata = LCD_IO_ReadData(0x04, 3);
		m_lcdID = (uint16_t)(getdata & 0xFFFF);
  	} 
  //If ID1 is 0, it means we need to check alternate registers, like 0xD3 in the case of ILI9341
  if (m_lcdID == 0) 
  {
    m_lcdID = LCD_IO_ReadData(0x00);
    if (m_lcdID == 0)
	{
      // reading ID4 register (0xD3)  to get ILI9341 identifier instead of register ID (0x04)
      getdata = LCD_IO_ReadData(0xD3, 3);
      m_lcdID = (uint16_t)(getdata & 0xFFFF);
    }
  }

  SERIAL_ECHOLNPAIR("LCD ID:", m_lcdID); // 0x9341 == 37697
    switch (m_lcdID) {
    case 0x1505: break; // R61505U
    case ST7789V_ID: ST7789V_Init(); break; // ST7789V
    case 0x8989: break; // SSD1289
    case 0x9325: break; // ILI9325
    case 0x9328: break; // ILI9328
    case ILI9341_ID: ILI9341_Init(); break; // ILI9341
    case 0x0404: break; // No LCD Controller detected
    default: break; // Unknown LCD Controller
    }

    // clear(WHITE);

  return 1;
}


void LgtLcdApi::setCursor(uint16_t Xpos, uint16_t Ypos)
{
    if (m_lcdID == ILI9341_ID)
        ILI9341_SetCursor(Xpos, Ypos);
    else if (m_lcdID == ST7789V_ID)
        ST7789V_SetCursor(Xpos, Ypos);
}

/**
 * set show rectangle
 * and prepare write gram
 */
void LgtLcdApi::setWindow(uint16_t Xmin, uint16_t Ymin, uint16_t XMax /* = 319 */, uint16_t Ymax /* = 239 */)
{
    if (m_lcdID == ILI9341_ID)
        ILI9341_SetWindow(Xmin, Ymin, XMax, Ymax);
    else if (m_lcdID == ST7789V_ID)
        ST7789V_SetWindow(Xmin, Ymin, XMax, Ymax);
}

void LgtLcdApi::prepareWriteRAM()
{
    if (m_lcdID == ILI9341_ID)
        ILI9341_WriteRam();
    else if (m_lcdID == ST7789V_ID)
        ST7789V_WriteRam();
}

void LgtLcdApi::backLightOn()
{
  OUT_WRITE(LCD_BACKLIGHT_PIN, HIGH);
}

void LgtLcdApi::backLightOff()
{
  OUT_WRITE(LCD_BACKLIGHT_PIN, LOW);
}

/** 
* Fill in a specified area with a single color
* (sx,sy),(ex,ey): Filled rectangular diagonal coordinates    Area size：(ex-sx+1)*(ey-sy+1) 
* color: Filled color
*/
void LgtLcdApi::fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{
    #if ENABLED(LCD_USE_DMA_FSMC)
        setWindow(sx, sy, ex, ey);
        uint32_t count = (ex - ey + 1) * (ey - ex + 1);
        NOMORE(count, LCD_PIXELS_COUNT);
        LCD_IO_WriteMultiple(color, count);
    #else
        uint16_t i,j;
        uint16_t xlen=0;
        // uint16_t temp;
        xlen=ex-sx+1;	 
        for(i=sy;i<=ey;i++)
        {
            setCursor(sx,i);      	 //Setting cursor position		
            prepareWriteRAM();      //start writing gram	
            for(j=0;j<xlen;j++)
                LCD_IO_WriteData(color);  //show color
        }
    #endif
}

/**
 * print string in lcd
 */
void LgtLcdApi::print(uint16_t x, uint16_t y, const char *text)
{
  for (uint16_t l = 0; (*(uint8_t*)(text + l) != 0) && ((x + l * 8 + 8) < 320); l ++) {
    uint16_t i, j, k;
    uint8_t character;
    character = (*(uint8_t*)(text + l) < 32 || *(uint8_t*)(text + l) > 127) ? 0 : *(text + l) - 32;
    setWindow(x + l * 8, y, x + l * 8 + 7, y + 15);

    for (i = 0; i < 2; i++)
      for (j = 0; j < 8; j++)
        for (k = 0; k < 8; k++)
          LCD_IO_WriteData(font16[character][i + 2 * k] & (128 >> j) ? m_color  : m_bgColor);
  }

}

struct imageHeader {
   uint8_t scan;
   uint8_t gray;
   uint16_t w;
   uint16_t h;
   uint8_t is565;
   uint8_t rgb;
}; 
#define IMAGE_BUFF_SIZE  4000

void LgtLcdApi::showImage(uint16_t x_st, uint16_t y_st, uint32_t addr)
{
    imageHeader head;
    spiFlash.W25QXX_Read(reinterpret_cast<uint8_t *>(&head), addr, sizeof(head));
    // SERIAL_ECHOLNPAIR("image-w: ", head.w);
    // SERIAL_ECHOLNPAIR("image-h: ", head.h);
	showRawImage(x_st, y_st, head.w, head.h, addr+8);    
}


//#define SLOW_SHOW_IMAGE

void LgtLcdApi::showRawImage(uint16_t xsta,uint16_t ysta,uint16_t width,uint16_t high, uint32_t addr)
{ 
    static uint8_t image_buffer[IMAGE_BUFF_SIZE];

    #if defined(SLOW_SHOW_IMAGE)
        uint16_t x = xsta, y= ysta;
    #endif

    // calculate read times
	uint32_t image_size = width * high * 2;
	uint16_t get_image_times=image_size/IMAGE_BUFF_SIZE;
	if((image_size-get_image_times*IMAGE_BUFF_SIZE)>0) {
		get_image_times = get_image_times + 1;
	}

    setWindow(xsta, ysta, xsta + width - 1, ysta + high - 1);
	for(uint16_t k = 0; k < get_image_times; k++) {
		uint16_t real_size = (get_image_times-k)>1?IMAGE_BUFF_SIZE:(image_size-k*IMAGE_BUFF_SIZE);
		spiFlash.W25QXX_Read(image_buffer, addr+k*IMAGE_BUFF_SIZE,real_size);

        #if ENABLED(LCD_USE_DMA_FSMC) 
             #define SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
             for (uint16_t i = 0; i < real_size; i = i + 2) {
                SWAP(image_buffer[i], image_buffer[i + 1]);    // little-endian
             }
            LCD_IO_WriteSequence_Async(reinterpret_cast<uint16_t *>(image_buffer), real_size / 2);
        #elif DISABLED(SLOW_SHOW_IMAGE)
            for (uint16_t i = 0; i < real_size; i = i + 2) {
                uint16_t color = image_buffer[i] | (image_buffer[i + 1] << 8); // little-endian
                LCD_IO_WriteData(color);
            }
        #else
            setCursor(x,y);
            prepareWriteRAM();		 
            for(uint16_t i=0; i< real_size; i = i + 2)
            {
                uint16_t color=image_buffer[i]|image_buffer[i+1]<<8;
                LCD_IO_WriteData(color);
                x++;			 
                if(x>=(xsta+width))
                { 
                x=xsta;
                y++; 
                setCursor(x,y);	
                prepareWriteRAM();	
                }				
            }
        #endif
	}  
}



#endif // LGT_LCD_TFT

