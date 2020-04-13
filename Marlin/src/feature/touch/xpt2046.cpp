/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// #include "../../inc/MarlinConfigPre.h"
#include "../../inc/MarlinConfig.h"

#if ENABLED(TOUCH_BUTTONS)
#include "xpt2046.h"

#define DEBUG_TOUCH
#define DEBUG_OUT ENABLED(DEBUG_TOUCH)
#include "../../core/debug_out.h"

#ifndef TOUCH_INT_PIN
  #define TOUCH_INT_PIN  -1
#endif
#ifndef TOUCH_MISO_PIN
  #define TOUCH_MISO_PIN MISO_PIN
#endif
#ifndef TOUCH_MOSI_PIN
  #define TOUCH_MOSI_PIN MOSI_PIN
#endif
#ifndef TOUCH_SCK_PIN
  #define TOUCH_SCK_PIN  SCK_PIN
#endif
#ifndef TOUCH_CS_PIN
  #define TOUCH_CS_PIN   CS_PIN
#endif

XPT2046 touch;
extern int8_t encoderDiff;

void XPT2046::init() {
  SET_INPUT(TOUCH_MISO_PIN);
  SET_OUTPUT(TOUCH_MOSI_PIN);
  SET_OUTPUT(TOUCH_SCK_PIN);
  OUT_WRITE(TOUCH_CS_PIN, HIGH);

  #if PIN_EXISTS(TOUCH_INT)
    // Optional Pendrive interrupt pin
    SET_INPUT(TOUCH_INT_PIN);
  #endif

  // Read once to enable pendrive status pin
  getInTouch(XPT2046_X);
}

#if HAS_DIGITAL_BUTTONS

#include "../../lcd/ultralcd.h" // For EN_C bit mask

uint8_t XPT2046::read_buttons() {
  int16_t tsoffsets[4] = { 0 };

  if (tsoffsets[0] + tsoffsets[1] == 0) {
    // Not yet set, so use defines as fallback...
    tsoffsets[0] = XPT2046_X_CALIBRATION;
    tsoffsets[1] = XPT2046_X_OFFSET;
    tsoffsets[2] = XPT2046_Y_CALIBRATION;
    tsoffsets[3] = XPT2046_Y_OFFSET;
  }

  // We rely on XPT2046 compatible mode to ADS7843, hence no Z1 and Z2 measurements possible.

  if (!isTouched()) return 0;
  const uint16_t x = uint16_t(((uint32_t(getInTouch(XPT2046_X))) * tsoffsets[0]) >> 16) + tsoffsets[1],
                 y = uint16_t(((uint32_t(getInTouch(XPT2046_Y))) * tsoffsets[2]) >> 16) + tsoffsets[3];
  if (!isTouched()) return 0; // Fingers must still be on the TS for a valid read.

  if (y < 175 || y > 234) return 0;

  return WITHIN(x,  14,  77) ? EN_D
       : WITHIN(x,  90, 153) ? EN_A
       : WITHIN(x, 166, 229) ? EN_B
       : WITHIN(x, 242, 305) ? EN_C
       : 0;
}
#endif

bool XPT2046::isTouched() {
  return (
    #if PIN_EXISTS(TOUCH_INT)
      READ(TOUCH_INT_PIN) != HIGH
    #else
      getInTouch(XPT2046_Z1) >= XPT2046_Z1_THRESHOLD
    #endif
  );
}

uint16_t XPT2046::getInTouch(const XPTCoordinate coordinate) {
  uint16_t data[3];

  OUT_WRITE(TOUCH_CS_PIN, LOW);

  const uint8_t coord = uint8_t(coordinate) | XPT2046_CONTROL | XPT2046_DFR_MODE;
  for (uint16_t i = 0; i < 3 ; i++) {
    for (uint8_t j = 0x80; j; j >>= 1) {
      WRITE(TOUCH_SCK_PIN, LOW);
      WRITE(TOUCH_MOSI_PIN, bool(coord & j));
      WRITE(TOUCH_SCK_PIN, HIGH);
    }

    data[i] = 0;
    for (uint16_t j = 0x8000; j; j >>= 1) {
      WRITE(TOUCH_SCK_PIN, LOW);
      if (READ(TOUCH_MISO_PIN)) data[i] |= j;
      WRITE(TOUCH_SCK_PIN, HIGH);
    }
    WRITE(TOUCH_SCK_PIN, LOW);
    data[i] >>= 4;
  }

  WRITE(TOUCH_CS_PIN, HIGH);

  uint16_t delta01 = _MAX(data[0], data[1]) - _MIN(data[0], data[1]),
           delta02 = _MAX(data[0], data[2]) - _MIN(data[0], data[2]),
           delta12 = _MAX(data[1], data[2]) - _MIN(data[1], data[2]);

  if (delta01 <= delta02 && delta01 <= delta12)
    return (data[0] + data[1]) >> 1;

  if (delta02 <= delta12)
    return (data[0] + data[2]) >> 1;

  return (data[1] + data[2]) >> 1;
}

