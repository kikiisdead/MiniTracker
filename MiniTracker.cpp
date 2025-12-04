/**
 * @file MiniTracker.cpp
 * @author Kiyoko Iuchi-Fung
 * @brief 
 * 
 * LIBRARIES USED
 *  - DadGFX
 *  - libDaisy
 *  - DaisySP
 * 
 * TODO
 * FatFS is still fucked and I don't know if its a problem on my end or not
 * Add step FX and shit
 *  - PITCH UP
 *  - PITCH DOWN
 *  - FX AUTOMATION
 * Implement custom allocator before I die of memory fragmentation
 * Check power draw  
 * Figure out sleep mode
 * 
 * KNOWN BUGS / ISSUES
 *  - Noise from TFT and from SD card when loading / reading (hardware issue)
 *  - The position of the 5th slice disappears
 *  - Incorrectly reads 32 bit float wave files
 *  - Worried about memory fragmentation from EventQueue and patterns
 *     - Implement custom allocator?
 *  - Get NullPointerException when trying to access non-existent lane on FX screen
 * 
 * CHANGES TO LIBRARY
 * SD_TIMEOUT in sd_diskio.c changed to 1000 ticks instead of 30 * 1000 ticks
 * 
 * @version 0.1
 * @date 2025-10-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "daisy_seed.h"
#include "daisysp.h"
#include "fatfs.h"
#include "src/UI/sequencer.h"
#include "src/UI/button.h"
#include "src/audio/sampleplayer.h"
#include "src/UI/instDisplay.h"
#include "src/UI/fxDisplay.h"
#include "src/UI/sampDisplay.h"
#include "src/UI/songDisplay.h"
#include <vector>
#include <queue>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stm32h7xx_hal.h>
#include <stm32h750xx.h>
#include "DaisySeedGFX2/cDisplay.h"
#include "src/UI/util/KodeMono_Regular8pt7b.h"
#include "src/sd/waveFileLoader.h"
#include "src/sd/dirLoader.h"
#include "src/sd/projSaverLoader.h"
#include "src/globals.h"
#include "src/UI/command.h"
#include "UserDefines.h"

using namespace daisy;
using namespace daisysp;
using namespace DadGFX;

// using this because issue with creating FIL objects on stack
#define DSY_TEXT __attribute__((section(".text")))

DaisySeed hw;

SdmmcHandler            sd;
DSY_TEXT FatFSInterface fsi;
DSY_TEXT FIL            readFIL; /* passing by pointer to loading functions */
DSY_TEXT FIL            writeFIL; /* passing by pointer to saving functions */

DECLARE_DISPLAY(__Display)
DECLARE_LAYER(MainLayer, 320, 240)
DadGFX::cFont MainFont(&KodeMono_Regular8pt7b);

/**
 * Creating important memory vectors globally
 * Some worries about memory fragmentation so maybe use
 * lined lists instead
 */

std::vector<InstrumentHandler*> handler;
std::vector<Instrument*> instruments;
std::vector<Pattern*> patterns; // holds all the possible patterns made
std::vector<int> songOrder; // holds the order of the patterns

bool projSafe = true;

/**
 * SDRAM STUFF
 * Make sure to not allocate the entire SDRAM because TFT relies on SDRAM for frame buffer
 */
#define CUSTOM_POOL_SIZE (48*1024*1024) // 48MB

DSY_SDRAM_BSS char sample_buffer[CUSTOM_POOL_SIZE];

size_t buffer_index = 0;
int allocation_count = 0;

/**
 * Allocates space inside of SDRAM
 * @param size in bytes to allocate
 * @return pointer to allocation start in buffer
 */
void* sample_buffer_allocate(size_t size)
{
  if (buffer_index + size >= CUSTOM_POOL_SIZE)
  {
    return 0;
  }
  void* ptr = &sample_buffer[buffer_index];
  buffer_index += size;
  return ptr;
}

/**
 * Deallocates space inside SDRAM
 * moves everything to prevent fragmentation
 * @param start the starting memory position
 * @param size in bytes to deallocate
 */
