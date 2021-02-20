#pragma once

#include <cstdint>
#include "hardware/spi.h"

enum SSD1306PixelColor {
    SSD1306_COLOR_OFF = 0,
    SSD1306_COLOR_ON = 1,
    SSD1306_COLOR_INVERTED = 2
};

/**
 * 4-pin SPI (+ reset) controller for SSD1306 OLED display, adapted from https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp
 */
class SSD1306 {

    public:
    /**
     * width: width of display
     * height: height of display
     * spi: pico spi instance
     * baudrate: spi baudrate in Hz
     * mosi_pin: spi tx pin / d0
     * cs_pin: spi chip select pin
     * sclk_pin: spi clock pin (slk)
     * rst_pin: reset pin
     * dc_pin: data/command pin
     */
    SSD1306(uint8_t width, uint8_t height, spi_inst_t* spi, uint32_t baudrate,
        uint8_t mosi_pin, uint8_t cs_pin, uint8_t sclk_pin, uint8_t rst_pin, uint8_t dc_pin);

    ~SSD1306();

    void init();

    void send_command(uint8_t command);
    void send_commands(const uint8_t* commands, size_t len);
    void update();

    void clear();
    void draw_pixel(int16_t x, int16_t y, SSD1306PixelColor color);

    inline uint8_t* buffer() { return buffer_; }
    inline uint8_t width() const { return width_; }
    inline uint8_t height() const { return height_; }
    inline uint16_t buffer_len() const {
        return width_ * ((height_ + 7) / 8);
    }

    private:
    void send_init_commands();

    uint8_t* buffer_;

    uint8_t width_;
    uint8_t height_;
    spi_inst_t* spi_;
    uint32_t baudrate_;

    uint8_t sclk_pin_;  // spi clock (d0)
    uint8_t mosi_pin_;  // spi tx / master out slave in (d1)
    uint8_t cs_pin_;  // chip select (keep low)
    uint8_t dc_pin_;  // data command pin (used to differentiate commands vs screen buffer)
    uint8_t rst_pin_;  // reset (keep high)
};
