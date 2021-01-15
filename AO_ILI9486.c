/*
Library:						TFT 3.2" LCD - ILI9486
Written by:					Alex Onop
Date written:				28/12/2020

Description:				This library makes use of the FSMC interface of the STM32 board to control a TFT LCD.
										The concept shown here is exactly the same for other TFT LCDs, might need to use 16 bits for
										some LCDs, but the method is similar.
										You can use this as a starting point to program your own LCD and share it with us ;) 
*/

//Header files
#include "AO_ILI9486.h"
extern RNG_HandleTypeDef hrng;
static uint8_t rotationNum=1;
static bool _cp437    = false;
//-------------------------------------------------------------------
typedef struct
{
	uint16_t TextColor;
	uint16_t BackColor;
	sFONT *pFont;
}LCD_DrawPropTypeDef;
LCD_DrawPropTypeDef lcdprop;
//-------------------------------------------------------------------
//***** Functions prototypes *****//
void ILI9486_Delay(uint32_t dly)
{
	uint32_t i;
	for(i=0;i<dly;i++);
}
//-------------------------------------------------------------------
__STATIC_INLINE void DelayMicro(uint32_t __IO micros)
{
	micros *=(SystemCoreClock/1000000)/5;
	while(micros--);
}
//-------------------------------------------------------------------
void ILI9486_SendCommand(uint8_t com)
{
	*(__IO uint8_t *)(0x60000000) = com;
}
//-------------------------------------------------------------------
void ILI9486_SendData(uint8_t data)
{
	*(__IO uint8_t *)(0x60040000) = data;
}
//-------------------------------------------------------------------
void ILI9486_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {


  ILI9486_SendCommand (ILI9486_COLUMN_ADDR);
  ILI9486_SendData(x1>>8);
  ILI9486_SendData(x1 & 0xFF);
  ILI9486_SendData(x2>>8);
  ILI9486_SendData(x2 & 0xFF);

  ILI9486_SendCommand (ILI9486_PAGE_ADDR);
  ILI9486_SendData(y1>>8);
  ILI9486_SendData(y1 & 0xFF);
  ILI9486_SendData(y2>>8);
  ILI9486_SendData(y2 & 0xFF);
  ILI9486_SendCommand (ILI9486_GRAM);
}
//-------------------------------------------------------------------
void ILI9486_fontsIni(void)
{
  Font8.Height = 8;
  Font8.Width = 5;
  Font12.Height = 12;
  Font12.Width = 7;
  Font16.Height = 16;
  Font16.Width = 11;
  Font20.Height = 20;
  Font20.Width = 14;
  Font24.Height = 24;
  Font24.Width = 17;
  lcdprop.BackColor=COLOR_BLACK;
  lcdprop.TextColor=COLOR_GREEN;
  lcdprop.pFont=&Font16;
}
//-------------------------------------------------------------------
void ILI9486_Init(void)
 {

		RESET_ACTIVE;
	  	HAL_Delay(10);
	  	RESET_IDLE;
	  	HAL_Delay(100);

	  	//  Interface Mode Control
	    ILI9486_SendCommand(ILI9486_RGB_INTERFACE);
	    ILI9486_SendData(0x00);

	    // Interface Pixel Format, 16 bits / pixel
	    ILI9486_SendCommand(ILI9486_PIXEL_FORMAT);
	    ILI9486_SendData(0x55); // 5D

	    // PGAMCTRL(Positive Gamma Control)
	    ILI9486_SendCommand(ILI9486_PGAMMA);
	    ILI9486_SendData(0x0F);
	    ILI9486_SendData(0x1F);
	    ILI9486_SendData(0x1C);
	    ILI9486_SendData(0x0C);
	    ILI9486_SendData(0x0F);
	    ILI9486_SendData(0x08);
	    ILI9486_SendData(0x48);
	    ILI9486_SendData(0x98);
	    ILI9486_SendData(0x37);
	    ILI9486_SendData(0x0A);
	    ILI9486_SendData(0x13);
	    ILI9486_SendData(0x04);
	    ILI9486_SendData(0x11);
	    ILI9486_SendData(0x0D);
	    ILI9486_SendData(0x00);
	    // NGAMCTRL (Negative Gamma Correction)
	    ILI9486_SendCommand(ILI9486_NGAMMA);
	    ILI9486_SendData(0x0F);
	    ILI9486_SendData(0x32);
	    ILI9486_SendData(0x2E);
	    ILI9486_SendData(0x0B);
	    ILI9486_SendData(0x0D);
	    ILI9486_SendData(0x05);
	    ILI9486_SendData(0x47);
	    ILI9486_SendData(0x75);
	    ILI9486_SendData(0x37);
	    ILI9486_SendData(0x06);
	    ILI9486_SendData(0x10);
	    ILI9486_SendData(0x03);
	    ILI9486_SendData(0x24);
	    ILI9486_SendData(0x20);
	    ILI9486_SendData(0x00);
	    // Digital Gamma Control 1
	    ILI9486_SendCommand(0xE2);
	    ILI9486_SendData(0x0F);
	    ILI9486_SendData(0x32);
	    ILI9486_SendData(0x2E);
	    ILI9486_SendData(0x0B);
	    ILI9486_SendData(0x0D);
	    ILI9486_SendData(0x05);
	    ILI9486_SendData(0x47);
	    ILI9486_SendData(0x75);
	    ILI9486_SendData(0x37);
	    ILI9486_SendData(0x06);
	    ILI9486_SendData(0x10);
	    ILI9486_SendData(0x03);
	    ILI9486_SendData(0x24);
	    ILI9486_SendData(0x20);
	    ILI9486_SendData(0x00);
	    ILI9486_setRotation(0);
	    // Idle mode control + Power +  Frame rate ctrl
	    ILI9486_SendCommand(0x38);
	     // frame rate ctrl
	    ILI9486_SendCommand(0xB1);
	    // Frame rate(Hz) (default: 70kHz) /-/ Division Ratio (default: fosc)
	    ILI9486_SendData(0xB0);
	    // Clock per Line (default: 17 clk cycles)
	    ILI9486_SendData(0x11);
	    // Power Control 3 (For Normal Mode)
	    ILI9486_SendCommand(0xC2);
	    ILI9486_SendData(0x55); // 44
	    // Display Inversion Control
	    ILI9486_SendCommand(0xB4);
	    ILI9486_SendData(0x02); // 2 dot invercion /-/ disabled | 0x12 to enable
	    // Display Function Control
	    ILI9486_SendCommand(ILI9486_DFC); //0xB6

	    ILI9486_SendData(0x02); //1 para
	    ILI9486_SendData(0x02); //2 para old x22
	    ILI9486_SendData(0x3B); //3 para
/*
	    //ILI9486_SendData(0x0a); //1 para
	    //ILI9486_SendData(0x82); //2 para
	    //ILI9486_SendData(0x27); //3 para
 */
	    //ILI9486_SendData(0x00); //4 para&


	    ILI9486_SendCommand (ILI9486_COLUMN_ADDR);
	      ILI9486_SendData(0x00);
	      ILI9486_SendData(0x00);
	      ILI9486_SendData(0x00);
	      ILI9486_SendData(0xEF);

	      ILI9486_SendCommand (ILI9486_PAGE_ADDR);
	      ILI9486_SendData(0x00);
	      ILI9486_SendData(0x00);
	      ILI9486_SendData(0x01);
	      ILI9486_SendData(0x3F);
	      ILI9486_SendCommand (ILI9486_GRAM);
	    HAL_Delay(150);
	    // # Sleep OUT
	    ILI9486_SendCommand(0x11);
	    // Display ON
	    ILI9486_SendCommand(0x29);

	    ILI9486_fontsIni();
	    //HAL_Delay(200);
	    ILI9486_setRotation(1);
 }