void sample_buffer_deallocate(void* start, size_t size) {

  // getting an int value offset from the beginning of the buffer
  std::uintptr_t begin = reinterpret_cast<uintptr_t>(start) - reinterpret_cast<uintptr_t>(&sample_buffer[0]);

  // moving and overwriting
  memmove(start, static_cast<uint8_t*>(start) + size, buffer_index - (begin + size)); 

  // udpating the write position 
  buffer_index -= size; 

}

/**
 * Clears the entire sample buffer by reseting buffer_index
 * erasing by ommision (buffer does not need to be 0)
 */
bool sample_buffer_clear() {
  buffer_index = 0;
  return true;
}

#define FILE_BUFFER_SIZE (1024*1024) // 1MB

DSY_SDRAM_BSS char file_buffer[FILE_BUFFER_SIZE];

size_t file_index = 0;

// allocating space in buffer
void* file_buffer_allocate(size_t size)
{
  if (file_index + size >= FILE_BUFFER_SIZE)
  {
    return 0;
  }
  void* ptr = &file_buffer[file_index];
  file_index += size;
  return ptr;
}

bool file_buffer_clear() {
  memset(file_buffer, 0, FILE_BUFFER_SIZE);
  file_index = 0;
  return true;
}

/**
 * SD CARD STUFF
 */
WaveFileLoader waveFileLoader;

ProjSaverLoader projSaverLoader;

DirLoader sampleDirLoader;

DirLoader projDirLoader;

/**
 * USER INTERFACES
 */
buttonInterface* userInterface;

Sequencer sequencer;

InstrumentDisplay instDisplay;

FXDisplay fxDisplay;

SampDisplay sampDisplay;

SongDisplay songDisplay;


volatile uint8_t StandbyActivate = 0;
volatile uint8_t EnterStandbyFlag = 0; /**< Flag to engage standby, will need to turn breakouts off as well mainly  */
volatile uint8_t LeaveStopFlag = 0;

/**
 * STANDBY MODE SETUP
 * 
 */
// WAKE UP PIN IS PC1
void Enter_Standby() {
  EnterStandbyFlag = 1;
}

/**
 * BUTTONS
 * Callbacks
 * Objects
 * and Vectors
*/
std::queue<Command*> eventQueue;

void APress() {
  eventQueue.push(new ACommand);
}
void BPress() {
  eventQueue.push(new BCommand);
}
void PlayPress() {
  eventQueue.push(new PlayCommand);
}
void UpPress() {
  eventQueue.push(new UpCommand);
}
void DownPress() {
  eventQueue.push(new DownCommand);
}
void LeftPress() {
  eventQueue.push(new LeftCommand);
}
void RightPress() {
  eventQueue.push(new RightCommand);
}
void LeftShoulderPress() {
  eventQueue.push(new LeftShoulderCommand);
}
void RightShoulderPress() {
  eventQueue.push(new RightShoulderCommand);
}
void BothShoulderPress() {
  EXTI_HandleTypeDef EXTI_Handle;
  EXTI_Handle.Line = 0;
  EXTI_Handle.PendingCallback = Enter_Standby;
  HAL_EXTI_IRQHandler(&EXTI_Handle);
}

Button A;
Button B;
Switch Shift;
Button Play;
Button Up;
Button Down;
Button Left;
Button Right;
Button LeftShoulder;
Button RightShoulder;

Button Standby;

GPIO backlight;
GPIO ampSD;
GPIO headphoneDet;

static bool shift;

static float samplerate;

std::vector<Button*> buttons;

/**
 * Handles update of all buttons
 * button callbacks push a new command object on the queue
 */
void InputHandle() {  
  shift = Shift.RawState();
  for (uint16_t i = 0; i < buttons.size(); i++) {
    buttons.at(i)->Update();
  }
}

Limiter limiter;

void CLEAR() {
  sample_buffer_clear();
  sequencer.Clear();

  for (Instrument* inst : instruments) {
    delete inst;
  }

  instruments.clear();

  for (Pattern* patt : patterns) {
    delete patt;
  } 

  patterns.clear();
  
  songOrder.clear();

  for (InstrumentHandler* hand : handler) {
    hand->ClearFX();
  }
  projSafe = false;
}

void SAFE() {
  projSafe = true;
  sequencer.Safe();
}

