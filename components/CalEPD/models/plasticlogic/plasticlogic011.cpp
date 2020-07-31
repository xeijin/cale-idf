#include "plasticlogic011.h"
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/task.h"

// Constructor
PlasticLogic011::PlasticLogic011(EpdSpi2Cs& dio): 
  Adafruit_GFX(PLOGIC011_WIDTH, PLOGIC011_HEIGHT),
  PlasticLogic(PLOGIC011_WIDTH, PLOGIC011_HEIGHT), IO(dio)
{
  printf("PlasticLogic011() %d*%d\n",
  PLOGIC011_WIDTH, PLOGIC011_HEIGHT);  
}

//Initialize the display
void PlasticLogic011::init(bool debug)
{
    debug_enabled = debug;
    if (debug_enabled) {
      printf("PlasticLogic011::init(%d) bufferSize: %d width: %d height: %d\n", 
    debug, PLOGIC011_BUFFER_SIZE, PLOGIC011_WIDTH, PLOGIC011_HEIGHT);
    }
    IO.init(4, debug); // 4MHz frequency
    
    if (CONFIG_EINK_RST > -1) {
      IO.reset(5);
    } else {
      IO.cmd(EPD_SOFTWARERESET);
    }
    
    uint8_t size = getEPDsize();
    printf("EPD size ID: %d\n", size);

    if (size != 11) {
      ESP_LOGE(TAG, "ATTENTION the size responded by the display: %d does not mach this class", size);
    }

    _wakeUp();

    //printf("Epaper temperature after wakeUp: %d °C\n", IO.readTemp());
    //Set landscape mode as default
    setEpdRotation(1);
}


uint8_t PlasticLogic011::getEPDsize() {
  int8_t response = 0;
  uint8_t programMtp[2] = {EPD_PROGRAMMTP, 0x02};
  uint8_t setMtpAddress[3] = {EPD_MTPADDRESSSETTING, 0xF2, 0x04};
  
  IO.data(programMtp, sizeof(programMtp));
  _waitBusy("programMtp");
  IO.data(setMtpAddress, sizeof(setMtpAddress));
  _waitBusy("setMtpAddress");

  uint8_t reg = 0x43|EPD_REGREAD;
  uint8_t regRead[2] = {reg, 0xFF};

  // Read 1 dummy bytes
  IO.readRegister(regRead,sizeof(regRead));
  response = IO.readRegister(regRead,sizeof(regRead));

  ESP_LOGI(TAG, "MISO responded with: %d", response);

  switch (response)
  {
  case 49:
    response = IO.readRegister(regRead,sizeof(regRead));
    
    if (response==49)    {
      return 11; // 1.1" detected
    } else {
      return 14; // 1.4" detected
    }        
    break;

  case 50:
    return 21;
    break;
  case 51:
    return 31;
    break;
  default:
    response = -1;
    break;
  }
  return response;
}

uint8_t PlasticLogic011::readTemperature()
{
  return IO.readTemp();
}

std::string PlasticLogic011::readTemperatureString(uint8_t type) {
  uint8_t temperature = readTemperature();
  std::string temp = std::to_string(temperature);
  switch (type)
  {
  case 1:
    temp = temp + "°C";
    break;
  }
  return temp;
}

/**
 * Sets internal EPD into portrait or Landscape mode
 * Not sure if it will be a public method at the end of integration
 */
void PlasticLogic011::setEpdRotation(uint8_t o) {
   uint8_t settingDataEntryPortrait[2] = {EPD_DATENTRYMODE, 0x07};
   uint8_t settingDataEntryLandscape[2] = {EPD_DATENTRYMODE, 0x20};

   switch (o)
   {
   case 1:
     /* Portrait */
     IO.data(settingDataEntryPortrait, sizeof(settingDataEntryPortrait));
     break;
   
   case 2:
     /* Landscape mode */
     IO.data(settingDataEntryLandscape, sizeof(settingDataEntryLandscape));
     break;
   }
}

void PlasticLogic011::clearScreen(){
  for (uint16_t x = 0; x < sizeof(_buffer); x++)
  {
    _buffer[x] = 0xff;
  }
}

