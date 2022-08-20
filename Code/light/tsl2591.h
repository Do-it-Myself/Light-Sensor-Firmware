#pragma once
#include <mbed.h>

// address
#define TSL2591_ADDR7BIT (0x29) // Default I2C address - 7 bits
#define TSL2591_ADDR8BIT (TSL2591_ADDR7BIT << 1) // Default I2C address - 8 bits = for MBed as it uses 8-bit address

// values for calculating lux
#define TSL2591_VISIBLE (2)      ///< (channel 0) - (channel 1)
#define TSL2591_INFRARED (1)     ///< channel 1
#define TSL2591_FULLSPECTRUM (0) ///< channel 0
#define TSL2591_LUX_DF (408.0F)   ///< Lux cooefficient
#define TSL2591_LUX_COEFB (1.64F) ///< CH0 coefficient
#define TSL2591_LUX_COEFC (0.59F) ///< CH1 coefficient A
#define TSL2591_LUX_COEFD (0.86F) ///< CH2 coefficient B

// common bytes
#define TSL2591_COMMAND_BIT (0xA0) ///< 1010 0000: bits 7 and 5 for 'command normal'
#define TSL2591_ENABLE_POWEROFF (0x00) ///< Flag for ENABLE register to disable
#define TSL2591_ENABLE_POWERON (0x01)  ///< Flag for ENABLE register to enable
#define TSL2591_ENABLE_AEN (0x02) ///< ALS Enable. This field activates ALS function. Writing a one activates the ALS. Writing a zero disables the ALS.
#define TSL2591_ENABLE_AIEN (0x10) ///< ALS Interrupt Enable. When asserted permits ALS interrupts to be generated, subject to the persist filter.
#define TSL2591_ENABLE_NPIEN (0x80) ///< No Persist Interrupt Enable. When asserted NP Threshold conditions will generate an interrupt, bypassing the persist filter
#define TSL2591_ID (0x50) /// Device ID in REGISTER_ID
#define TSL2591_INTEGRATION_TIME_EXTRACTION (0x07) /// <0000 0111: extract bits 2, 1, 0 from REGISTER_CONTROL
#define TSL2591_GAIN_EXTRACTION (0x30) /// <0011 0000: extract bits 5 and 4 from REGISTER_CONTROL

// register map
enum {
    TSL2591_REGISTER_ENABLE = 0x00, // Enable register
    TSL2591_REGISTER_CONTROL = 0x01, // Control register
    TSL2591_REGISTER_AILTL = 0x04, // ALS low threshold lower byte
    TSL2591_REGISTER_AILTH = 0x05, // ALS low threshold upper byte
    TSL2591_REGISTER_AIHTL = 0x06, // ALS high threshold lower byte
    TSL2591_REGISTER_AIHTH = 0x07, // ALS high threshold upper byte
    TSL2591_REGISTER_NPAILTL = 0x08, // No Persist ALS low threshold lower byte
    TSL2591_REGISTER_NPAILTH = 0x09, // No Persist ALS low threshold higher byte
    TSL2591_REGISTER_NPAIHTL = 0x0A, // No Persist ALS high threshold lower byte
    TSL2591_REGISTER_NPAIHTH = 0x0B, // No Persist ALS high threshold higher byte
    TSL2591_REGISTER_PERSIST = 0x0C, // Interrupt persistence filter
    TSL2591_REGISTER_PID = 0x11, // Package Identification
    TSL2591_REGISTER_ID = 0x12, // Device Identification
    TSL2591_REGISTER_STATUS = 0x13, // Internal Status
    TSL2591_REGISTER_C0DATAL = 0x14, // Channel 0 data, low byte
    TSL2591_REGISTER_C0DATAH = 0x15, // Channel 0 data, high byte
    TSL2591_REGISTER_C1DATAL = 0x16, // Channel 1 data, low byte
    TSL2591_REGISTER_C1DATAH = 0x17, // Channel 1 data, high byte
};

// enumeration for persistance filter
typedef enum {
    TSL2591_PERSIST_EVERY = 0x00, // Every ALS cycle generates an interrupt
    TSL2591_PERSIST_ANY = 0x01,   // Any value outside of threshold range
    TSL2591_PERSIST_2 = 0x02,     // 2 consecutive values out of range
    TSL2591_PERSIST_3 = 0x03,     // 3 consecutive values out of range
    TSL2591_PERSIST_5 = 0x04,     // 5 consecutive values out of range
    TSL2591_PERSIST_10 = 0x05,    // 10 consecutive values out of range
    TSL2591_PERSIST_15 = 0x06,    // 15 consecutive values out of range
    TSL2591_PERSIST_20 = 0x07,    // 20 consecutive values out of range
    TSL2591_PERSIST_25 = 0x08,    // 25 consecutive values out of range
    TSL2591_PERSIST_30 = 0x09,    // 30 consecutive values out of range
    TSL2591_PERSIST_35 = 0x0A,    // 35 consecutive values out of range
    TSL2591_PERSIST_40 = 0x0B,    // 40 consecutive values out of range
    TSL2591_PERSIST_45 = 0x0C,    // 45 consecutive values out of range
    TSL2591_PERSIST_50 = 0x0D,    // 50 consecutive values out of range
    TSL2591_PERSIST_55 = 0x0E,    // 55 consecutive values out of range
    TSL2591_PERSIST_60 = 0x0F,    // 60 consecutive values out of range
} Persist;

// enumeration for integration time
typedef enum {
    TSL2591_TIME_100MS = 0x00, // 100 millis
    TSL2591_TIME_200MS = 0x01, // 200 millis
    TSL2591_TIME_300MS = 0x02, // 300 millis
    TSL2591_TIME_400MS = 0x03, // 400 millis
    TSL2591_TIME_500MS = 0x04, // 500 millis
    TSL2591_TIME_600MS = 0x05, // 600 millis
} IntegrationTime;

// Enumeration for sensor gain
typedef enum {
    TSL2591_GAIN_LOW = 0x00,  /// low gain (1x)
    TSL2591_GAIN_MED = 0x10,  /// medium gain (25x)
    TSL2591_GAIN_HIGH = 0x20, /// medium gain (428x)
    TSL2591_GAIN_MAX = 0x30,  /// max gain (9876x)
} Gain;

class TSL2591 {
public:
    // Class field
    I2C i2c; 
    char data;
    char data_array[2];
    char data_store;
    int time;
    int gain;

    // Constructor 
    TSL2591(PinName SDA, PinName SCL, Gain gain = TSL2591_GAIN_MED, IntegrationTime time = TSL2591_TIME_300MS): 
    i2c(SDA, SCL) {
        setGain(gain);
        setTiming(time);
    }

    // Basic functions
    void write(char address, char data_to_write);
    char read(char address);
    char read16(char address);
    void enable();
    void disable();

    // Configuration
    void setTiming(IntegrationTime time);
    void setGain(Gain gain);

    // Getting setting-related data
    bool begin();
    IntegrationTime getTiming();
    Gain getGain();

    // Getting light-related data
    uint32_t getFullLuminosity();
    uint16_t getLuminosity(uint8_t channel);
    float calculateLux();
};

