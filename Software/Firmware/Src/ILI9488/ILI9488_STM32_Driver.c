/* Includes ------------------------------------------------------------------*/
#include "ILI9488_STM32_Driver.h"
#include "spi.h"
#include "gpio.h"

/* Global Variables ------------------------------------------------------------------*/
static __IO uint16_t LCD_HEIGHT = ILI9488_SCREEN_HEIGHT;
static __IO uint16_t LCD_WIDTH	 = ILI9488_SCREEN_WIDTH;

/*Send data (char) to LCD*/
void ILI9488_SPI_Send(uint8_t SPI_Data)
{
	for(;;)
		if(__HAL_SPI_GET_FLAG(HSPI_INSTANCE, SPI_FLAG_TXE))
			break;
	*((__IO uint8_t *)HSPI_INSTANCE.Instance->DR) = SPI_Data;
}

/* Send command (char) to LCD */
void ILI9488_Write_Command(uint8_t Command)
{
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	ILI9488_SPI_Send(Command);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

/* Send Data (char) to LCD */
void ILI9488_Write_Data(uint8_t Data)
{
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	ILI9488_SPI_Send(Data);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

/* Set Address - Location block - to draw into */
void ILI9488_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2)
{
	ILI9488_Write_Command(0x2A);
	ILI9488_Write_Data(X1 >> 8);
	ILI9488_Write_Data(X1);
	ILI9488_Write_Data(X2 >> 8);
	ILI9488_Write_Data(X2);

	ILI9488_Write_Command(0x2B);
	ILI9488_Write_Data(Y1 >> 8);
	ILI9488_Write_Data(Y1);
	ILI9488_Write_Data(Y2 >> 8);
	ILI9488_Write_Data(Y2);

	ILI9488_Write_Command(0x2C);
}

/*Set rotation of the screen - changes x0 and y0*/
void ILI9488_Set_Rotation(uint8_t Rotation)
{
	uint8_t screen_rotation = Rotation;
	ILI9488_Write_Command(0x36);
	HAL_Delay(1);
	switch(screen_rotation)
	{
		case SCREEN_VERTICAL_1:
			ILI9488_Write_Data(0x40 | 0x08);
			LCD_WIDTH = ILI9488_SCREEN_HEIGHT;
			LCD_HEIGHT = ILI9488_SCREEN_WIDTH;
			break;
		case SCREEN_HORIZONTAL_1:
			ILI9488_Write_Data(0x20 | 0x08);
			LCD_WIDTH  = ILI9488_SCREEN_WIDTH;
			LCD_HEIGHT = ILI9488_SCREEN_HEIGHT;
			break;
		case SCREEN_VERTICAL_2:
			ILI9488_Write_Data(0x80 | 0x08);
			LCD_WIDTH  = ILI9488_SCREEN_HEIGHT;
			LCD_HEIGHT = ILI9488_SCREEN_WIDTH;
			break;
		case SCREEN_HORIZONTAL_2:
			ILI9488_Write_Data(0x40 | 0x80 | 0x20 | 0x08);
			LCD_WIDTH  = ILI9488_SCREEN_WIDTH;
			LCD_HEIGHT = ILI9488_SCREEN_HEIGHT;
			break;
		default:
			//EXIT IF SCREEN ROTATION NOT VALID!
			break;
	}
}

/*Enable LCD led*/
void ILI9488_LedEnable(void)
{
	HAL_GPIO_WritePin(LCD_LED_PORT, LCD_LED_PIN, GPIO_PIN_SET);
}

/*Disable LCD led*/
void ILI9488_LedDisable(void)
{
	HAL_GPIO_WritePin(LCD_LED_PORT, LCD_LED_PIN, GPIO_PIN_RESET);
}

/*Initialize and turn On the LCD display*/
void ILI9488_Init(void)
{
	SPI_1LINE_TX(HSPI_INSTANCE);	 /* Configure communication direction : 1Line */
	__HAL_SPI_ENABLE(HSPI_INSTANCE); /* Enable SPI peripheral */
	// ---
	ILI9488_Write_Command(0x01); // Soft Reset
	// Most of commands from https://github.com/jaretburkett/ILI9488
	ILI9488_Write_Command(0xE0);
	ILI9488_Write_Data(0x00);
	ILI9488_Write_Data(0x03);
	ILI9488_Write_Data(0x09);
	ILI9488_Write_Data(0x08);
	ILI9488_Write_Data(0x16);
	ILI9488_Write_Data(0x0A);
	ILI9488_Write_Data(0x3F);
	ILI9488_Write_Data(0x78);
	ILI9488_Write_Data(0x4C);
	ILI9488_Write_Data(0x09);
	ILI9488_Write_Data(0x0A);
	ILI9488_Write_Data(0x08);
	ILI9488_Write_Data(0x16);
	ILI9488_Write_Data(0x1A);
	ILI9488_Write_Data(0x0F);

	ILI9488_Write_Command(0XE1);
	ILI9488_Write_Data(0x00);
	ILI9488_Write_Data(0x16);
	ILI9488_Write_Data(0x19);
	ILI9488_Write_Data(0x03);
	ILI9488_Write_Data(0x0F);
	ILI9488_Write_Data(0x05);
	ILI9488_Write_Data(0x32);
	ILI9488_Write_Data(0x45);
	ILI9488_Write_Data(0x46);
	ILI9488_Write_Data(0x04);
	ILI9488_Write_Data(0x0E);
	ILI9488_Write_Data(0x0D);
	ILI9488_Write_Data(0x35);
	ILI9488_Write_Data(0x37);
	ILI9488_Write_Data(0x0F);

	ILI9488_Write_Command(0XC0);      //Power Control 1
	ILI9488_Write_Data(0x17);    //Vreg1out
	ILI9488_Write_Data(0x15);    //Verg2out

	ILI9488_Write_Command(0xC1);      //Power Control 2
	ILI9488_Write_Data(0x41);    //VGH,VGL

	ILI9488_Write_Command(0xC5);      //Power Control 3
	ILI9488_Write_Data(0x00);
	ILI9488_Write_Data(0x12);    //Vcom
	ILI9488_Write_Data(0x80);

	ILI9488_Write_Command(0x36);      //Memory Access
	ILI9488_Write_Data(0x48);

	ILI9488_Write_Command(0x3A);      // Interface Pixel Format
	ILI9488_Write_Data(0x66); 	  //18 bit

	ILI9488_Write_Command(0XB0);      // Interface Mode Control
	ILI9488_Write_Data(0x80);     			 //SDO NOT USE

	ILI9488_Write_Command(0xB1);    // Frame rate
	ILI9488_Write_Data(0xC0);       // 78Hz
	ILI9488_Write_Data(0x11);		// 17 clocks

	ILI9488_Write_Command(0xB4);      //Display Inversion Control
	ILI9488_Write_Data(0x02);    //2-dot

	ILI9488_Write_Command(0XB6);      //Display Function Control  RGB/MCU Interface Control

	ILI9488_Write_Data(0x02);    //MCU
	ILI9488_Write_Data(0x02);

	ILI9488_Write_Command(0XE9);
	ILI9488_Write_Data(0x00);    // Disable 24 bit data

	ILI9488_Write_Command(0xF7);      // Adjust Control
	ILI9488_Write_Data(0xA9);
	ILI9488_Write_Data(0x51);
	ILI9488_Write_Data(0x2C);
	ILI9488_Write_Data(0x82);    // D7 stream, loose

	ILI9488_Write_Command(0x11);    //Exit Sleep
	HAL_Delay(120);
	ILI9488_Write_Command(0x29);    //Display on
	HAL_Delay(500);
	ILI9488_Set_Rotation(SCREEN_HORIZONTAL_1);
}

/*Turn Off and enter to sleep mode the display*/
void ILI9488_TurnOff(void)
{
	ILI9488_Write_Command(0x28); // Display OFF
	HAL_Delay(120);
	ILI9488_Write_Command(0x10); // Sleep IN
	HAL_Delay(500);
}

//FILL THE ENTIRE SCREEN WITH SELECTED COLOUR
/*Sets address (entire screen) and Sends Height*Width amount of colour information to LCD*/
void ILI9488_Fill_Screen(uint16_t Colour)
{
	ILI9488_Set_Address(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
	ILI9488_Draw_Colour_Burst(Colour, LCD_WIDTH_x_LCD_HEIGHT);
}

//INTERNAL FUNCTION OF LIBRARY
/*Sends block colour information to LCD*/
void ILI9488_Draw_Colour_Burst(uint16_t Colour, uint32_t Size)
{
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	uint8_t data[3];
	data[0] = (((Colour & 0xF800) >> 11) * 255) / 31; // r
	data[1] = (((Colour & 0x07E0) >> 5) * 255) / 63;  // g
	data[2] = ((Colour & 0x001F) * 255) / 31;		  // b
	for(; Size; Size--)
		for(uint8_t i = 0; i < 3;)
			if(__HAL_SPI_GET_FLAG(HSPI_INSTANCE, SPI_FLAG_TXE))
				*((__IO uint8_t *)HSPI_INSTANCE.Instance->DR) = data[i++];
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

//DRAW PIXEL AT XY POSITION WITH SELECTED COLOUR
//
//Location is dependent on screen orientation. x0 and y0 locations change with orientations.
//Using pixels to draw big simple structures is not recommended as it is really slow
//Try using either rectangles or lines if possible
//
void ILI9488_Draw_Pixel(uint16_t X, uint16_t Y, uint16_t Colour)
{
	ILI9488_Set_Address(X, Y, X, Y);
	//COLOUR
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	uint8_t data[3];
	data[0] = (((Colour & 0xF800) >> 11) * 255) / 31;
	data[1] = (((Colour & 0x07E0) >> 5) * 255) / 63;
	data[2] = ((Colour & 0x001F) * 255) / 31;
	HAL_SPI_Transmit(HSPI_INSTANCE, data, 3, 1);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

//DRAW RECTANGLE OF SET SIZE AND HEIGTH AT X and Y POSITION WITH CUSTOM COLOUR
//
//Rectangle is hollow. X and Y positions mark the upper left corner of rectangle
//As with all other draw calls x0 and y0 locations dependent on screen orientation
//
void ILI9488_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Colour)
{
	ILI9488_Set_Address(X, Y, X+Width-1, Y+Height-1);
	ILI9488_Draw_Colour_Burst(Colour, Height*Width);
}

//DRAW LINE FROM X,Y LOCATION to X+Width,Y LOCATION
void ILI9488_Draw_Horizontal_Line(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Colour)
{
	ILI9488_Set_Address(X, Y, X+Width-1, Y);
	ILI9488_Draw_Colour_Burst(Colour, Width);
}

//DRAW LINE FROM X,Y LOCATION to X,Y+Height LOCATION
void ILI9488_Draw_Vertical_Line(uint16_t X, uint16_t Y, uint16_t Height, uint16_t Colour)
{
	ILI9488_Set_Address(X, Y, X, Y+Height-1);
	ILI9488_Draw_Colour_Burst(Colour, Height);
}
