#include "daisy_seed.h"
#include "daisysp.h"
#include "src/audio/decode.h"
#include "fatfs.h"
#include "src/UI/sequencer.h"
#include "src/UI/button.h"
#include "dev/oled_ssd130x.h"
#include "src/samples/cowbell.h"
#include "src/samples/collins.h"
#include "src/audio/sampleplayer.h"
#include "src/UI/instDisplay.h"
#include "src/UI/fxDisplay.h"
#include <vector>

#define LANE_NUMBER 4

#define UpPin 18
#define DownPin 17
#define LeftPin 16
#define RightPin 15
#define APin 19
#define BPin 20
#define ShiftPin 21
#define PlayPin 22
#define LeftShoulderPin 13
#define RightShoulderPin 14

#define USE_DAISYSP_LGPL 1

using namespace daisy;
using namespace daisysp;

using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

DaisySeed hw;

SdmmcHandler   sd;
FatFSInterface fsi;
FIL            SDFile;

MyOledDisplay display;

std::vector<InstrumentHandler*> handler;

std::vector<Instrument*> instruments;

bool shift;

WavFile sample;
WavFile sample2;


/**
 * SDRAM STUFF
 */
#define CUSTOM_POOL_SIZE (48*1024*1024)

DSY_SDRAM_BSS char sample_buffer[CUSTOM_POOL_SIZE];

size_t buffer_index = 0;
int allocation_count = 0;

// allocating space in buffer
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
 * USER INTERFACES
 */
std::vector<buttonInterface*> interfaces;

std::vector<buttonInterface*>::iterator userInterface;

Sequencer sequencer;

InstrumentDisplay instDisplay;

FXDisplay fxDisplay;


/**
 * BUTTONS
 * Callbacks
 * Objects
 * and Vectors
*/
void APress() {
  if (shift) (*userInterface)->AltAButton();
  else (*userInterface)->AButton();
}
void BPress() {
  if (shift) (*userInterface)->AltBButton();
  else (*userInterface)->BButton();
}
void PlayPress() {
  if (shift) (*userInterface)->AltPlayButton();
  else (*userInterface)->PlayButton();
}
void UpPress() {
  if (shift) (*userInterface)->AltUpButton();
  else (*userInterface)->UpButton();
}
void DownPress() {
  if (shift) (*userInterface)->AltDownButton();
  else (*userInterface)->DownButton();
}
void LeftPress() {
  if (shift) (*userInterface)->AltLeftButton();
  else (*userInterface)->LeftButton();
}
void RightPress() {
  if (shift) (*userInterface)->AltRightButton();
  else (*userInterface)->RightButton();
}
void LeftShoulderPress() {
  if (userInterface == interfaces.begin()) {
    userInterface = interfaces.end() - 1;
  } else {
    userInterface--;
  }
}
void RightShoulderPress() {
  if (userInterface != interfaces.end() - 1) {
    userInterface++;
  } else {
    userInterface = interfaces.begin();
  }
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

std::vector<Button*> buttons;

void InputHandle() {  
  shift = Shift.RawState();
  for (uint16_t i = 0; i < buttons.size(); i++) {
    buttons.at(i)->Update();
  }
}

/** LOOP */
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) 
{
  float outBuff = 0;
  
  InputHandle(); 

  // weird cutouts coming from updating display, seems as though updating the display in anyway pauses the whole program
  
  for (size_t i = 0; i < size; i++)
  {
    outBuff = 0;

    sequencer.Update();

    for (InstrumentHandler* hand : handler) {
      outBuff = hand->Process();
    }

    out[0][i] = outBuff;
    out[1][i] = outBuff;
  }
}

