#if defined(REVDUINO_MAIN) && !defined(PIO_UNIT_TESTING)

#if (defined(EXAMPLE_COLOR_SENSOR) + defined(EXAMPLE_NORMALIZED_COLOR)) != 1
#error "Select exactly one Revduino example"
#elif defined(EXAMPLE_COLOR_SENSOR)
#include "../examples/color_sensor/color_sensor.ino"
#elif defined(EXAMPLE_NORMALIZED_COLOR)
#include "../examples/normalized_color/normalized_color.ino"
#endif

#endif