//-------------------------------------------------------------------
void ILI9486_drawPixel(uint16_t x, uint16_t y, uint16_t color) {
  ILI9486_SetCursorPosition(x, y, x, y);
	ILI9486_SendData(color>>8);
	ILI9486_SendData(color&0xFF);
}
//-------------------------------------------------------------------
void ILI9486_fill(uint16_t color) {
	uint32_t n = ILI9486_PIXEL_COUNT;
	
	if(rotationNum==1 || rotationNum==3)
	{
		ILI9486_SetCursorPosition(0, 0,   ILI9486_WIDTH -1, ILI9486_HEIGHT -1);
	}
	else if(rotationNum==2 || rotationNum==4)
	{
		ILI9486_SetCursorPosition(0, 0, ILI9486_HEIGHT -1, ILI9486_WIDTH -1);
	}
	
	while (n) {
			n--;
       ILI9486_SendData(color>>8);
				ILI9486_SendData(color&0xff);
	}
}
//-------------------------------------------------------------------
void ILI9486_fillRect(unsigned int x0,unsigned int y0, unsigned int x1,unsigned int y1, uint16_t color) {
	uint32_t n = ((x1+1)-x0)*((y1+1)-y0);
	if (n>ILI9486_PIXEL_COUNT) n=ILI9486_PIXEL_COUNT;
	ILI9486_SetCursorPosition(x0, y0, x1, y1);
	while (n) {
			n--;
      ILI9486_SendData(color>>8);
				ILI9486_SendData(color&0xff);
	}
}
//-------------------------------------------------------------------
void ILI9486_drawRect(unsigned int x0,unsigned int y0, unsigned int x1,unsigned int y1, uint16_t color) {
	ILI9486_drawLine(x0,y0,x1,y0,color);
	ILI9486_drawLine(x1,y0,x1,y1,color);
	ILI9486_drawLine(x0,y0,x0,y1,color);
	ILI9486_drawLine(x0,y1,x1,y1,color);
}
//-------------------------------------------------------------------
void ILI9486_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  ILI9486_drawPixel(x0  , y0+r, color);
  ILI9486_drawPixel(x0  , y0-r, color);
  ILI9486_drawPixel(x0+r, y0  , color);
  ILI9486_drawPixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    ILI9486_drawPixel(x0 + x, y0 + y, color);
    ILI9486_drawPixel(x0 - x, y0 + y, color);
    ILI9486_drawPixel(x0 + x, y0 - y, color);
    ILI9486_drawPixel(x0 - x, y0 - y, color);
    ILI9486_drawPixel(x0 + y, y0 + x, color);
    ILI9486_drawPixel(x0 - y, y0 + x, color);
    ILI9486_drawPixel(x0 + y, y0 - x, color);
    ILI9486_drawPixel(x0 - y, y0 - x, color);
  }
}
//-------------------------------------------------------------------
static void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
	int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      ILI9486_drawPixel(x0 + x, y0 + y, color);
      ILI9486_drawPixel(x0 + y, y0 + x, color);
    } 
    if (cornername & 0x2) {
      ILI9486_drawPixel(x0 + x, y0 - y, color);
      ILI9486_drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      ILI9486_drawPixel(x0 - y, y0 + x, color);
      ILI9486_drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      ILI9486_drawPixel(x0 - y, y0 - x, color);
      ILI9486_drawPixel(x0 - x, y0 - y, color);
    }
  }
}
//-------------------------------------------------------------------
static void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
	int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      ILI9486_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      ILI9486_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      ILI9486_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      ILI9486_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}
