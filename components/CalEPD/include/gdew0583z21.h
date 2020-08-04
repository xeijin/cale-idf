// 5.83 600*448 b/w/R Controller: IL0371 (3 colors) http://www.e-paper-display.com/download_detail/downloadsId%3d536.html
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include <stdint.h>
#include <math.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include <string>
#include <epd.h>
#include <Adafruit_GFX.h>
#include <epdspi.h>
#include "soc/rtc_wdt.h"
#include <gdew_colors.h>

#define GDEW0583Z21_WIDTH 600
#define GDEW0583Z21_HEIGHT 448

#define GDEW0583Z21_BUFFER_SIZE (uint32_t(GDEW0583Z21_WIDTH) * uint32_t(GDEW0583Z21_HEIGHT) / 8)

#define GDEW0583Z21_PU_DELAY 500

class Gdew0583z21 : public Epd
{
  public:
   
    Gdew0583z21(EpdSpi& IO);
    
    void init(bool debug);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillScreen(uint16_t color);
    void update();
    
    // This are already inherited from Epd: write(uint8_t); print(const std::string& text);println(same);

  private:
    EpdSpi& IO;

    uint8_t _buffer[GDEW0583Z21_BUFFER_SIZE];
    uint8_t _red_buffer[GDEW0583Z21_BUFFER_SIZE];
    bool _using_partial_mode = false;
    bool _initial = true;

    void _send8pixel(uint8_t data,uint8_t red);
    void PIC_display(const unsigned char* picData);
    
    void _wakeUp();
    void _sleep();
    void _waitBusy(const char* message);
    void _rotate(uint16_t& x, uint16_t& y, uint16_t& w, uint16_t& h);
    // Command & data structs
   
    static const epd_init_2 epd_wakeup_power;
    static const epd_init_2 epd_panel_setting;
    static const epd_init_4 epd_resolution;
};