bool XPT2046::getTouchPoint(uint16_t &x, uint16_t &y) {
  if (isTouched()) {
    x = getInTouch(XPT2046_X);
    y = getInTouch(XPT2046_Y);
  }
  return isTouched();
}


#if  ENABLED(LGT_LCD_TFT)
#include "../../longer3d/lcddrive/lcdapi.h"

#define MAX_READ_XY_REPEAT 10u	// 最大重复读取次数
#define ERR_RANGE 50u //误差范围 

int16_t XPT2046::xCalibration = XPT2046_X_CALIBRATION;
int16_t XPT2046::yCalibration = XPT2046_Y_CALIBRATION;
int16_t XPT2046::xOffset = XPT2046_X_OFFSET;
int16_t XPT2046::yOffset = XPT2046_Y_OFFSET;

/**
 * read touch x-y value
 * value can't less than 100 or more than 4000
 */
uint8_t XPT2046::readTouchXY(uint16_t &x,uint16_t &y)
{
	uint16_t xtemp,ytemp;
	xtemp = getInTouch(XPT2046_X);
	if (xtemp < 100 || xtemp > 4000)
		return 0; // failed
	ytemp = getInTouch(XPT2046_Y);	  												   
	if(ytemp < 100 || ytemp > 4000)
		return 0; // failed
	x=xtemp;
	y=ytemp;
	return 1;   // successful
}

/** read x-y ad value two time
 */
uint8_t XPT2046::readTouchXY2(uint16_t &x, uint16_t &y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
	for (uint8_t i = 0; i < MAX_READ_XY_REPEAT; ++i) {
		if (readTouchXY(x1,y1))
			if (readTouchXY(x2,y2))
				if ((x1 > x2 - ERR_RANGE) && (x2 > x1 - ERR_RANGE) && 
					(y1 > y2 -ERR_RANGE) && (y2 > y1 - ERR_RANGE)) {
						x = (x1 + x2)/2;
						y = (y1 + y2)/2;									
						return 1;
					}						
	}	
	return 0;
}

uint8_t XPT2046::readTouchPoint(uint16_t &x, uint16_t &y)
{
  if (xCalibration + xOffset == 0) {
    // Not yet set, so use defines as fallback...
    xCalibration = XPT2046_X_CALIBRATION;
    xOffset      = XPT2046_X_OFFSET;
    yCalibration = XPT2046_Y_CALIBRATION;
    yOffset      = XPT2046_Y_OFFSET;
  }

  if (!isTouched())
    return 0;
  uint16_t xAd, yAd;
  if (readTouchXY2(xAd, yAd)) {
    x = uint16_t((uint32_t(xAd) * xCalibration) >> 16) + xOffset;
    y = uint16_t((uint32_t(yAd) * yCalibration) >> 16) + yOffset;
  } else {
    DEBUG_ECHOLN("read touch failed");
    x = y = 0;
    return 0;
  }
  if (!isTouched()) // recheck if touched
    return 0;
  return 1;
}

#define TXT_TFT_CONTROLLER_ID            "ControllerID:  %04X"
#define TXT_TFT_CONTROLLER               "Controller: %s"
#define TXT_TOUCH_CALIBRATION            "Touch calibration"
#define TXT_TOP_LEFT                     "Top Left"
#define TXT_BOTTOM_LEFT                  "Bottom Left"
#define TXT_TOP_RIGHT                    "Top Right"
#define TXT_BOTTOM_RIGHT                 "Bottom Right"

#define TXT_CALI_COMPLETED               "Touch calibration completed"
#define TXT_X_CALIBRATION                "X_CALIBRATION:"
#define TXT_Y_CALIBRATION                "Y_CALIBRATION:"
#define TXT_X_OFFSET              		   "X_OFFSET:"
#define TXT_Y_OFFSET                     "Y_OFFSET:"

#define TXT_PROMPT_INFO1                 "Please touch the screen to"
#define TXT_PROMPT_INFO2                 "return to the home page!"

