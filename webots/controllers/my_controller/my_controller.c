#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <stdio.h>

#define TIME_STEP 32

int main() {
  wb_robot_init();

  WbDeviceTag ds = wb_robot_get_device("ds7");
  wb_distance_sensor_enable(ds, TIME_STEP);

  while (wb_robot_step(TIME_STEP) != -1) {
    double dist = wb_distance_sensor_get_value(ds);
    printf("sensor value is %f\n", dist);
  }

  wb_robot_cleanup();

  return 0;
}