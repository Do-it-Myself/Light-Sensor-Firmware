#include <mbed.h>
#include <tsl2591.h>

Serial serial(PA_9, PA_10, 115200);
TSL2591 sensor(PB_11, PB_10);

int main() {
    // Check whether I2C connection is established by verifying chip ID
    if (sensor.begin()) {
        serial.printf("ID found! \n");
    }

    // Print integration time
    serial.printf("Timing: ");
    serial.printf("%d", (sensor.time + 1) * 100);
    serial.printf("\n");

    // Print gain
    serial.printf("Gain: ");
    switch (sensor.gain)
    {
    case TSL2591_GAIN_LOW:
        serial.printf("1x (Low)");
        break;
    case TSL2591_GAIN_MED:
        serial.printf("25x (Medium)");
        break;
    case TSL2591_GAIN_HIGH:
        serial.printf("428x (High)");
        break;
    case TSL2591_GAIN_MAX:
        serial.printf("9876x (Max)");
        break;
    }
    serial.printf("\n");


    for (int d = 1; d <= 5; d++) {
        // Print lux
        serial.printf("Lux: %f\n", sensor.calculateLux());

        // Wait
        wait_ms(640);
    }

    return 0;
}