
#ifndef _ADAFRUIT_MBEDFORK_SSD1306_SPI_H
#define _ADAFRUIT_MBEDFORK_SSD1306_SPI_H

#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

/** This is the SPI SSD1306 display driver transport class
 *
 */
class Adafruit_SSD1306_Spi : public Adafruit_SSD1306
{
public:
    /** Create a SSD1306 SPI transport display driver instance with the specified DC, RST, and CS pins, as well as the display dimentions
     *
     * Required parameters
     * @param spi - a reference to an initialized SPI object
     * @param DC (Data/Command) pin name
     * @param RST (Reset) pin name
     * @param CS (Chip Select) pin name
     *
     * Optional parameters
     * @param rawHeight - the vertical number of pixels for the display, defaults to 32
     * @param rawWidth - the horizonal number of pixels for the display, defaults to 128
     */
    Adafruit_SSD1306_Spi(SPI &spi, PinName DC, PinName RST, PinName CS, uint8_t rawHieght = 32, uint8_t rawWidth = 128, OledType displayType = SSD_1306)
            : Adafruit_SSD1306(RST, rawHieght, rawWidth)
            , cs(CS,true)
            , dc(DC,false)
            , mDisplayType(displayType)
            , mspi(spi)
    {}

    void toggleDC(int dcState)
    {
        cs = 1;
        dc = dcState;
        wait_us(1);
        cs = 0;
    }

    void command(uint8_t c) override
    {
        toggleDC(0);
        mspi.write(c);
        cs = 1;
    };

    void data(uint8_t c) override
    {
        toggleDC(1);
        mspi.write(c);
        cs = 1;
    };

protected:
    void sendDisplayBuffer() override
    {
        auto rows = HEIGHT / 8;
        auto col = 0U;
        for(uint8_t row=0; row < rows; row++) {
            // for each row we go into command mode and send the new row offset, SH1106 cannot do this
            // automatically, so we need to do it for it.
            command(0xB0 + row);//set page address
            command(col & 0xfU);//set lower column address
            command(0x10U | (col >> 4U));//set higher column address

            toggleDC(1);

            // now we prepare a whole row of data to send over SPI.
            for(int i=0;i<WIDTH;i++) {
                mspi.write(buffer[(row * WIDTH) + i]);
            }
        }

        // make sure we de-select the chip
        cs = 1;
    };

    DigitalOut cs, dc;
    OledType mDisplayType;
    SPI &mspi;
};

#endif //_ADAFRUIT_MBEDFORK_SSD1306_SPI_H