/**
 * Initializing Seed Audio
 */
void SEED_AUDIO_INIT() {
  hw.SetAudioBlockSize(128); // number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  samplerate = hw.AudioSampleRate();
}

/**
 * Initialize SDIO & waveFileLoader
 */
void SDIO_INIT() {
  
  SdmmcHandler::Config sd_cfg;
  sd_cfg.clock_powersave = true; // turn clock off when not in use to save power
  sd_cfg.speed = SdmmcHandler::Speed::VERY_FAST; // very fast because wav files are big
  sd_cfg.width = SdmmcHandler::BusWidth::BITS_4; // same here
  
  sd.Init(sd_cfg);

  // FatFS Interface
  fsi.Init(FatFSInterface::Config::MEDIA_SD);

  // Mount SD Card
  if (f_mount(&fsi.GetSDFileSystem(), "/", 1) == FR_OK) {
    sampleDirLoader.Init(file_buffer_allocate, "/Samples"); // building separate trees; this one loads sample file info
    projDirLoader.Init(file_buffer_allocate, "/Projects"); // building separate trees; this one loads project file info
    waveFileLoader.Init(samplerate, sample_buffer_allocate, &readFIL);
    projSaverLoader.Init("/Projects/", &readFIL, &writeFIL, &sequencer, &instruments, &handler, CLEAR, SAFE, &waveFileLoader);
  }
}

/**
 * Initialize Buttons
 */
void BUTTON_INIT() {
  /**
   * Initializing Buttons
   * sending callbacks and adding to buttons vector
   */
  A.Init(hw.GetPin(APin), samplerate, false);
  A.CallbackHandler(APress);
  buttons.push_back(&A);

  B.Init(hw.GetPin(BPin), samplerate, false);
  B.CallbackHandler(BPress);
  buttons.push_back(&B);

  Play.Init(hw.GetPin(PlayPin), samplerate, false);
  Play.CallbackHandler(PlayPress);
  buttons.push_back(&Play);

  Shift.Init(hw.GetPin(ShiftPin), samplerate);

  Left.Init(hw.GetPin(LeftPin), samplerate, true);
  Left.CallbackHandler(LeftPress);
  buttons.push_back(&Left);

  Right.Init(hw.GetPin(RightPin), samplerate, true);
  Right.CallbackHandler(RightPress);
  buttons.push_back(&Right);

  Up.Init(hw.GetPin(UpPin), samplerate, true);
  Up.CallbackHandler(UpPress);
  buttons.push_back(&Up);

  Down.Init(hw.GetPin(DownPin), samplerate, true);
  Down.CallbackHandler(DownPress);
  buttons.push_back(&Down);

  LeftShoulder.Init(hw.GetPin(LeftShoulderPin), samplerate, false);
  LeftShoulder.CallbackHandler(LeftShoulderPress);
  buttons.push_back(&LeftShoulder);

  RightShoulder.Init(hw.GetPin(RightShoulderPin), samplerate, false);
  RightShoulder.CallbackHandler(RightShoulderPress);
  buttons.push_back(&RightShoulder);

  Standby.Init(hw.GetPin(StandbyPin), samplerate, false, Switch::PULL_NONE);
  Standby.CallbackHandler(Enter_Standby);
  buttons.push_back(&Standby);

  GPIO_InitTypeDef GPIO_Init;
  GPIO_Init.Pin = GPIO_PIN_7;
  GPIO_Init.Mode = GPIO_MODE_INPUT;
  GPIO_Init.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_Init);
}

/** LOOP */
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
  float outL = 0;
  float outR = 0;
  
  if (!projSafe) return; // cutsoff audio processing during project load

  InputHandle(); 

  for (size_t i = 0; i < size; i++)
  {
    outL = 0;
    outR = 0;

    sequencer.Update();

    for (InstrumentHandler* hand : handler) {
      float tempL = 0.0f;
      float tempR = 0.0f;
      hand->Process(tempL, tempR);
      outL += tempL * 0.5f;
      outR += tempR * 0.5f;
    }

    songDisplay.Process(outL, outR);

    out[0][i] = outL;
    out[1][i] = outR;
  }

  limiter.ProcessBlock(out[0], size, 1.0f);
  limiter.ProcessBlock(out[1], size, 1.0f);
}

