#include "MIKROE2766.h"
#include "mbed.h"

#define REG_ADDR_SPDCTRL1 0x00 
#define REG_ADDR_SPDCTRL2 0x01
#define REG_ADDR_MOTORSPEED1 0x11
#define REG_ADDR_MOTORCURRENT1 0x17
#define REG_ADDR_SUPPLYVOLTAGE 0x1A
#define REG_ADDR_STATUS 0x10
#define REG_ADDR_FAULTCODE 0x1E

#define RAW_SPEED_TO_RPM 3
#define RAW_CURRENT_TO_AMPS .00586
#define RAW_VOLTAGE_TO_VOLTS .11718
#define MAX_SPEED 1530

MIKROE2766::MIKROE2766(I2C* i2c, int addr) {
    _i2c = i2c;
    _addr = addr; 
}

int MIKROE2766::setSpeed(int inputSpeed) {
    //Make sure the inputted speed is not too high
    if (inputSpeed > MAX_SPEED) {
        printf ("\nAttempted to set motor speed higher than max!  (entered: %irpm, max: 1530rpm)\t \bSetting speed to 1530prm\b\n", inputSpeed);
        inputSpeed = MAX_SPEED; 
    }

    //Convert newSpeed from int to uint16_t (from rpm to hz as well)
    uint16_t newSpeed = (inputSpeed / 3.0);

    //Establishing the variables necessary to pull the data
    int nak;
    char spdCtrl1, spdCtrl2;
    char buff[2];

    //Making speed control be a 16 bit char with 1 then the speed bits
    spdCtrl2 = (1<<7) | (newSpeed >> 8);
    printf("spdCtrl2 = %0X\n", spdCtrl2);
    //Putting the address and the speed data into the buffer
    buff[0] = REG_ADDR_SPDCTRL2; 
    buff[1] = spdCtrl2;

    //Writing to spdcontrol2 for the first section of input
    nak = _i2c->write(_addr, buff, 2);
    if (nak) return nak;

    //
    spdCtrl1 = newSpeed & 0xFF;
    buff[0] = REG_ADDR_SPDCTRL1;
    buff[1] = spdCtrl1; 
    printf("spdCtrl1 = %0X\n", spdCtrl1);

    nak = _i2c-> write(_addr, buff, 2); 
    return nak; 
}

int MIKROE2766::getSpeed(int &mainRPM) {
    int nak; 
    char buff[2];
    int16_t rawSpeedOutput;

    buff[0] = REG_ADDR_MOTORSPEED1; 
    
    nak = _i2c->write(_addr, buff, 1, true);
    if (nak) return nak;

    nak = _i2c->read(_addr, buff, 2); 

    rawSpeedOutput = (buff[0]<<8 | buff[1]);  
    mainRPM = (rawSpeedOutput * RAW_SPEED_TO_RPM);    

    return nak; 
}

int MIKROE2766::getCurrent(float &mainCurrent) {
    int nak;
    char buff[2];
    int16_t rawCurrentOutput;

    buff[0] = REG_ADDR_MOTORCURRENT1; 

    nak = _i2c->write(_addr, buff, 1, true); 
    if (nak) return nak; 

    nak = _i2c->read(_addr, buff, 2);

    rawCurrentOutput = (buff[0] << 8 | buff[1]); 
    
    if (rawCurrentOutput >= 1023) {
        mainCurrent = (rawCurrentOutput-1023) * RAW_CURRENT_TO_AMPS;
    }
    else {
        mainCurrent = rawCurrentOutput * RAW_CURRENT_TO_AMPS; 
    }

    return nak; 
}

int MIKROE2766::getVoltage(float &mainVolts) {
    int nak;
    char buff[2]; 
    int8_t rawVoltageOutput; 

    buff[0] = REG_ADDR_SUPPLYVOLTAGE; 

    nak = _i2c->write(_addr, buff, 1, true); 
    if (nak) return nak; 

    nak = _i2c->read(_addr, buff, 1); 

    rawVoltageOutput = (buff[1]<<8) | buff[0]; 
    mainVolts = rawVoltageOutput * RAW_VOLTAGE_TO_VOLTS; 

    return nak; 
}

int MIKROE2766::checkStatus() {
    char buff[2]; 
    int8_t statusCodes; 

    buff[0] = REG_ADDR_STATUS;

    _i2c->write(_addr, buff, 1, true); 

    _i2c->read(_addr, buff, 1); 

    statusCodes = buff[0]; 

    if (statusCodes == 0x00) { 
        printf ("Status is nominal\n");
    }
    else {
        printf ("Status: %x\n", statusCodes);
    }

    return 0;
}

int MIKROE2766::checkFaults() {
    char buff[2]; 
    int8_t faultCodes; 

    buff[0] = REG_ADDR_FAULTCODE;

    _i2c->write(_addr, buff, 1, true); 

    _i2c->read(_addr, buff, 1); 

    faultCodes = buff[0]; 

    if (faultCodes == 0x00) { 
        printf ("No faults reported\n");
    }
    else {
        printf ("Faults: %x\n", faultCodes);
    }

    return 0;
}