//-------------------------------------------------------------------
void ILI9486_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	ILI9486_drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}
//-------------------------------------------------------------------
void ILI9486_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      ILI9486_drawPixel(y0, x0, color);
    } else {
      ILI9486_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}	
//-------------------------------------------------------------------
void ILI9486_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	ILI9486_drawLine(x, y, x+w-1, y, color);
}
//-------------------------------------------------------------------
void ILI9486_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	ILI9486_drawLine(x, y, x, y+h-1, color);
}
//-------------------------------------------------------------------
void ILI9486_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	ILI9486_drawLine(x0, y0, x1, y1, color);
  ILI9486_drawLine(x1, y1, x2, y2, color);
  ILI9486_drawLine(x2, y2, x0, y0, color);
}
//-------------------------------------------------------------------
void ILI9486_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    ILI9486_drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if(a > b) swap(a,b);
    ILI9486_drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if(a > b) swap(a,b);
    ILI9486_drawFastHLine(a, y, b-a+1, color);
	}
}
//-------------------------------------------------------------------
void ILI9486_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
	if((x >= ILI9486_WIDTH)            || // Clip right
     (y >= ILI9486_HEIGHT)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

  if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = pgm_read_byte(font1+(c*5)+i);
    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          ILI9486_drawPixel(x+i, y+j, color);
        else {  // big size
          ILI9486_fillRect(x+(i*size), y+(j*size), size + x+(i*size), size+1 + y+(j*size), color);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          ILI9486_drawPixel(x+i, y+j, bg);
        else {  // big size
          ILI9486_fillRect(x+i*size, y+j*size, size + x+i*size, size+1 + y+j*size, bg);
        }
      }
      line >>= 1;
    }
  }
}
//-------------------------------------------------------------------
void ILI9486_setTextColor(uint16_t color)
{
  lcdprop.TextColor=color;
}
//-------------------------------------------------------------------
void ILI9486_setBackColor(uint16_t color)
{
  lcdprop.BackColor=color;
}
//-------------------------------------------------------------------
void ILI9486_setFont(sFONT *pFonts)
{
  lcdprop.pFont=pFonts;
}
//-------------------------------------------------------------------
void ILI9486_drawCharF(uint16_t x, uint16_t y, uint8_t c){

	  uint32_t i = 0, j = 0;
	  uint16_t height, width;
	  uint8_t offset;
	  uint8_t *c_t;
	  uint8_t *pchar;
	  uint32_t line=0;
	  height = lcdprop.pFont->Height;
	  width  = lcdprop.pFont->Width;
	  offset = 8 *((width + 7)/8) -  width ;
	  c_t = (uint8_t*) &(lcdprop.pFont->table[(c-' ') * lcdprop.pFont->Height * ((lcdprop.pFont->Width + 7) / 8)]);
	  for(i = 0; i < height; i++)
	  {
	    pchar = ((uint8_t *)c_t + (width + 7)/8 * i);
	    switch(((width + 7)/8))
	    {
	      case 1:
	          line =  pchar[0];
	          break;
	      case 2:
	          line =  (pchar[0]<< 8) | pchar[1];
	          break;
	      case 3:
	      default:
	        line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];
	        break;
	    }
	    for (j = 0; j < width; j++)
	    {
	      if(line & (1 << (width- j + offset- 1)))
	      {
	    	  ILI9486_drawPixel((x + j), y, lcdprop.TextColor);
	      }
	      else
	      {
	    	  ILI9486_drawPixel((x + j), y, lcdprop.BackColor);
	      }
	    }
	    y++;
	  }


}
//-------------------------------------------------------------------
void ILI9486_string(uint16_t x,uint16_t y, char *str)
{
  while(*str)
  {
	 ILI9486_drawCharF(x,y,str[0]);
    x+=lcdprop.pFont->Width;
    (void)*str++;
  }
}
//-------------------------------------------------------------------
void ILI9486_printText(char text[], int16_t x, int16_t y, uint16_t color, uint16_t bg, uint8_t size)
{
	int16_t offset;
	offset = size*6;
	for(uint16_t i=0; i<40 && text[i]!=NULL; i++)
	{
		ILI9486_drawChar(x+(offset*i), y, text[i],color,bg,size);
	}
}
//-------------------------------------------------------------------
//12. Image print (RGB 565, 2 bytes per pixel)
void ILI9486_printImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data, uint32_t size)
{
	uint32_t n = size;
	ILI9486_SetCursorPosition(x, y, w+x-1, h+y-1);
	for(uint32_t i=0; i<n ; i++)
	{
		ILI9486_SendData(data[i]);
	}
}
//-------------------------------------------------------------------
void ILI9486_setRotation(uint8_t rotate)
{
	switch(rotate)
	{
		case 1:
			rotationNum = 1;
			ILI9486_SendCommand(ILI9486_MEMCONTROL);
			ILI9486_SendData(ILI9486_MADCTL_MY | ILI9486_MADCTL_BGR);
			break;
		case 2:
			rotationNum = 2;
			ILI9486_SendCommand(ILI9486_MEMCONTROL);
			ILI9486_SendData(ILI9486_MADCTL_MV | ILI9486_MADCTL_BGR);
			break;
		case 3:
			rotationNum = 3;
			ILI9486_SendCommand(ILI9486_MEMCONTROL);
			ILI9486_SendData(ILI9486_MADCTL_MX | ILI9486_MADCTL_BGR);
			break;
		case 4:
			rotationNum = 4;
			ILI9486_SendCommand(ILI9486_MEMCONTROL);
			ILI9486_SendData(ILI9486_MADCTL_MX | ILI9486_MADCTL_MY | ILI9486_MADCTL_MV | ILI9486_MADCTL_BGR);
			break;
		default:
			rotationNum = 1;
			ILI9486_SendCommand(ILI9486_MEMCONTROL);
			ILI9486_SendData(ILI9486_MADCTL_MY | ILI9486_MADCTL_BGR);
			break;
	}
}
//-------------------------------------------------------------------
uint16_t ILI9486_RandColor(void)
{
	return HAL_RNG_GetRandomNumber(&hrng)&0x0000FFFF;
}
//-------------------------------------------------------------------
void ILI9486_Test(void)
{
	uint16_t i,j;

	ILI9486_fill(COLOR_BLACK);
	HAL_Delay(500);
	for(i=0;i<20;i++)
	    {
	      ILI9486_fill(ILI9486_RandColor());
	      HAL_Delay(150);
	    }
			HAL_Delay(500);
	    ILI9486_fill(COLOR_BLACK);
	    for(i=0;i<20;i++)
	    {
	      ILI9486_fillRect(0, 0, ILI9486_WIDTH/2-1, ILI9486_HEIGHT/2-1, ILI9486_RandColor());
	      HAL_Delay(100);
	      ILI9486_fillRect(ILI9486_WIDTH/2, 0, ILI9486_WIDTH-1, ILI9486_HEIGHT/2-1, ILI9486_RandColor());
	      HAL_Delay(100);
	      ILI9486_fillRect(0, ILI9486_HEIGHT/2, ILI9486_WIDTH/2-1, ILI9486_HEIGHT-1, ILI9486_RandColor());
	      HAL_Delay(100);
	      ILI9486_fillRect(ILI9486_WIDTH/2, ILI9486_HEIGHT/2, ILI9486_WIDTH-1, ILI9486_HEIGHT-1, ILI9486_RandColor());
	      HAL_Delay(100);
	    }
	    HAL_Delay(500);
	    ILI9486_fill(COLOR_BLACK);
	    for(i=0;i<1000;i++)
	    {
	      ILI9486_fillRect(HAL_RNG_GetRandomNumber(&hrng)%ILI9486_WIDTH,
	              HAL_RNG_GetRandomNumber(&hrng)%ILI9486_HEIGHT,
	              HAL_RNG_GetRandomNumber(&hrng)%ILI9486_WIDTH,
	              HAL_RNG_GetRandomNumber(&hrng)%ILI9486_HEIGHT,
	              ILI9486_RandColor());
	      HAL_Delay(10);
	    }
	    HAL_Delay(500);
	    ILI9486_fill(COLOR_BLACK);
	    for(i=0;i<15000;i++)
	    {
	      for(j=0;j<100;j++)
	      {
	        ILI9486_drawPixel(HAL_RNG_GetRandomNumber(&hrng)%ILI9486_WIDTH,
	                          HAL_RNG_GetRandomNumber(&hrng)%ILI9486_HEIGHT,
	                          COLOR_BLACK);
	      }
	      ILI9486_drawPixel(HAL_RNG_GetRandomNumber(&hrng)%ILI9486_WIDTH,
	                          HAL_RNG_GetRandomNumber(&hrng)%ILI9486_HEIGHT,
	                          ILI9486_RandColor());
	      DelayMicro(100);
	    }
	    HAL_Delay(500);
	    for(j=0;j<20;j++)
	    {
	      for(i=0;i<ILI9486_WIDTH;i++)
	      {
	        ILI9486_drawLine(i,0,i,ILI9486_HEIGHT-1,ILI9486_RandColor());
	      }
	    }
	    HAL_Delay(500);
	    ILI9486_fill(COLOR_BLACK);
	    for(i=0;i<1000;i++)
	    {
	      ILI9486_drawLine(
	                      HAL_RNG_GetRandomNumber(&hrng)%ILI9486_WIDTH,
	                      HAL_RNG_GetRandomNumber(&hrng)%ILI9486_HEIGHT,
	                      HAL_RNG_GetRandomNumber(&hrng)%ILI9486_WIDTH,
	                      HAL_RNG_GetRandomNumber(&hrng)%ILI9486_HEIGHT,ILI9486_RandColor());
	      HAL_Delay(10);
	    }
	    HAL_Delay(500);
	    ILI9486_fill(COLOR_BLACK);
	    for(j=0;j<20;j++)
	    {
	      for(i=0;i<ILI9486_WIDTH/2;i++)
	      {
	        ILI9486_drawRect(i, i, ILI9486_WIDTH-i-1, ILI9486_HEIGHT-i-1,ILI9486_RandColor());
	      }
	    }
	    HAL_Delay(500);
	    ILI9486_fill(COLOR_BLACK);
	    for(i=0;i<2000;i++)
	    {
	      ILI9486_drawCircle(HAL_RNG_GetRandomNumber(&hrng)%(ILI9486_WIDTH-40)+20,
	                         HAL_RNG_GetRandomNumber(&hrng)%(ILI9486_HEIGHT-40)+20,
	                         20,ILI9486_RandColor());
	      HAL_Delay(3);
	    }
	    HAL_Delay(500);
	    ILI9486_fill(COLOR_BLACK);
	    ILI9486_drawChar(30, 34, 'S', COLOR_BLACK, COLOR_WHITE, 5);
	    ILI9486_drawChar(50, 44, 'T', COLOR_BLACK, COLOR_WHITE, 5);
	    ILI9486_drawChar(70, 54, 'M', COLOR_BLACK, COLOR_WHITE, 5);
	    ILI9486_drawChar(90, 64, '3', COLOR_BLACK, COLOR_WHITE, 5);
	    ILI9486_drawChar(110, 74, '2', COLOR_BLACK, COLOR_WHITE, 5);
	    HAL_Delay(2000);
	    ILI9486_fill(COLOR_BLACK);
	    ILI9486_drawChar(10, 34, 'S', COLOR_GREEN, COLOR_RED, 3);
	    ILI9486_drawChar(20, 44, 'T', COLOR_GREEN, COLOR_RED, 3);
	    ILI9486_drawChar(30, 54, 'M', COLOR_GREEN, COLOR_RED, 3);
	    ILI9486_drawChar(40, 64, '3', COLOR_GREEN, COLOR_RED, 3);
	    ILI9486_drawChar(50, 74, '2', COLOR_GREEN, COLOR_RED, 3);
	    HAL_Delay(2000);
	    ILI9486_fill(COLOR_BLACK);
	    ILI9486_drawChar(10, 34, 'S', COLOR_BLUE, COLOR_YELLOW, 1);
	    ILI9486_drawChar(15, 44, 'T', COLOR_BLUE, COLOR_YELLOW, 1);
	    ILI9486_drawChar(20, 54, 'M', COLOR_BLUE, COLOR_YELLOW, 1);
	    ILI9486_drawChar(25, 64, '3', COLOR_BLUE, COLOR_YELLOW, 1);
	    ILI9486_drawChar(30, 74, '2', COLOR_BLUE, COLOR_YELLOW, 1);
	    HAL_Delay(2000);
	    ILI9486_fill(COLOR_BLACK);
	    for(i=0;i<16;i++)
	    {
	      ILI9486_setRotation(i%4);
	      ILI9486_printText("ABCDEF12345678", 1, 80, COLOR_WHITE, COLOR_BLACK, 5);
	      ILI9486_printText("ABCDEF12345678", 1, 120, COLOR_WHITE, COLOR_BLACK, 4);
	      ILI9486_printText("ABCDEF12345678RSTUVWXY", 1, 160, COLOR_WHITE, COLOR_BLACK, 3);
	      ILI9486_printText("ABCDEFGHIKLMNOPQRSTUVWXY 123456789", 1, 200, COLOR_WHITE, COLOR_BLACK, 2);
	      ILI9486_printText("ABCDEFGHIKLMNOPQRSTUVWXYZ 123456789ABCDEFGHIKL", 1, 240, COLOR_WHITE, COLOR_BLACK, 1);
	      HAL_Delay(3000);
	      ILI9486_fill(COLOR_BLACK);
	    }
	    HAL_Delay(10000);
	    ILI9486_setRotation(0);

	    void ILI9486_setTextColor(uint16_t color)
{
  lcdprop.TextColor=color;
}
//-------------------------------------------------------------------
void ILI9486_setBackColor(uint16_t color)
{
  lcdprop.BackColor=color;
}
//-------------------------------------------------------------------
void ILI9486_setFont(sFONT *pFonts)
{
  lcdprop.pFont=pFonts;
}
//-------------------------------------------------------------------
void ILI9486_drawCharF(uint16_t x, uint16_t y, uint8_t c){

	  uint32_t i = 0, j = 0;
	  uint16_t height, width;
	  uint8_t offset;
	  uint8_t *c_t;
	  uint8_t *pchar;
	  uint32_t line=0;
	  height = lcdprop.pFont->Height;
	  width  = lcdprop.pFont->Width;
	  offset = 8 *((width + 7)/8) -  width ;
	  c_t = (uint8_t*) &(lcdprop.pFont->table[(c-' ') * lcdprop.pFont->Height * ((lcdprop.pFont->Width + 7) / 8)]);
	  for(i = 0; i < height; i++)
	  {
	    pchar = ((uint8_t *)c_t + (width + 7)/8 * i);
	    switch(((width + 7)/8))
	    {
	      case 1:
	          line =  pchar[0];
	          break;
	      case 2:
	          line =  (pchar[0]<< 8) | pchar[1];
	          break;
	      case 3:
	      default:
	        line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];
	        break;
	    }
	    for (j = 0; j < width; j++)
	    {
	      if(line & (1 << (width- j + offset- 1)))
	      {
	    	  ILI9486_drawPixel((x + j), y, lcdprop.TextColor);
	      }
	      else
	      {
	    	  ILI9486_drawPixel((x + j), y, lcdprop.BackColor);
	      }
	    }
	    y++;
	  }
}
ILI9486_setTextColor(COLOR_YELLOW);
ILI9486_setBackColor(COLOR_BLUE);
ILI9486_setFont(&Font24);
ILI9486_fill(COLOR_BLACK);
ILI9486_string(1,100,"ABCDEF12345678");
ILI9486_setFont(&Font20);
ILI9486_string(1,124,"ABCDEFGHI12345678");
ILI9486_setFont(&Font16);
ILI9486_string(1,144,"ABCDEFGHIKL123456789");
ILI9486_setFont(&Font12);
ILI9486_string(1,160,"ABCDEFGHIKLMNOPQRSTUVWXY 123456789");
ILI9486_setFont(&Font8);
ILI9486_string(1,172,"ABCDEFGHIKLMNOPQRSTUVWXYZ 123456789ABCDEFGHIKL");
HAL_Delay(2000);

}
