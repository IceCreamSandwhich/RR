#include "include/imu_service.h"
#include "include/events.h"

static BNO08x imu;
bno08x_quat_t quat;
bno08x_euler_angle_t euler;
bno08x_ang_vel_t omega;

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
                ESP_LOGI(TAG, "Roll: %f, Pitch: %f, Yaw: %f", euler.x, euler.y, euler.z);
            }

            // Get linear acceleration data
            if (imu.rpt.linear_accelerometer.has_new_data())
            {
                bno08x_accel_t lin_accel = imu.rpt.accelerometer.get();
                ESP_LOGI(TAG, "Linear Accel: (x: %.2f y: %.2f z: %.2f)[m/s^2]", lin_accel.x, lin_accel.y, lin_accel.z);
            }

            // Get Angular acceleration data
            if (imu.rpt.accelerometer.has_new_data())
            {
                bno08x_accel_t ang_accel = imu.rpt.accelerometer.get();
                ESP_LOGW(TAG, "Angular Accel: (x: %.2f y: %.2f z: %.2f)[m/s^2]", ang_accel.x, ang_accel.y, ang_accel.z);
            }
            // Get calibrated magnetic field
            if (imu.rpt.cal_magnetometer.has_new_data())
            {
                bno08x_magf_t mag_data = imu.rpt.cal_magnetometer.get();
                ESP_LOGI(TAG, "Mag Field: (magx: %.2f magy: %.2f magz: %.2f)[T]", mag_data.x, mag_data.y, mag_data.z);
            }
        }
        else 
        {
            ESP_LOGI(TAG, "No data available");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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