#include "include/drivetrain.hpp"
#include "include/wireless_driving.h"
#include "esp_log.h"


// Motor speed definitions (10-bit PWM: 0-1023)
#define FULL_STOP 0
#define DRIVE_SPEED 512  // 50% speed
#define TURN_SPEED 384   // 37.5% speed for smoother turns
#define do_autonomy false //flag for toogling when you should do autonomous task or not


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
            
        default:
            speed_callback(FULL_STOP, FULL_STOP); // Safety stop
            break;
    }
}


// void autonomous_task()
// {
//     /*
//     Ideas for autonomous code:

//     Use the stop button to toogle a flag back and forth that starts autonomous taks

//     while(falge = true){
//         do autonomous taks     
//     }

//     so when you press stop, it stops the autonomous task 
//     */
//     speed_callback(512, 512);  // Move both motors forward
//     vTaskDelay(3000 / portTICK_PERIOD_MS);

// }