void PlasticLogic011::_wakeUp(){
    uint8_t panelSetting[2] = {EPD_PANELSETTING, 0x12};
    IO.data(panelSetting, sizeof(panelSetting));
    _waitBusy("Panel setting");

    uint8_t settingWriteRectangular[5] = {EPD_WRITEPXRECTSET, 0x00, 0x47, 0x00, 0x93};
    IO.data(settingWriteRectangular, sizeof(settingWriteRectangular));
    _waitBusy("settingWriteRectangular");

    uint8_t settingVcom[5] = {EPD_VCOMCONFIG, 0x0, 0x0, 0x24, 0x07};
    IO.data(settingVcom, sizeof(settingVcom));
    _waitBusy("VCOM configuration");

    uint8_t settingDriverVoltage[3] = {EPD_DRIVERVOLTAGE, 0x25, 0xff};
    IO.data(settingDriverVoltage, sizeof(settingDriverVoltage));
    _waitBusy("Driver voltage setting");

    uint8_t settingVborder[2] = {EPD_BORDERSETTING, 0x04};
    IO.data(settingVborder, sizeof(settingVborder));
    _waitBusy("Vborder setting");

    uint8_t settingLoadMono[2] = {EPD_LOADMONOWF, 0x60};
    IO.data(settingLoadMono, sizeof(settingLoadMono));
    _waitBusy("Load mono");

    uint8_t settingTemperature[2] = {EPD_INTTEMPERATURE, 0x0a};
    IO.data(settingTemperature, sizeof(settingTemperature));
    _waitBusy("Temperature");

    uint8_t settingBoost[3] = {EPD_BOOSTSETTING, 0x22, 0x17};
    IO.data(settingBoost, sizeof(settingBoost));
    _waitBusy("Boost setting");
}

void PlasticLogic011::update(uint8_t updateMode)
{
  ESP_LOGD(TAG, "Sending %d bytes buffer", sizeof(_buffer));

  uint8_t pixelAccessPos[3] = {EPD_PIXELACESSPOS, 0x00, 0x93};
  uint8_t programMtp[2] = {EPD_PROGRAMMTP, 0x00};
  uint8_t displayEngine[2] = {EPD_DISPLAYENGINE, 0x03};

  IO.data(pixelAccessPos, sizeof(pixelAccessPos));

  bufferEpd[0] = 0x10;
  //_buffer[0] = 0x10;
  // Copy GFX buffer contents:
  for (int i=1; i < sizeof(bufferEpd); i++) {
    bufferEpd[i] = _buffer[i-1];
  }

  IO.data(bufferEpd,sizeof(bufferEpd));
  
  _waitBusy("Buffer sent", EPD_TMG_SRT);
  
  _powerOn();

    switch (updateMode) {
        case 0:
          IO.data(programMtp, sizeof(programMtp));
          IO.data(displayEngine, sizeof(displayEngine));
          _waitBusy("EPD_UPD_FULL", EPD_TMG_LNG);

          break;
        case 1:
          displayEngine[1] = 0x07;

          IO.data(programMtp, sizeof(programMtp));
          IO.data(displayEngine, sizeof(displayEngine));
          _waitBusy("EPD_UPD_FULL", EPD_TMG_LNG);
            break;
        case 2:
          programMtp[1] = 0x02;
          displayEngine[1] = 0x07;

          IO.data(programMtp, sizeof(programMtp));
          IO.data(displayEngine, sizeof(displayEngine));
          _waitBusy("EPD_UPD_FULL", EPD_TMG_LNG);
          break;
    }
    
  _powerOff();
}

void PlasticLogic011::_powerOn(void) {

  uint8_t setResolution[5]   = {EPD_SETRESOLUTION, 0x00, 0xEF, 0x00, 0x93};
  uint8_t setTcomTiming[3]   = {EPD_TCOMTIMING   , 0x67, 0x65};
  uint8_t setPowerSeq[4]     = {EPD_POWERSEQUENCE, 0x00, 0x00, 0x00};
  uint8_t setPowerControl[2] = {EPD_POWERCONTROL , 0xD1};

  IO.data(setResolution, sizeof(setResolution));
  _waitBusy("Panel resolution");

  IO.data(setTcomTiming, sizeof(setTcomTiming));
  _waitBusy("TCOM Timing");
  
  IO.data(setPowerSeq, sizeof(setPowerSeq));
  _waitBusy("Power sequence");

  IO.data(setPowerControl, sizeof(setPowerControl));
  _waitBusy("setPowerControl");
  // 70 works
  vTaskDelay(70/portTICK_RATE_MS);       // Only because reading the value below is not working
  //while (IO.readRegister(0x15) == 0) {}   // Wait until Internal Pump is ready 
}

