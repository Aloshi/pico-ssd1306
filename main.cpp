#include <stdio.h>
#include <assert.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "math.h"

#include "ssd1306.h"

#include "data/blink.h"

/*int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}*/

inline void blit_animation_1bpp(uint8_t* dest, size_t dest_width, size_t dest_height, uint16_t dest_x, uint16_t dest_y,
                                const Animation& anim, uint16_t frame_num) {
    assert(frame_num < anim.num_frames);
    
    const Animation::Frame& frame = anim.frames[frame_num];
    for (uint8_t layer_idx = 0; layer_idx < anim.num_layers; layer_idx++) {
        const Animation::Frame::Layer& layer = frame.layers[layer_idx];

        // draw pixel: dest[x + (y / 8) * width_] |= (1 << (y & 7));
        for (uint16_t y = 0; y < layer.height; y++) {
            for (uint16_t x = 0; x < layer.width; x++) {
                uint16_t in_x = x + layer.x;
                uint16_t in_y = y + layer.y;
                uint16_t in_offset = (in_x / 8) + (in_y * (anim.image_width / 8));
                assert(in_offset < anim.image_width * anim.image_height);
                uint8_t val = anim.image_data[(in_x / 8) + (in_y * (anim.image_width / 8))] & (1 << (in_x & 7));

                uint16_t out_x = x + layer.dest_offset_x + dest_x;
                uint16_t out_y = y + layer.dest_offset_y + dest_y;
                assert(out_x < dest_width && out_y < dest_height);
                if (val) {
                    dest[out_x + (out_y / 8) * dest_width] |= (1 << (out_y & 7));
                }
            }
        }
    }
}


int main()
{
    stdio_init_all();

    // set onboard led high to show that we're on
    /*gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);*/

    SSD1306 display(128, 64, spi0, /*baudrate*/ 8000 * 1000, /*mosi*/ 3, /*cs*/ 5, /*sclk*/ 2, /*reset*/ 6, /*dc*/ 7);
    display.init();

    const Animation& anim = animation_blink;
    uint16_t frame = 0;
    while (true) {
        display.clear();
        blit_animation_1bpp(display.buffer(), display.width(), display.height(), 0, 8, anim, frame);
        display.update();
        sleep_ms(anim.frames[frame].duration_ms);
        frame = (frame + 1) % anim.num_frames;
    }

    while (true) {
        sleep_ms(1000);
    }
    
    /*while (true) {
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
    }*/

    return 0;
}
