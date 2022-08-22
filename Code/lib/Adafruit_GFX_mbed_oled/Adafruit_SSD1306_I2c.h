
#ifndef _ADAFRUIT_MBEDFORK_SSD1306_I2C_H
#define _ADAFRUIT_MBEDFORK_SSD1306_I2C_H

#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

/** This is the I2C SSD1306 display driver transport class
 *
 */
class Adafruit_SSD1306_I2c : public Adafruit_SSD1306
{
public:
#define SSD_I2C_ADDRESS     0x78
    /** Create a SSD1306 I2C transport display driver instance with the specified RST pin name, the I2C address, as well as the display dimensions
     *
     * Required parameters
     * @param i2c - A reference to an initialized I2C object
     * @param RST - The Reset pin name
     *
     * Optional parameters
     * @param i2cAddress - The i2c address of the display
     * @param rawHeight - The vertical number of pixels for the display, defaults to 32
     * @param rawWidth - The horizonal number of pixels for the display, defaults to 128
     */
    Adafruit_SSD1306_I2c(I2C &i2c, PinName RST, uint8_t i2cAddress = SSD_I2C_ADDRESS, uint8_t rawHeight = 32, uint8_t rawWidth = 128, OledType displayType = SSD_1306)
            : Adafruit_SSD1306(RST, rawHeight, rawWidth)
            , mi2c(i2c)
            , mi2cAddress(i2cAddress)
            , mDisplayType(displayType) { }

    virtual ~Adafruit_SSD1306_I2c() = default;

    void command(uint8_t c) override
    {
        char buff[2];
        buff[0] = 0; // Command Mode
        buff[1] = c;
        mi2c.write(mi2cAddress, buff, sizeof(buff));
    }

    void data(uint8_t c) override
    {
        char buff[2];
        buff[0] = 0x40; // Data Mode
        buff[1] = c;
        mi2c.write(mi2cAddress, buff, sizeof(buff));
    };

protected:
    void sendDisplayBuffer() override
    {
        char buff[133];

        auto rows = uint8_t(HEIGHT / 8);
        uint16_t col = 0;
        for(uint8_t row=0; row < rows; row++) {
            // for each row we go into command mode and send the new row offset, SH1106 cannot do this
            // automatically, so we need to do it and it's compatible with SSD1306 too.
            command(0xB0 + row);//set page address
            command(col & 0xfU);//set lower column address
            command(0x10U | (col >> 4U));//set higher column address

            buff[0] = 0x40; // start data mode
            // now we prepare a whole row of data, no need to mess around with smaller segments, then dump
            // the lot into i2c.
            for(int i=0;i<WIDTH;i++) {
                buff[i + 1] = buffer[(row * WIDTH) + i];
            }
            mi2c.write(mi2cAddress, buff, WIDTH);
        }
    }

    I2C &mi2c;
    uint8_t mi2cAddress;
    OledType mDisplayType;
};

#endif //_ADAFRUIT_MBEDFORK_SSD1306_I2C_H
