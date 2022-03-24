/*Near space research project: Coding for motor controller*/

#ifndef MIKROE2766_H
#define MIKROE2766_H 

#include "mbed.h"


class MIKROE2766 {
    public:
        MIKROE2766(I2C* i2c, int addr);

        int setSpeed(int inputSpeed); //Allows the user to set the speed of the motor in RPM
        
        int getSpeed(int &mainRPM); //Returns the motor speed in RPM

        int getCurrent(float &mainCurrent); //Returns the current going into the motor in Amps

        int getVoltage(float &mainVolts); //Returns the voltage available to the motor

        int checkStatus(); //Returns the reported status of the motor

        int checkFaults(); //Will gather the reported faults of the motor 
    
    private:
        I2C* _i2c; 
        int _addr; 
};

#endif