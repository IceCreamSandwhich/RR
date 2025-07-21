#ifndef WIRELESS_DRIVING_H
#define WIRELESS_DRIVING_H

#include <stdint.h>

// Motor speed definitions (10-bit PWM: 0-1023)
#define FULL_STOP 0
#define DRIVE_SPEED 512  // 50% speed
#define TURN_SPEED 384   // 37.5% speed for smoother turns

// Command mappings
#define CMD_STOP     0
#define CMD_FORWARD  1
#define CMD_BACKWARD 2
#define CMD_LEFT     4
#define CMD_RIGHT    8

/**
 * @brief Processes driving commands from webpage WebSocket and controls motors
 * 
 * @param command The received command (0=stop, 1=forward, 2=backward, 4=left, 8=right)
 * 
 * Converts website button commands to motor movements:
 * - 0 = Stop
 * - 1 = Forward (both motors forward)
 * - 2 = Backward (both motors backward)
 * - 4 = Left (left motor back, right motor forward)
 * - 8 = Right (right motor back, left motor forward)
 * 
 * Any other value triggers safety stop
 */
void process_drive_command(int command);

#endif // WIRELESS_DRIVING_H