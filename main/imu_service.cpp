#include "include/imu_service.h"
#include "include/events.h"
#include <fcntl.h>    // for open(), O_WRONLY, O_CREAT, etc.
#include <unistd.h>   // for close(), write(), etc.
#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"



static BNO08x imu;
bno08x_quat_t quat;
bno08x_euler_angle_t euler;
bno08x_ang_vel_t omega;
char buf[512];
int buf_ret;

static const char *TAG = "IMU";

void init_imu()
{
    ESP_LOGI(TAG, "IMU Enabled");
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

        // print buf
        ESP_LOGI(TAG, "%s", buf);

        // add to text file 
        buf_to_text();
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

void buf_to_text() {
    // int fd; /* file descriptor */
    // int nbytes; /* number of bytes read */
    // int retval; /* return value */

    // if ((fd = open("/Users/miamoto/Documents/Github/RR/main/data_file.txt", O_WRONLY | O_CREAT)) < 0) {
    //     perror("open");
    //     exit(1);
    // }
    // if ((nbytes = write(fd, buf, sizeof(buf))) < 0) {
    //     perror("write");
    //     exit(1);
    // }
    // // close file
    // if ((retval = close(fd)) < 0) {
    //     perror("close");
    //     exit(1);
    // }
    esp_vfs_spiffs_conf_t config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    esp_vfs_spiffs_register(&config);
    /*Create file with name hello.txt */
    ESP_LOGI(TAG, "Creating New file: data.txt");
    FILE *f = fopen("/spiffs/data.txt", "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    ESP_LOGI(TAG, "Writing data to file: data.txt");
    fprintf(f, buf);  // write data to data.txt file
    fclose(f);
    ESP_LOGI(TAG, "File written");
    /* read data from data.txt file */
    // ESP_LOGE(TAG, "Reading data from file: data.txt");
    // FILE *file = fopen("/spiffs/data.txt", "r");
    // if (file == NULL)
    // {
    //     ESP_LOGE(TAG, "File does not exist!");
    // }
    // else
    // {
    //     char line[256];
    //     while (fgets(line, sizeof(line), file) != NULL)
    //     {
    //         printf(line);
    //     }
    //     fclose(file);
    // }
    esp_vfs_spiffs_unregister(NULL);
}