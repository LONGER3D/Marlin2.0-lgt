#include "lcdapi.h"
#include "lcdio.h"
#include "ili9341.h"
#include "st7789v.h"

LgtLcdApi lcd;

/**
 * consturctor
 */
LgtLcdApi::LgtLcdApi() :
    m_lcdID(0)
{

}

uint8_t LgtLcdApi::begin()
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

    clear(RED);
  return 1;
}


void LgtLcdApi::setCursor(uint16_t Xpos, uint16_t Ypos)
{
    if (m_lcdID == ILI9341_ID)
        ILI9341_SetCursor(Xpos, Ypos);
    else if (m_lcdID == ST7789V_ID)
        ST7789V_SetCursor(Xpos, Ypos);
}

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



