#pragma once

#include <cstdint>
#include <vector>


struct Animation {
    const char* name;
    const uint8_t num_layers;
    const uint16_t num_frames;

    const uint8_t* image_data;
    uint16_t image_width;  // in px
    uint16_t image_height;  // in px

    struct Frame {
        struct Layer {
            // location of layer in image_data, all in px
            uint16_t x;
            uint16_t y;
            uint16_t width;
            uint16_t height;

            // extra px to skip into dest when blitting to properly align for empty space
            uint16_t dest_offset_x;
            uint16_t dest_offset_y;
        };
        const std::vector<Layer> layers;
        uint16_t duration_ms;
    };

    const Frame* frames;
};
