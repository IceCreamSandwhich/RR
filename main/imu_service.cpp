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
    imu.rpt.rv_gyro_integrated.enable(1000); // Set report rate in us
    //imu.rpt.linear_accelerometer.enable(1000);
    //imu.rpt.cal_magnetometer.enable(1000);
    

}

void imu_loop(void *pvParameter)
{
    while(1)
    {
        // block until new report is detected
        if (imu.data_available())
        {
            // // get the latest report from Gyro
            // imu.rpt.rv_gyro_integrated.get(quat, omega);
            // euler = quat;
            // ESP_LOGI(TAG, "Roll: %f, Pitch: %f, Yaw: %f", euler.x, euler.y, euler.z);

            // get the linear acceleration data
            /*
            bno08x_accel_t linear_accel;
            linear_accel = imu.rpt.linear_accelerometer.get();
            ESP_LOGI(TAG, "Linear Acceleration -X: %f, Y: %f, Z: %f", 
                linear_accel.rqdata.x, linear_accel.rqdata.y, linear_accel.rqdata.z);

            //get calibrated magnetic field
            bno08x_magf_t mag_field;
            mag_field = imu.rpt.cal_magnetometer.get();
            ESP_LOGI(TAG, "Magnetometer - X: %f, Y: %f, Z: %f", 
            mag_field.rqdata.x, mag_field.rqdata.y, mag_field.rqdata.z);
            */
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
        "imu_task",
        2048,
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