#include "pca9685/pca9685.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50

#define ENA 0  //left motor speed pin ENA connect to PCA9685 port 0
#define ENB 1  //right motor speed pin ENB connect to PCA9685 port 1
#define IN1 4  //Left motor IN1 connect to wPi pin# 4 (Physical 16,BCM GPIO 23)
#define IN2 5  //Left motor IN2 connect to wPi pin# 5 (Physical 18,BCM GPIO 24)
#define IN3 2  //right motor IN3 connect to wPi pin# 2 (Physical 13,BCM GPIO 27)
#define IN4 3  //right motor IN4 connect to wPi pin# 3 (Physical 15,BCM GPIO 22)

//Servo motor
#define SERVO_PIN 15  //right motor speed pin ENB connect to PCA9685 port 1
#define LEFT 400 //ultrasonic sensor facing right
#define CENTER 280//ultrasonic sensor facing front
#define RIGHT 160 //ultrasonic sensor facing left


//Default speed 2000
#define SPEED 3000
#define HIGH_SPEED 4000
#define MIN_SPEED 1500


// SPEED 2000 , factor divider is 1000 
// SPEED divider should be based on how many digits SPEED has
const float MOTOR_FACTOR = SPEED / 100;

#define DEFAULT_HEAD_TURN_DELAY 200

#define MAX_DISTANCE 50.0 //cm
#define STOP_DISTANCE 20.0 //cm
const float DISTANCE_FACTOR = MAX_DISTANCE / 100;

const float L_MOTOR_FACTOR = 0.815;
const float R_MOTOR_FACTOR = 1.0;
const float L_MOTOR_FACTOR_THRESHOLD = 8000.0;
const float R_MOTOR_FACTOR_THRESHOLD = 8000.0;
double current_distance = 0.00;


                                        
#define SERVO_PIN 15  //right motor speed pin ENB connect to PCA9685 port 1
#define LEFT 400 //ultrasonic sensor facing right
#define CENTER 280//ultrasonic sensor facing front
#define RIGHT 160 //ultrasonic sensor facing left
#define TRIG 28 //wPi#28=BCM GPIO#20=Physical pin#38
#define ECHO 29 //wPi#29=BCM GPIO#21=Physical pin#40
#define OBSTACLE 20
#define short_delay 200
#define long_delay  300
#define extra_long_delay 400

void setup() {

 pinMode(IN1,OUTPUT);
 pinMode(IN2,OUTPUT);
 pinMode(IN3,OUTPUT);
 pinMode(IN4,OUTPUT);
 pinMode(TRIG,OUTPUT);
 pinMode(ECHO,INPUT);
 
 digitalWrite(IN1,LOW);
 digitalWrite(IN2,LOW);
 digitalWrite(IN3,LOW);
 digitalWrite(IN4,LOW);

}


double distance() {


        //delay(100);
        //Send trig pulse

        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);
 
	
        //Wait for echo start
        long timeoutStart = micros();
        while(digitalRead(ECHO) == LOW) {
            if(micros() - timeoutStart > 30000) {
                printf("Timeout waiting for echo start\n");
                return -1;
            }
		    
	}
 
        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH);
        long travelTime = micros() - startTime;
 
        //Get distance in cm
        double distance = travelTime / 58;
         if (distance==0) distance=1000;
        
        return distance;
}

void scan_surroundings_track(int fd) {
    // brainstorm 
    // robot will always be near the Stop distance so when it suddenly detects an increase in distance
    // run a function that stops all motors, turn the head and pick which direction has the closest object
    // and turn in that direction
}

// function to set variable speed to motors
void setMotors(int fd, float current_distance) {
    delay(20);
    float leftSpeed = SPEED;
    float rightSpeed = SPEED;
    
    
    printf("comparing distance to max\n");
    if(current_distance <= MAX_DISTANCE) {
        float magnitude = (float)(MAX_DISTANCE - current_distance) / DISTANCE_FACTOR;
        leftSpeed = SPEED - (magnitude * MOTOR_FACTOR);
        rightSpeed = SPEED - (magnitude * MOTOR_FACTOR);

        if(leftSpeed < MIN_SPEED) {
            leftSpeed = MIN_SPEED;
        }

        if(rightSpeed < MIN_SPEED) {
            rightSpeed = MIN_SPEED;
        }
    } else {
        rightSpeed = 0;
        leftSpeed = 0;
    }

    //printf("running limit checks\n");
    // lower limit check
    
    // add in motor compensation
    /*if (leftSpeed <= L_MOTOR_FACTOR_THRESHOLD) {
        leftSpeed *= L_MOTOR_FACTOR;

    }

    
    if (rightSpeed <= R_MOTOR_FACTOR_THRESHOLD) {
        rightSpeed *= R_MOTOR_FACTOR;

    }*/


    
    // check stop distance
    if(current_distance <= STOP_DISTANCE && current_distance >= 10) leftSpeed = 0; 
    if(current_distance <= STOP_DISTANCE  && current_distance >= 10) rightSpeed = 0;

    if(current_distance < 10) {
        leftSpeed = -leftSpeed;   
        rightSpeed = -rightSpeed;
    }


    if(rightSpeed == 0 && leftSpeed == 0) {
        digitalWrite(IN1,LOW);
        digitalWrite(IN2,LOW);
        digitalWrite(IN3,LOW);
        digitalWrite(IN4,LOW); 
        pca9685PWMWrite(fd, ENA, 0, 0);
        pca9685PWMWrite(fd, ENB, 0, 0);
	    //printf("LEFT_SPEED: %f \n", leftSpeed);
	   // printf("RIGHT_SPEED: %f \n", rightSpeed);
        

    } else if(rightSpeed < 0 && leftSpeed < 0) {

        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW); 
        leftSpeed *= L_MOTOR_FACTOR;
        pca9685PWMWrite(fd, ENA, 0, leftSpeed);
        pca9685PWMWrite(fd, ENB, 0, rightSpeed);


    }   else {

        

        digitalWrite(IN1,LOW);
        //printf("IN1 low\n");
        digitalWrite(IN2,HIGH);
        //printf("IN2 high\n");
        digitalWrite(IN3,LOW);
        //printf("IN3 low\n");
        digitalWrite(IN4,HIGH);
	//printf("IN4 high\n");
	
	    //printf("LEFT_SPEED: %f \n", leftSpeed);
	    //printf("RIGHT_SPEED: %f \n", rightSpeed);
	

        leftSpeed *= L_MOTOR_FACTOR;
	
	
	
        pca9685PWMWrite(fd, ENA, 0, leftSpeed);
        pca9685PWMWrite(fd, ENB, 0, rightSpeed);

        
    }
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
        current_distance = distance();
        printf("Distance is: %lf\n", current_distance);
        delay(10);
	    //printf("before calling setMotor main\n");
        setMotors(fd, current_distance);
	    //printf("\n-=-=-=-=-=-setMotors is called-=-=-=-=-=--=-\n");
        
    }
	
}
