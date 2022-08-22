/**
 * the simplest possible example of using an OLED display with our library
 */

#include <cstdio>
#include "mbed.h"
#include <PlatformDetermination.h>
#include <TaskManager.h>
//#include "Adafruit_SSD1306_I2c.h" (for i2c displays)
#include "Adafruit_SSD1306_Spi.h"
#include <Fonts/FreeSans9pt7b.h>

bool running = true;

//I2C i2c(PF_0,PF_1);
//Adafruit_SSD1306_I2c gfx(i2c, NC, SSD_I2C_ADDRESS, 64, 132, SH_1106);

SPI spi(PB_5, PB_4, PB_3);
Adafruit_SSD1306_Spi gfx(spi, PD_15, PF_12, PF_13, 64, 128, SSD_1306);

int main()
{
    //i2c.frequency(400000);

    gfx.begin();

    gfx.clearDisplay();
    gfx.setFont(&FreeSans9pt7b);
    gfx.setCursor(0, 16);
    gfx.setTextColor(WHITE);
    gfx.print("Hello mbed");


    // actually cause the write to take place.
    gfx.display();

    taskManager.scheduleFixedRate(1, [] {
        gfx.fillRect(0, 20, gfx.width(), 20, BLACK);
        gfx.setFont(nullptr);
        gfx.setCursor(0, 20);
        gfx.print(millis());

        // actually cause the write to take place.
        gfx.display();
    }, TIME_SECONDS);

    while(running) {
        taskManager.runLoop();
    }
}