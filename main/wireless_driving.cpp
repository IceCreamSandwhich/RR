#include "include/drivetrain.hpp"
#include "include/wireless_driving.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdbool.h>

// Motor speed definitions (10-bit PWM: 0-1023)
#define FULL_STOP 0
#define DRIVE_SPEED 512  // 50% speed
#define TURN_SPEED 384   // 37.5% speed for smoother turns
bool do_autonomy = false; // Flag for toogling when you should do autonomous task or not

void autonomous_task();

void process_drive_command(int command) 
{
    /* 
     * Converts website button commands to motor movements:
     * Command Mapping:
     * 0 = Stop
     * 1 = Forward
     * 2 = Backward
     * 4 = Left 
     * 8 = Right
     * 9 = Autonomous mode
     */
    switch(command) {
        case 0: // STOP
            speed_callback(FULL_STOP, FULL_STOP);
            
            break;
            
        case 1: // FORWARD (both motors forward)
            speed_callback(DRIVE_SPEED, DRIVE_SPEED);
            break;
            
        case 2: // BACKWARD (both motors backward)
            speed_callback(-DRIVE_SPEED, -DRIVE_SPEED);
            break;
            
        case 4: // LEFT (left motor back, right motor forward)
            speed_callback(-TURN_SPEED, TURN_SPEED);
            break;
            
        case 8: // RIGHT (right motor back, left motor forward)
            speed_callback(TURN_SPEED, -TURN_SPEED);
            break;

        case 9: // Autonomous driving Mode
            do_autonomy = !do_autonomy;
            printf("the value of the flag is: %d\n", do_autonomy);
            autonomous_task();
            break;
            
        default:
            speed_callback(FULL_STOP, FULL_STOP); // Safety stop
            break;
    }
}


void autonomous_task()
{
//     /*
//     Ideas for autonomous code:

//     Use the buttons to toogle a flag back and forth that starts autonomous tak

//     if do_autonomy:
    // {
//         do autonomous taks
           //speed_callback(512, 512);  // Move both motors forward
//     }

        //else:
            //stop
//     so when you press it agin, it stops the autonomous task 
//     */
//     
    if (do_autonomy)
    {
        printf("Starting autonomous task");

    }
    
    else
    {
        printf("Ending autonomous task");
    }
}