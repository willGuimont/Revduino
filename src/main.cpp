#if defined(REVDUINO_MAIN) && !defined(PIO_UNIT_TESTING)

#if !defined(EXAMPLE_COLOR_SENSOR)
#error "Select exactly one Revduino example"
#elif defined(EXAMPLE_COLOR_SENSOR)
#include "../examples/color_sensor/color_sensor.ino"
#endif

#endif