uint8_t XPT2046::calibrate()
{
  uint16_t color = YELLOW;
  uint16_t bgColor = BLACK;
  uint16_t length;
  // uint32_t i;
  uint16_t x[4] = {0,0,0,0};
  uint16_t y[4] = {0,0,0,0};
  char text[41];
  SERIAL_ECHOLN("start calibration");

  lgtlcd.setColor(color);
  lgtlcd.setBgColor(bgColor);

  for (uint8_t i = 0; i < 4;) {
    lgtlcd.clear(bgColor);

    /**
     * Test coordinates and colors inversion.
     * Draw RED and GREEN squares in top left area of the screen.
     */
    lgtlcd.setWindow(40, 20, 99, 69);

    length = sprintf(text, TXT_TFT_CONTROLLER_ID, lgtlcd.lcdId());
    // center horizontal alignment, x = (320 - len * 8) / 2
    lgtlcd.print(160 - length * 4, 48, text);
    // length = sprintf(text, TXT_TFT_CONTROLLER, controller);
    // lgtlcd.print(160 - length * 4, 66, text);

    length = sprintf(text, TXT_TOUCH_CALIBRATION);
    lgtlcd.print(92, 88, text);

    switch (i) {
      case 0:
        lgtlcd.drawCross( 20 , 20, 0xFFFF);
        length = sprintf(text, TXT_TOP_LEFT);
        break;
      case 1:
        lgtlcd.drawCross( 20, 219, 0xFFFF);
        length = sprintf(text, TXT_BOTTOM_LEFT);
        break;
      case 2:
        lgtlcd.drawCross(299,  20, 0xFFFF);
        length = sprintf(text, TXT_TOP_RIGHT);
        break;
      case 3:
        lgtlcd.drawCross(299, 219, 0xFFFF);
        length = sprintf(text, TXT_BOTTOM_RIGHT);
        break;
    }
    lgtlcd.print(160 - length * 4, 108, text);

    // wait for touch then read touch
    // waitForTouch(x[i], y[i]);
    while (1) {
      if ((isTouched()) && readTouchXY2(x[i], y[i]))
        break;
    }

    DEBUG_ECHOPAIR("\ntouched i:", i);
    DEBUG_ECHOLNPAIR(" x:", x[i]);
    DEBUG_ECHOLNPAIR(" y:", y[i]);
    if ((x[i] < 409 || x[i] > 1637) && (y[i] < 409 || y[i] > 1637)) {

      switch (i) {
        case 0: // Top Left
          i++;
          waitForRelease();
          delay(300);
          continue;
        case 1: // Bottom Left
          if (((x[0] < 409 && x[1] < 409) || (x[0] > 1637 && x[1] > 1637)) && ((y[0] < 409 && y[1] > 1637) || (y[0] > 1637 && y[1] < 409))) {
            i++;
            waitForRelease();
            delay(300);
            continue;
          }
          break;
        case 2: // Top Right
          if (((x[0] < 409 && x[2] > 1637) || (x[0] > 1637 && x[2] < 409)) && ((y[0] < 409 && y[2] < 409) || (y[0] > 1637 && y[2] > 1637))) {
            i++;
            waitForRelease();
            delay(300);
            continue;
          }
          break;
        case 3: // Bottom Right
          if (((x[0] < 409 && x[3] > 1637) || (x[0] > 1637 && x[3] < 409)) && ((y[0] < 409 && y[3] > 1637) || (y[0] > 1637 && y[3] < 409))) {
            i++;
            waitForRelease();
            delay(300);
            continue;
          }
          break;
        }
    }
    delay(1500);
    lgtlcd.clear(RED);
    waitForRelease();
    delay(500);
  }

  // calulate touch coefficient
  // 36569088L == ((int32_t)(299 - 20)) << 17
  xCalibration = (int16_t)(36569088L / ((int32_t)x[3] + (int32_t)x[2] - (int32_t)x[1] - (int32_t)x[0]));
  // 26083328L == ((int32_t)(219 - 20)) << 17
  yCalibration = (int16_t)(26083328L / ((int32_t)y[3] - (int32_t)y[2] + (int32_t)y[1] - (int32_t)y[0]));
  xOffset = (int16_t)(20 - ((((int32_t)(x[0] + x[1])) * (int32_t)xCalibration) >> 17));
  yOffset = (int16_t)(20 - ((((int32_t)(y[0] + y[2])) * (int32_t)yCalibration) >> 17));

  // print result to lcd

  lgtlcd.clear(bgColor);
  length = sprintf(text, TXT_TFT_CONTROLLER_ID, lgtlcd.lcdId());
  lgtlcd.print(160 - length * 4, 48, text);
  // length = sprintf(text,TXT_TFT_CONTROLLER, controller);
  // lgtlcd.print(160 - length * 4, 66, text);
  lgtlcd.setColor(GREEN);
  length = sprintf(text, TXT_CALI_COMPLETED);
  lgtlcd.print(160 - length * 4, 88, text);
  lgtlcd.setColor(YELLOW);

  sprintf(text, TXT_X_CALIBRATION);
  lgtlcd.print(76, 108, text);
  sprintf(text, "%6d", xCalibration);
  lgtlcd.m_color = xCalibration >= 0 ? GREEN : RED;
  lgtlcd.print(196, 108, text);
  lgtlcd.m_color = YELLOW;

  sprintf(text, TXT_Y_CALIBRATION);
  lgtlcd.print(76, 124, text);
  sprintf(text, "%6d", yCalibration);
  lgtlcd.m_color = yCalibration >= 0 ? GREEN : RED;
  lgtlcd.print(196, 124, text);
  lgtlcd.m_color = YELLOW;

  sprintf(text, TXT_X_OFFSET);
  lgtlcd.print(76, 140, text);
  sprintf(text, "%6d", xOffset);
  lgtlcd.m_color = xOffset >= 0 ? GREEN : RED;
  lgtlcd.print(196, 140, text);
  lgtlcd.m_color = YELLOW;

  sprintf(text, TXT_Y_OFFSET);
  lgtlcd.print(76, 156, text);
  sprintf(text, "%6d", yOffset);
  lgtlcd.m_color = yOffset >= 0 ? GREEN : RED;
  lgtlcd.print(196, 156, text);
  lgtlcd.m_color = GREEN;
  length = sprintf(text, TXT_PROMPT_INFO1);
  lgtlcd.print(160 - length * 4, 180, text);
  length = sprintf(text, TXT_PROMPT_INFO2);
  lgtlcd.print(160 - length * 4, 198, text);
  lgtlcd.m_color = WHITE;

  // print result to serial
  MYSERIAL0.print("xCalibration:");
  MYSERIAL0.print(xCalibration);
  MYSERIAL0.println();
  MYSERIAL0.print("yCalibration:");
  MYSERIAL0.print(yCalibration);
  MYSERIAL0.println();
  MYSERIAL0.print("xOffset:");
  MYSERIAL0.print(xOffset);
  MYSERIAL0.println();
  MYSERIAL0.print("yOffset:");
  MYSERIAL0.print(yOffset);

  x[0] = (uint16_t)((((int32_t)x[0] * (int32_t)xCalibration) >> 16) + xOffset);
  x[1] = (uint16_t)((((int32_t)x[1] * (int32_t)xCalibration) >> 16) + xOffset);
  x[2] = (uint16_t)((((int32_t)x[2] * (int32_t)xCalibration) >> 16) + xOffset);
  x[3] = (uint16_t)((((int32_t)x[3] * (int32_t)xCalibration) >> 16) + xOffset);
  y[0] = (uint16_t)((((int32_t)y[0] * (int32_t)yCalibration) >> 16) + yOffset);
  y[1] = (uint16_t)((((int32_t)y[1] * (int32_t)yCalibration) >> 16) + yOffset);
  y[2] = (uint16_t)((((int32_t)y[2] * (int32_t)yCalibration) >> 16) + yOffset);
  y[3] = (uint16_t)((((int32_t)y[3] * (int32_t)yCalibration) >> 16) + yOffset);

  MYSERIAL0.println("\nCalibrated coordinates:");
  MYSERIAL0.print("X: "); MYSERIAL0.print(x[0]); MYSERIAL0.print("   Y: "); MYSERIAL0.println(y[0]);
  MYSERIAL0.print("X: "); MYSERIAL0.print(x[1]); MYSERIAL0.print("   Y: "); MYSERIAL0.println(y[1]);
  MYSERIAL0.print("X: "); MYSERIAL0.print(x[2]); MYSERIAL0.print("   Y: "); MYSERIAL0.println(y[2]);
  MYSERIAL0.print("X: "); MYSERIAL0.print(x[3]); MYSERIAL0.print("   Y: "); MYSERIAL0.println(y[3]);
  MYSERIAL0.flush();

  // wait for touch
  while (!isTouched()){
    ;
  }
  lgtlcd.setColor(BLACK);
  lgtlcd.setBgColor(WHITE);

 return 1;
}
#endif

#endif // TOUCH_BUTTONS