void PlasticLogic011::_waitBusy(const char* message, uint16_t busy_time){
  if (debug_enabled) {
    ESP_LOGI(TAG, "_waitBusy for %s", message);
  }
  int64_t time_since_boot = esp_timer_get_time();
  // On high is busy
  if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 1) {
  while (1){
    if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 1) break;
    vTaskDelay(1);
    if (esp_timer_get_time()-time_since_boot>7000000)
    {
      if (debug_enabled) ESP_LOGI(TAG, "Busy Timeout");
      break;
    }
  }
  } else {
    vTaskDelay(busy_time/portTICK_RATE_MS); 
  }
}

void PlasticLogic011::_waitBusy(const char* message){
  if (debug_enabled) {
    ESP_LOGI(TAG, "_waitBusy for %s", message);
  }
  int64_t time_since_boot = esp_timer_get_time();

  while (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 0){
    vTaskDelay(1/portTICK_RATE_MS); 

    if (esp_timer_get_time()-time_since_boot>500000)
    {
      if (debug_enabled) ESP_LOGI(TAG, "Busy Timeout");
      break;
    }
  }
}

// Called _poweroff in microEPD
void PlasticLogic011::_powerOff(){
  uint8_t setPowerControl[2] = {EPD_POWERCONTROL , 0xD0};
  IO.data(setPowerControl, sizeof(setPowerControl)); 
  _waitBusy("power_off");
  
  setPowerControl[1] = 0xC0;
  IO.data(setPowerControl, sizeof(setPowerControl));
  _waitBusy("setPowerControl");
}

/**
 * Putting the UC8156 in deep sleep mode with less than 1µA current @3.3V
 * Reset pin toggling needed to wakeup the driver IC again.
 */ 
void PlasticLogic011::_sleep(){
  printf("Deepsleep called. In order to wake up from sleep mode this display needs to be initialized again calling init() method\n");
  uint8_t deepsleep[5] = {0x21 , 0xff, 0xff, 0xff, 0xff};
  IO.data(deepsleep, sizeof(deepsleep));
}

void PlasticLogic011::_rotate(uint16_t& x, uint16_t& y, uint16_t& w, uint16_t& h)
{
  switch (getRotation())
  {
    case 1:
      swap(x, y);
      swap(w, h);
      x = PLOGIC011_WIDTH - x - w - 1;
      break;
    case 2:
      x = PLOGIC011_WIDTH - x - w - 1;
      y = PLOGIC011_HEIGHT - y - h - 1;
      break;
    case 3:
      swap(x, y);
      swap(w, h);
      y = PLOGIC011_HEIGHT - y - h - 1;
      break;
  }
}


void PlasticLogic011::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

  // check rotation, move pixel around if necessary
  switch (getRotation())
  {
    case 1:
      swap(x, y);
      x = PLOGIC011_WIDTH - x - 1;
      break;
    case 2:
      x = PLOGIC011_WIDTH - x - 1;
      y = PLOGIC011_HEIGHT - y - 1;
      break;
    case 3:
      swap(x, y);
      y = PLOGIC011_HEIGHT - y - 1;
      break;
  }
  
  y=y+3;
  uint8_t pixels = _buffer[x/4 + (y) * _nextline];
	switch (x%4) {					            //2-bit grayscale dot
    	case 0: _buffer[x/4 + (y) * _nextline] = (pixels & 0x3F) | ((uint8_t)color << 6); break;	
    	case 1: _buffer[x/4 + (y) * _nextline] = (pixels & 0xCF) | ((uint8_t)color << 4); break;	
    	case 2: _buffer[x/4 + (y) * _nextline] = (pixels & 0xF3) | ((uint8_t)color << 2); break;	
    	case 3: _buffer[x/4 + (y) * _nextline] = (pixels & 0xFC) | (uint8_t)color; break;		
	}
}