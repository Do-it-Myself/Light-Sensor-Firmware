/**
 * An example that shows how to render using an SSD1306 display for text
 * and graphics. It should compile for most mbed 5 and 6 boards.
 * Use build flag BUILD_FOR_MBED_6 to target mbed 6.
 */

#include <cstdio>
#include <TaskManager.h>
#include <SwitchInput.h>
#include "mbed.h"
//#include "Adafruit_SSD1306_I2c.h"
#include "Adafruit_SSD1306_Spi.h"
#include <IoLogging.h>
#include <Fonts/FreeSans9pt7b.h>

// Host PC Communication channels
#ifdef BUILD_FOR_MBED_6
BufferedSerial pc(USBTX, USBRX);
#else
Serial pc(USBTX, USBRX);
#endif
MBedLogger LoggingPort(pc);

//I2C i2c(PF_0,PF_1);
//Adafruit_SSD1306_I2c gfx(i2c, NC, SSD_I2C_ADDRESS, 64, 132, SH_1106);

SPI spi(PB_5, PB_4, PB_3);
Adafruit_SSD1306_Spi gfx(spi, PD_15, PF_12, PF_13, 64, 128, SSD_1306);

bool exitApp = false;

const uint8_t iconWifiThreeBar[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03,
        0x30, 0x0c, 0x08, 0x10, 0xc0, 0x03, 0x20, 0x04, 0x80, 0x01, 0x80, 0x01
};
int main()
{
#ifdef BUILD_FOR_MBED_6
    pc.set_baud(115200);
#else
    pc.baud(115200);
#endif

    //i2c.frequency(400000);
    gfx.begin();
    serdebugF("Hello from mbed graphics demo");

    switches.initialise(internalDigitalIo(), true);
    switches.addSwitch(PE_4, [] (pinid_t id, bool held) {
        serdebugF3("Switch Pressed", (int)id, (int) held);
    });
    switches.addSwitch(USER_BUTTON, [] (pinid_t id, bool held) {
        serdebugF3("User Pressed", (int)id, (int) held);
    }, NO_REPEAT, true);

    setupRotaryEncoderWithInterrupt(PE_2, PE_5, []( int val) {
        serdebugF2("Encoder", val);
    });

    serdebugF("Created");

    // Display with the Adafruit Library
    gfx.clearDisplay();

    taskManager.scheduleFixedRate(75, [] {
        gfx.setFont(nullptr);
        gfx.setCursor(0, 0);
        gfx.setTextSize(2);
        gfx.print("mbed demo");
        gfx.fillRect(10, 25, 50, 10, BLACK);
        gfx.setTextSize(1);
        gfx.setCursor(10, 25);
        gfx.print((double) millis() / 1000.0);
        gfx.drawXBitmap(40, 40, iconWifiThreeBar, 16, 12, WHITE);
        gfx.drawCircle(100, 40, 10, WHITE);
        gfx.display();
    });

    while(!exitApp) {
        taskManager.runLoop();
    }
}