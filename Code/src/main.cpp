#include "mbed.h"
#include <tsl2591.h>
#include <Adafruit_SSD1306_I2c.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/Tiny3x3a2pt7b.h>

I2C i2c(PB_9, PB_8);
TSL2591 sensor(PB_11, PB_10);
Adafruit_SSD1306_I2c gfx(i2c, NC, SSD_I2C_ADDRESS, 32, 128, SH_1106);

char str[10];
float lux;
Gain gain_check;

void begin() {
    gfx.clearDisplay();
    gfx.setCursor(0, 8);
    gfx.setTextColor(WHITE);

    gfx.setFont(&FreeMonoBold9pt7b);
    gfx.print("Light");

    gfx.setFont(&Tiny3x3a2pt7b);
    gfx.write('\n');
    gfx.write('\n');
    gfx.write('\n');
    gfx.write('\n');

    gfx.setFont(&FreeMonoBold9pt7b);
    gfx.print("Sensor");

    gfx.display();
    ThisThread::sleep_for(1000);
}

void disconnected() {
    while (1) {
        gfx.clearDisplay();
        gfx.setCursor(0, 12);
        gfx.setTextColor(WHITE);

        gfx.setFont(&FreeMonoBold9pt7b);
        gfx.print("Sensor");

        gfx.setFont(&Tiny3x3a2pt7b);
        gfx.write('\n');
        gfx.write('\n');
        gfx.write('\n');
        gfx.write('\n');

        gfx.setFont(&FreeMonoBold9pt7b);
        gfx.print("Undetected");

        gfx.display();

        ThisThread::sleep_for(1000);

        gfx.clearDisplay();
        gfx.setCursor(0, 12);
        gfx.setTextColor(WHITE);

        gfx.setFont(&FreeMonoBold9pt7b);
        gfx.print("Please");

        gfx.setFont(&Tiny3x3a2pt7b);
        gfx.write('\n');
        gfx.write('\n');
        gfx.write('\n');
        gfx.write('\n');

        gfx.setFont(&FreeMonoBold9pt7b);
        gfx.print("Reset");

        gfx.display();

        ThisThread::sleep_for(1000);
    }
}

void normal() {
    gfx.clearDisplay();
    gfx.setCursor(0, 12);
    gfx.setTextColor(WHITE);

    gfx.setFont(&FreeMonoBold9pt7b);
    if (lux < 10000.0F) { sprintf(str, "Lux:%.2F", lux); }
    else { sprintf(str, "Lux:%.1F", lux); }
    gfx.print(str);

    gfx.setFont(&Tiny3x3a2pt7b);
    gfx.write('\n');
    gfx.write('\n');
    gfx.write('\n');
    gfx.write('\n');

    gfx.setFont(&FreeMonoBold9pt7b);
    switch (sensor.gain)
    {
    case TSL2591_GAIN_LOW:
        gfx.print("Gain:Low");
        break;
    case TSL2591_GAIN_MED:
        gfx.print("Gain:Med");
        break;
    case TSL2591_GAIN_HIGH:
        gfx.print("Gain:High");
        break;
    case TSL2591_GAIN_MAX:
        gfx.print("Gain:Max");
        break;
    }

    gfx.display();
    ThisThread::sleep_for(1000);
}

void too_bright() {
    gfx.clearDisplay();
    gfx.setCursor(0, 12);
    gfx.setTextColor(WHITE);

    gfx.setFont(&FreeMonoBold9pt7b);
    gfx.print("Too");

    gfx.setFont(&Tiny3x3a2pt7b);
    gfx.write('\n');
    gfx.write('\n');
    gfx.write('\n');
    gfx.write('\n');

    gfx.setFont(&FreeMonoBold9pt7b);
    gfx.print("Bright");

    gfx.display();
    ThisThread::sleep_for(1000);
}

int main()
{
    gfx.begin();
    begin();

    while (1) {
        // check if connected or not
        if (sensor.begin()) {
        detect:
            lux = sensor.calculateLux();
            // check if lux is out of range
            if (lux >= 2400.0F) {
                gain_check = TSL2591_GAIN_LOW;
            }
            else if ((lux >= 125.0F) && (lux < 2400.0F)) {
                gain_check = TSL2591_GAIN_MED;
            }
            else if ((lux >= 3.0F) && (lux < 125.0F)) {
                gain_check = TSL2591_GAIN_HIGH;
            }
            else if ((lux >= 0.0F) && (lux < 3.0F)) {
                gain_check = TSL2591_GAIN_MAX;
            }
            else {
                if (sensor.gain != TSL2591_GAIN_LOW) {
                    sensor.setGain((Gain)(sensor.gain - 0x10));
                    goto detect;
                }
                else {
                    // overflow
                    too_bright();
                    goto detect;
                }
            }

            // display
            if (gain_check == sensor.gain) {
                normal();
            }
            else {
                sensor.setGain(gain_check);
            }
        }
        else {
            disconnected();
            break;
        }
    }
    return 0;
}