/** SETUP */
int main(void) 
{
  hw.Init();
  
  /** Configure the Display */
  MyOledDisplay::Config disp_cfg;
  
  disp_cfg.driver_config.transport_config.i2c_config.address = 0x3C;
  disp_cfg.driver_config.transport_config.i2c_config.periph = I2CHandle::Config::Peripheral::I2C_1;
  disp_cfg.driver_config.transport_config.i2c_config.speed  = I2CHandle::Config::Speed::I2C_1MHZ;
  disp_cfg.driver_config.transport_config.i2c_config.mode = I2CHandle::Config::Mode::I2C_MASTER;
  disp_cfg.driver_config.transport_config.i2c_config.pin_config.scl = hw.GetPin(11);
  disp_cfg.driver_config.transport_config.i2c_config.pin_config.sda = hw.GetPin(12);
  
  /** And Initialize */
  display.Init(disp_cfg);


  // // Init SD Card
  // SdmmcHandler::Config sd_cfg;
  // sd_cfg.Defaults();
  // sd.Init(sd_cfg);

  // // FatFS Interface
  // if (fsi.Init(FatFSInterface::Config::MEDIA_SD) == FR_OK) {
  //   // Mount SD Card
  //   f_mount(&fsi.GetSDFileSystem(), "/", 1);
  // }

  /**
   * Initializing Seed Audio
   */
  hw.SetAudioBlockSize(8); // number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);

  float samplerate = hw.AudioSampleRate();



  // move to sample loader eventually
  // load into sample buffer get pointer to beginning of sub array and size
  sample.format.AudioFormat = WAVE_FORMAT_ULAW;
  sample.format.BitPerSample = 16;
  sample.format.SampleRate = 44100;
  sample.format.NbrChannels = 1;
  sample.size = sizeof(collins);
  sample.start = sample_buffer_allocate(sample.size * sizeof(int16_t));
  
  sprintf(sample.name, "COLLINS");

  void* readPtr = &collins[0];
  
  void* writePtr = sample.start;

  size_t readIndex = 0;

  while (readIndex < sample.size) {
    int16_t val = MuLaw2Lin((*(static_cast<uint8_t*>(readPtr)) * -1) - 1); // get the sample and convert

    readPtr = static_cast<uint8_t*>(readPtr) + 1;
    readIndex += 1;
    *(static_cast<int16_t*>(writePtr)) = val;
    writePtr = static_cast<int16_t*>(writePtr) + 1;
  }

  sample2.format.AudioFormat = WAVE_FORMAT_ULAW;
  sample2.format.BitPerSample = 16;
  sample2.format.SampleRate = 44100;
  sample2.format.NbrChannels = 1;
  sample2.size = sizeof(cowbell);
  sample2.start = sample_buffer_allocate(sample2.size * sizeof(int16_t));
  
  sprintf(sample2.name, "COWBELL");

  readPtr = &cowbell[0];
  
  writePtr = sample2.start;

  readIndex = 0;

  while (readIndex < sample2.size) {
    int16_t val = MuLaw2Lin((*(static_cast<uint8_t*>(readPtr)) * -1) - 1); // get the sample and convert

    readPtr = static_cast<uint8_t*>(readPtr) + 1;
    readIndex += 1;
    *(static_cast<int16_t*>(writePtr)) = val;
    writePtr = static_cast<int16_t*>(writePtr) + 1;
  }


  instruments.push_back(new Instrument);
  instruments.back()->Init(samplerate, &sample);

  instruments.push_back(new Instrument);
  instruments.back()->Init(samplerate, &sample2);

  // for (int i = 0; i < LANE_NUMBER; i ++) {
  //   effects.push_back(new std::vector<EffectHandler*>);
  //   effects.back()->push_back(new EffectHandler);
  //   effects.back()->back()->Init(samplerate);
  // }

  for (int i = 0; i < LANE_NUMBER; i ++) {
    handler.push_back(new InstrumentHandler);
    handler[i]->Init(&instruments, samplerate);
  }

  /**
   * Initializing Buttons
   * sending callbacks and adding to buttons vector
   */
  A.Init(hw.GetPin(APin), samplerate);
  A.CallbackHandler(APress);
  buttons.push_back(&A);

  B.Init(hw.GetPin(BPin), samplerate);
  B.CallbackHandler(BPress);
  buttons.push_back(&B);

  Play.Init(hw.GetPin(PlayPin), samplerate);
  Play.CallbackHandler(PlayPress);
  buttons.push_back(&Play);

  Shift.Init(hw.GetPin(ShiftPin), samplerate);

  Left.Init(hw.GetPin(LeftPin), samplerate);
  Left.CallbackHandler(LeftPress);
  buttons.push_back(&Left);

  Right.Init(hw.GetPin(RightPin), samplerate);
  Right.CallbackHandler(RightPress);
  buttons.push_back(&Right);

  Up.Init(hw.GetPin(UpPin), samplerate);
  Up.CallbackHandler(UpPress);
  buttons.push_back(&Up);

  Down.Init(hw.GetPin(DownPin), samplerate);
  Down.CallbackHandler(DownPress);
  buttons.push_back(&Down);

  LeftShoulder.Init(hw.GetPin(LeftShoulderPin), samplerate);
  LeftShoulder.CallbackHandler(LeftShoulderPress);
  buttons.push_back(&LeftShoulder);

  RightShoulder.Init(hw.GetPin(RightShoulderPin), samplerate);
  RightShoulder.CallbackHandler(RightShoulderPress);
  buttons.push_back(&RightShoulder);

  /**
   * Initializing UI objects
   * and adding to interfaces vector
   */
  sequencer.Init(handler, samplerate);
  instDisplay.Init(instruments, handler.at(0));
  fxDisplay.Init(samplerate, handler);

  interfaces.push_back(&sequencer);
  interfaces.push_back(&instDisplay);
  interfaces.push_back(&fxDisplay);

  userInterface = interfaces.begin();

  hw.StartAudio(AudioCallback);

  for (;;) {
    (*userInterface)->UpdateDisplay(display);
    display.Update();
    System::Delay(10);
  }
}
