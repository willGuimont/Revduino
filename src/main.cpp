#if defined(REVDUINO_MAIN) && !defined(PIO_UNIT_TESTING)

#if (defined(EXAMPLE_COLOR_SENSOR) + defined(EXAMPLE_COLOR_SENSOR_STATUS)) != 1
#error "Select exactly one Revduino example"
#elif defined(EXAMPLE_COLOR_SENSOR)
#include "../examples/color_sensor/color_sensor.ino"
#elif defined(EXAMPLE_COLOR_SENSOR_STATUS)
#include "../examples/color_sensor_status/color_sensor_status.ino"
#endif

#endif