int main(void) {

  hw.Init();

  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

  INIT_DISPLAY(__Display);
  __Display.setOrientation(Rotation::Degre_90);
  DadGFX::cLayer* pMain = ADD_LAYER(MainLayer, 0, 0, 1);
  DadGFX::cFont MainFont(&KodeMono_Regular8pt7b);
  backlight.Init(seed::D7, GPIO::Mode::OUTPUT);
  ampSD.Init(AmpSDPin, GPIO::Mode::OUTPUT);
  ampSD.Write(false);
  
  int xOffset = 160 - ((11 * CHAR_WIDTH) / 2);
  int yOffset = 120 - (CHAR_HEIGHT / 2);
  pMain->eraseLayer(BACKGROUND);
  pMain->setFont(&MainFont);
  pMain->setCursor(xOffset, yOffset);
  pMain->setTextFrontColor(ACCENT1);
  char strbuff[20];
  sprintf(strbuff, "Mini");
  pMain->drawText(strbuff);
  pMain->setTextFrontColor(ACCENT2);
  pMain->setCursor(xOffset + (CHAR_WIDTH * 4), yOffset);
  sprintf(strbuff, "Tracker");
  pMain->drawText(strbuff);
  __Display.flush();
  backlight.Write(true);
  System::Delay(150); // wait for it to finish sending

  SEED_AUDIO_INIT();
  ampSD.Write(true);

  SDIO_INIT();

  BUTTON_INIT();

  /**
   * Building instrument handlers
   */
  for (int i = 0; i < LANE_NUMBER; i ++) {
    handler.push_back(new InstrumentHandler);
    handler[i]->Init(&instruments, samplerate, &MainFont);
  }

  /**
   * Initializing UI objects
   * and adding to interfaces vector
   */
  limiter.Init();
  sequencer.Init(&handler, samplerate, &MainFont, &patterns, &songOrder);
  instDisplay.Init(&instruments, handler.at(0), &MainFont);
  fxDisplay.Init(samplerate, &handler, &MainFont);
  sampDisplay.Init(&waveFileLoader, &instruments, &MainFont, &buffer_index, sampleDirLoader.GetRootNode(), sample_buffer_deallocate);
  songDisplay.Init(&sequencer, projDirLoader.GetRootNode(), &projSaverLoader, &MainFont, file_buffer_allocate);

  songDisplay.SetPrev(nullptr);
  songDisplay.SetNext(&sampDisplay);
  sampDisplay.SetPrev(&songDisplay);
  sampDisplay.SetNext(&sequencer);
  sequencer.SetPrev(&sampDisplay);
  sequencer.SetNext(&instDisplay);
  instDisplay.SetPrev(&sequencer);
  instDisplay.SetNext(&fxDisplay);
  fxDisplay.SetPrev(&instDisplay);
  fxDisplay.SetNext(nullptr);
  
  userInterface = &sequencer;

  hw.StartAudio(AudioCallback);

  uint32_t timeSinceEvent = System::GetNow();

  for (;;) {
    userInterface->UpdateDisplay(pMain); 
    GPIO_PinState detState = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
    if (detState == GPIO_PIN_SET) {
      pMain->drawFillRect(0, 0, 100, 100, MAIN);
    }
    __Display.flush();

    while (eventQueue.size() != 0) {
      timeSinceEvent = System::GetNow();
      Command* command = eventQueue.front(); 
      command->Execute(userInterface, shift);
      eventQueue.pop();
      delete command; // pointer is deleted at end of loop
    }

    
    if (System::GetNow() - timeSinceEvent >= (uint32_t) SLEEP_TIME) {
      Enter_Standby();
    }

    if(EnterStandbyFlag)
    {
      // Enter Standby Mode!
      EnterStandbyFlag = 0;
      System::Delay(50); // debounce off button bc no debounce in standby state
      // 1) Clear The Wakeup Flag
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
      // 2) Enable The WKUP1 Pin
      HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
      // 3) Enter The Standby Mode
      HAL_PWR_EnterSTANDBYMode();
    } 
  }
}

