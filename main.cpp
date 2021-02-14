#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "math.h"

#include "ssd1306.h"

/*int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}*/


int main()
{
    stdio_init_all();

    // set onboard led high to show that we're on
    /*gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);*/

    SSD1306 display(128, 64, spi0, /*baudrate*/ 8000 * 1000, /*mosi*/ 3, /*cs*/ 5, /*sclk*/ 2, /*reset*/ 6, /*dc*/ 7);
    display.init();
    
    while (true) {
        display.clear();

        float t = (float) (to_ms_since_boot(get_absolute_time())) / 1000.0f * 2.0f * M_PI;
        int16_t x = 64 + (int16_t) (cos(t) * 60);
        int16_t y = 48;
        //int16_t y = 45 + (int16_t) (sin(t) * 15);
    
        display.draw_pixel(x, y, SSD1306_COLOR_ON);
        display.draw_pixel(x-1, y, SSD1306_COLOR_ON);
        display.draw_pixel(x+1, y, SSD1306_COLOR_ON);
        display.draw_pixel(x, y-1, SSD1306_COLOR_ON);
        display.draw_pixel(x, y+1, SSD1306_COLOR_ON);

        display.update();

        sleep_ms(2);
    }

    return 0;
}
