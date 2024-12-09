#include "pca9685/pca9685.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50

#define DEFAULT_HEAD_TURN_DELAY 120

                                        
#define SERVO_PIN 15  //right motor speed pin ENB connect to PCA9685 port 1

#define FULL_LEFT 400 //ultrasonic sensor facing right
#define SLIGHT_LEFT 340
#define CENTER 280//ultrasonic sensor facing front
#define SLIGHT_RIGHT 220
#define FULL_RIGHT 160 //ultrasonic sensor facing left

#define HEAD_POSITIONS 5
#define int positions[4]

#define TRIG 28 //wPi#28=BCM GPIO#20=Physical pin#38
#define ECHO 29 //wPi#29=BCM GPIO#21=Physical pin#40
#define TARGET_DISTANCE 20
#define short_delay 200
#define long_delay  300
#define extra_long_delay 400

void setup() {

 pinMode(TRIG,OUTPUT);
 pinMode(ECHO,INPUT);

}

float distance() {

        delay(20);
        //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);
 
        //Wait for echo start
        while(digitalRead(ECHO) == LOW);
 
        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH);
        long travelTime = micros() - startTime;
 
        //Get distance in cm
        float distance = travelTime / 58;
         if (distance==0) distance=1000;
        
        return distance;
}

int turnHead(int fd) {
    pca9685PWMWrite(fd, SERVO_PIN, 0, SLIGHT_LEFT)

}


int main(void) {

    if(wiringPiSetup()==-1){
            printf("setup wiringPi failed!\n");
            printf("please check your setup\n");
            return -1;
        }
        setup();

        printf("Part 1: Object Tracking\n");

    int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
    if (fd < 0)
    {
        printf("Error in setup\n");
        return fd;
    }

    pca9685PWMWrite(fd, SERVO_PIN, 0, CENTER);

   
    while(1) {
        float current_distance = distance();
        printf("Distance is: %f\n", current_distance);
        delay(10);
        setMotors(fd, current_distance);
        
    }
}
