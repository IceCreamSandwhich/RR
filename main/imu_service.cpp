#include "include/imu_service.h"
#include "include/events.h"
#include <fcntl.h>    // for open(), O_WRONLY, O_CREAT, etc.
#include <unistd.h>   // for close(), write(), etc.
#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include <string.h>



static BNO08x imu;
bno08x_quat_t quat;
bno08x_euler_angle_t euler;
bno08x_ang_vel_t omega;
char buf[512];
int buf_ret;

static const char *TAG = "IMU";

void init_imu()
{
    ESP_LOGI(TAG, "IMU enabled");
    // initialize imu
    if (!imu.initialize())
    {
        ESP_LOGE(TAG, "Init failure, returning from main.");
        return;
    }
    imu.rpt.rv_gyro_integrated.enable(100000UL); // 100,000us == 100ms report interval
    imu.rpt.linear_accelerometer.enable(100000UL);
    imu.rpt.accelerometer.enable(100000UL);
    imu.rpt.cal_magnetometer.enable(100000UL);
}

void imu_loop(void *pvParameter)
{
    UBaseType_t stack_remaining;
    while(1)
    {
        // block until new report is detected
        if (imu.data_available())
        {
            stack_remaining = uxTaskGetStackHighWaterMark(NULL);
            ESP_LOGI(TAG, "Stack remaining: %u", stack_remaining);
            
            // Get the latest report from Gyro
            if (imu.rpt.rv_gyro_integrated.has_new_data())
            {
                imu.rpt.rv_gyro_integrated.get(quat, omega);
                euler = quat;

                // write to buf
                size_t len = strlen(buf);
                if ((buf_ret = snprintf(buf + len, sizeof(buf) - len, "Roll: %f, Pitch: %f, Yaw: %f\n", euler.x, euler.y, euler.z)) < 0) {
                    ESP_LOGE(TAG, "Failed to write to buffer");
                }
                // ESP_LOGI(TAG, "Roll: %f, Pitch: %f, Yaw: %f", euler.x, euler.y, euler.z);
            }

            // Get linear acceleration data
            if (imu.rpt.linear_accelerometer.has_new_data())
            {
                bno08x_accel_t lin_accel = imu.rpt.accelerometer.get();
                // write to buf
                size_t len = strlen(buf);
                if ((buf_ret = snprintf(buf + len, sizeof(buf) - len, "Linear Accel: (x: %.2f y: %.2f z: %.2f)[m/s^2]\n", lin_accel.x, lin_accel.y, lin_accel.z)) < 0) {
                    ESP_LOGE(TAG, "Failed to write to buffer");
                }
                // ESP_LOGI(TAG, "Linear Accel: (x: %.2f y: %.2f z: %.2f)[m/s^2]", lin_accel.x, lin_accel.y, lin_accel.z);
            }

            // Get Angular acceleration data
            if (imu.rpt.accelerometer.has_new_data())
            {
                bno08x_accel_t ang_accel = imu.rpt.accelerometer.get();
                // write to buf
                size_t len = strlen(buf);
                if ((buf_ret = snprintf(buf + len, sizeof(buf) - len, "Angular Accel: (x: %.2f y: %.2f z: %.2f)[m/s^2]\n", ang_accel.x, ang_accel.y, ang_accel.z)) < 0) {
                    ESP_LOGE(TAG, "Failed to write to buffer");
                }
                // ESP_LOGW(TAG, "Angular Accel: (x: %.2f y: %.2f z: %.2f)[m/s^2]", ang_accel.x, ang_accel.y, ang_accel.z);
            }
            // Get calibrated magnetic field
            if (imu.rpt.cal_magnetometer.has_new_data())
            {
                bno08x_magf_t mag_data = imu.rpt.cal_magnetometer.get();
                // write to buf
                size_t len = strlen(buf);
                if ((buf_ret = snprintf(buf + len, sizeof(buf) - len, "Mag Field: (magx: %.2f magy: %.2f magz: %.2f)[T]\n", mag_data.x, mag_data.y, mag_data.z)) < 0) {
                    ESP_LOGE(TAG, "Failed to write to buffer");
                }
                // ESP_LOGI(TAG, "Mag Field: (magx: %.2f magy: %.2f magz: %.2f)[T]", mag_data.x, mag_data.y, mag_data.z);
            }
        }
        else 
        {
            ESP_LOGI(TAG, "No data available");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // add to text file 
        strcat(buf, "hello world\n");
        imu_buf_to_text();
    }
}

BaseType_t imu_service(void)
{
    BaseType_t status;
    status = xTaskCreate(
        imu_loop,
        "imu_loop",
        4096,
        NULL,
        5,
        NULL);
    
    if (status == pdPASS)
    {
        ESP_LOGI(TAG, "service started");
    }
    else
    {
        ESP_LOGI(TAG, "Error starting the service");
    }
    return status;
}

void imu_buf_to_text() {  
    // write from buf to file
    FILE *f = fopen("/storage/IMU_data.txt", "a");  // "a" to append
    if (f == NULL) {
    ESP_LOGE("FILE", "Failed to open file for writing");
    } 
    else {
        fwrite(buf, 1, strlen(buf), f);  // write the buffer
        fclose(f);
        ESP_LOGI("FILE", "Data written to file");

    }   

    // read into terminal
    // f = fopen("/storage/IMU_data.txt", "r");
    // if (f == NULL) {
    //     ESP_LOGE("FILE", "Failed to open file for reading");
    // } else {
    //     ESP_LOGI("FILE", "Reading file contents:");
    //     char line[128];
    //     while (fgets(line, sizeof(line), f)) {
    //         printf("%s", line);  // or use ESP_LOGI if you prefer
    //     }
    //     fclose(f);
    // }

    // esp_vfs_spiffs_unregister(NULL);
}