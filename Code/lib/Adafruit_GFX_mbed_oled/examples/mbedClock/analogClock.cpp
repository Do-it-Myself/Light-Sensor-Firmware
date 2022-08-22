/**
 * A more complex example that acquires time from the network using NTP and displays it on the screen
 * The NTP class is licensed under the same license as the code, and can be used directly in your code
 * even for commercial designs.
 *
 * MBED 6 RTOS example.
 */

#include <cstdio>
#include "mbed.h"
#include <TaskManager.h>
#include <IoLogging.h>
#include <NetworkInterface.h>
//#include "Adafruit_SSD1306_I2c.h" (for i2c displays)
#include "Adafruit_SSD1306_Spi.h"
#include "NTPTimeEvent.h"

// Host PC Communication channels
BufferedSerial pc(USBTX, USBRX);
MBedLogger LoggingPort(pc);

bool running = true;

//I2C i2c(PF_0,PF_1);
//Adafruit_SSD1306_I2c gfx(i2c, NC, SSD_I2C_ADDRESS, 64, 132, SH_1106);

SPI spi(PB_5, PB_4, PB_3);
Adafruit_SSD1306_Spi gfx(spi, PD_15, PF_12, PF_13, 64, 128, SSD_1306);

// we are going to use NTP, we need to enable the network device to get a reading.
NetworkInterface* defNetwork = NetworkInterface::get_default_instance();

// here we calculate teh centre points of the screen in terms of the clock.
float offsetY = 20.0F + (float(gfx.height() - 20.0F) / 2.0F);
float offsetX = float(gfx.width()) / 2.0F;
float magFactor = (float(gfx.height()) / 2.0F) - 12.0F;

// we need to know the position of the hour hand at each interval, and also the minute / second hand
float degsPerHour = 360.0F / 12.0F;
float degsPerMinute = 360.0F / 60.0F;
#define MATH_PI_FLT 3.141593F

//
// couple of forward declarations
//
void dealWithHand(float degMultiplier, int value, float mag);
void prepareRealtimeClock();

inline float toRadian(float deg) {
    if(deg > 360.0F) deg = deg - 360.0F;
    return (deg * MATH_PI_FLT) / 180.F;
}

int main()
{
    //i2c.frequency(400000);

    gfx.begin();

    prepareRealtimeClock();

    taskManager.scheduleFixedRate(1, [] {
        auto timeNow = time(nullptr);
        auto tm = gmtime(&timeNow);

        gfx.clearDisplay();

        for(int i=0; i<12; i++) {
            float deg = float(i) * degsPerHour;
            float rads = toRadian(deg);
            gfx.fillCircle(offsetX + (magFactor * sin(rads)), offsetY + (magFactor * cos(rads)), 2, WHITE);
        }

        dealWithHand(degsPerHour, 12 - (tm->tm_hour % 12), magFactor * 0.65F);
        dealWithHand(degsPerMinute, 60 - tm->tm_min,  magFactor * 0.75F);
        dealWithHand(degsPerMinute, 60 - tm->tm_sec,  magFactor * 0.95F);

        char sz[30];
        sprintf(sz, "Time: %02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
        gfx.setCursor(0,0);
        gfx.print(sz);

        gfx.display();
    }, TIME_SECONDS);

    while(running) {
        taskManager.runLoop();
    }
}

void dealWithHand(float degMultiplier, int value, float mag) {
    float deg = (float(value) * degMultiplier);
    float rads = toRadian(deg);

    float xEnd = offsetX - (mag * sin(rads));
    float yEnd = offsetY - (mag * cos(rads));
    gfx.drawLine(offsetX, offsetY, xEnd, yEnd, WHITE);
}

class ClockNtpTimeEvent : public NTPTimeEvent {
public:

    ClockNtpTimeEvent(NetworkInterface *interface, const char *timeServer, int timePort)
            : NTPTimeEvent(interface, timeServer, timePort) {
    }

    void exec() override {
        set_time(_presentValue);
        setCompleted(true);
    }
};

void prepareRealtimeClock() {
    if(defNetwork->connect() != NSAPI_ERROR_IS_CONNECTED) {
        taskManager.scheduleOnce(250, prepareRealtimeClock);
        return;
    }

    taskManager.registerEvent(new ClockNtpTimeEvent(defNetwork,"2.pool.ntp.org", 123),  true);
}
