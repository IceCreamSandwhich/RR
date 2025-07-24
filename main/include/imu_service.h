#include "BNO08x.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <fcntl.h>    // for open(), O_WRONLY, O_CREAT, etc.
#include <unistd.h>   // for close(), write(), etc.

void init_imu();
void imu_loop(void *pvParameter);
BaseType_t imu_service(void);
void launch_rr_os_service();
void imu_buf_to_text();
void imu_time_to_buf(int32_t time_ms);
void gyro_data_to_buf(float data);
void data_to_buf(float data);

