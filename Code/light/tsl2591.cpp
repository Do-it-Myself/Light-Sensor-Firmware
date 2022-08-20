#include "tsl2591.h"

/*Basic functions*/
// write the byte content to the specific address
void TSL2591::write(char address, char data_to_write) {
    data = TSL2591_COMMAND_BIT | address;
    i2c.write(TSL2591_ADDR8BIT, &data, 1, true);
    data = data_to_write;
    i2c.write(TSL2591_ADDR8BIT, &data, 1);
}

// read the byte contained in the specific address of the chip, return the byte
char TSL2591::read(char address) {
    data = TSL2591_COMMAND_BIT | address;
    i2c.write(TSL2591_ADDR8BIT, &data, 1, true);
    i2c.read(TSL2591_ADDR8BIT, &data, 1);

    return data;
}

// read the byte contained in the specific address of the chip, return the byte
char TSL2591::read16(char address) {
    data = TSL2591_COMMAND_BIT | address;
    i2c.write(TSL2591_ADDR8BIT, &data, 1, true);
    i2c.read(TSL2591_ADDR8BIT, data_array, 2);

    return *data_array;
}

// enable settings - start measurement
void TSL2591::enable() {
    write(TSL2591_REGISTER_ENABLE, TSL2591_ENABLE_POWERON | TSL2591_ENABLE_AEN | TSL2591_ENABLE_AIEN | TSL2591_ENABLE_NPIEN);
};

// disable settings - end measurement
void TSL2591::disable() {
    write(TSL2591_REGISTER_ENABLE, TSL2591_ENABLE_POWEROFF);
};

/*Configurations*/
// set sensor integration time
void TSL2591::setTiming(IntegrationTime time) {
    enable();

    // read control registry
    data_store = read(TSL2591_REGISTER_CONTROL);

    // clear integration time registry 
    data_store = data_store & 0xF8; /// 1111 1000

    // write integration time while keeping original gain setting
    write(TSL2591_REGISTER_CONTROL, time | data_store);

    this->time = time;

    disable();
};

// set sensor gain
void TSL2591::setGain(Gain gain) {
    enable();

    // read control registry
    data_store = read(TSL2591_REGISTER_CONTROL);

    // clear gain registry 
    data_store = data_store & 0xCF; /// 1100 1111

    // write integration time while keeping original gain setting
    write(TSL2591_REGISTER_CONTROL, gain | data_store);

    this->gain = gain;

    disable();

};

/*Getting setting-related data*/
// detect whether i2c connection is established by reading chip ID
bool TSL2591::begin() {
    // read chip ID via I2C connection
    enable();
    data_store = read(TSL2591_REGISTER_ID);
    disable();

    // check if data fetched = ID
    if (data_store == TSL2591_ID) {
        return true;
    }
    else {
        return false;
    }
};

// get sensor integration time
IntegrationTime TSL2591::getTiming() {
    enable();

    // read control registry
    data_store = read(TSL2591_REGISTER_CONTROL);

    // extract bits for integration time
    data_store = data_store & TSL2591_INTEGRATION_TIME_EXTRACTION;

    disable();

    return (IntegrationTime)data_store;
}

// get sensor gain
Gain TSL2591::getGain() {
    enable();

    // read control registry
    data_store = read(TSL2591_REGISTER_CONTROL);

    // extract bits for integration time
    data_store = data_store & TSL2591_GAIN_EXTRACTION;

    disable();

    return (Gain)data_store;
}

/*Getting light-related data*/
// calculate full luminosity
uint32_t TSL2591::getFullLuminosity() {
    enable();

    // wait x ms for ADC to complete
    for (int d = 0; d <= time; d++) {
        wait_ms(120);
    }

    // read channel byte - note auto increment protocol
    read16(TSL2591_REGISTER_C0DATAL);
    char ch0_low = data_array[0];
    char ch0_high = data_array[1];

    read16(TSL2591_REGISTER_C1DATAL);
    char ch1_low = data_array[0];
    char ch1_high = data_array[1];

    uint16_t ch0_data = ch0_low | (ch0_high << 8);
    uint16_t ch1_data = ch1_low | (ch1_high << 8);
    uint32_t channel_data = ch0_data | (ch1_data << 16);

    disable();

    return channel_data;
}

// calculate luminosity
uint16_t TSL2591::getLuminosity(uint8_t channel) {
    uint32_t channel_data = getFullLuminosity();

    if (channel == TSL2591_FULLSPECTRUM) {
        // Reads two byte value from channel 0 (visible + infrared)
        return (channel_data & 0xFFFF);
    }
    else if (channel == TSL2591_INFRARED) {
        // Reads two byte value from channel 1 (infrared)
        return (channel_data >> 16);
    }
    else if (channel == TSL2591_VISIBLE) {
        // Reads all and subtracts out just the visible!
        return ((channel_data & 0xFFFF) - (channel_data >> 16));
    }

    // unknown channel
    return 0;
}

// calculate lux
float TSL2591::calculateLux() {
    uint32_t lum = getFullLuminosity();
    uint16_t ch1, ch0;
    ch1 = lum >> 16;
    ch0 = lum & 0xFFFF;

    float a_time, a_gain;
    float cpl, lux;

    // Check for overflow conditions first
    if ((ch0 == 0xFFFF) | (ch1 == 0xFFFF)) {
        return -1; // Signal an overflow
    };

    // Note: This algorithm is based on preliminary coefficients
    // provided by AMS and may need to be updated in the future
    switch (time) {
    case TSL2591_TIME_100MS:
        a_time = 100.0F;
        break;
    case TSL2591_TIME_200MS:
        a_time = 200.0F;
        break;
    case TSL2591_TIME_300MS:
        a_time = 300.0F;
        break;
    case TSL2591_TIME_400MS:
        a_time = 400.0F;
        break;
    case TSL2591_TIME_500MS:
        a_time = 500.0F;
        break;
    case TSL2591_TIME_600MS:
        a_time = 600.0F;
        break;
    default: // 100ms
        a_time = 100.0F;
        break;
    }

    switch (gain) {
    case TSL2591_GAIN_LOW:
        a_gain = 1.0F;
        break;
    case TSL2591_GAIN_MED:
        a_gain = 25.0F;
        break;
    case TSL2591_GAIN_HIGH:
        a_gain = 428.0F;
        break;
    case TSL2591_GAIN_MAX:
        a_gain = 9876.0F;
        break;
    default:
        a_gain = 1.0F;
        break;
    }

    cpl = (a_time * a_gain) / TSL2591_LUX_DF;
    lux = (((float)ch0 - (float)ch1)) * (1.0F - ((float)ch1 / (float)ch0)) / cpl;

    return lux;
}
