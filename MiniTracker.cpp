#include "daisy_seed.h"
#include "daisysp.h"
#include "fatfs.h"
#include "src/UI/sequencer.h"
#include "src/UI/display.h"
#include "src/UI/button.h"
#include "dev/oled_ssd130x.h"
#include "src/samples/cowbell.h"
#include "src/audio/sampleplayer.h"
#include <vector>

#define UpPin 18
#define DownPin 17
#define LeftPin 16
#define RightPin 15
#define APin 19
#define BPin 20
#define ShiftPin 21
#define PlayPin 22
#define LeftShoulderPin 23
#define RightShoulderPin 24

using namespace daisy;
using namespace daisysp;

using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

DaisySeed hw;

SdmmcHandler   sd;
FatFSInterface fsi;
FIL            SDFile;

MyOledDisplay display;

Sequencer sequencer;

std::vector<InstrumentHandler*> handler;

std::vector<Instrument*> instruments;

bool shift;


// Will use as a buffer for samples
// 
#define CUSTOM_POOL_SIZE (48*1024*1024)

DSY_SDRAM_BSS float sample_buffer[CUSTOM_POOL_SIZE];

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




buttonInterface* control;

/** Button Press Callback functions (needed to be static and change as control pointer changed) */
void APress() {
  if (shift) control->AltAButton();
  else control->AButton();
}
void BPress() {
  if (shift) control->AltBButton();
  else control->BButton();
}
void PlayPress() {
  if (shift) control->AltPlayButton();
  else control->PlayButton();
}
void UpPress() {
  if (shift) control->AltUpButton();
  else control->UpButton();
}
void DownPress() {
  if (shift) control->AltDownButton();
  else control->DownButton();
}
void LeftPress() {
  if (shift) control->AltLeftButton();
  else control->LeftButton();
}
void RightPress() {
  if (shift) control->AltRightButton();
  else control->RightButton();
}

Button A;
Button B;
Switch Shift;
Button Play;
Button Up;
Button Down;
Button Left;
Button Right;
// Switch LeftShoulder;
// Switch RightShoulder;

std::vector<Button*> buttons;

void InputHandle() {  
  shift = Shift.RawState();
  for (uint16_t i = 0; i < buttons.size(); i++) {
    buttons.at(i)->Update();
  }
}

Metro tick;

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

    // if (tick.Process()) {
    //   display.Update();
    // }
    
    for (InstrumentHandler* hand : handler) {
      outBuff += hand->Process() * 0.2;
    }

    out[0][i] = outBuff;
    out[1][i] = outBuff;
  }
}

/** SETUP */
int main(void) 
{
  hw.Init();
  
  // #if SCREEN_ON
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

  // #endif

  // // Init SD Card
  // SdmmcHandler::Config sd_cfg;
  // sd_cfg.Defaults();
  // sd.Init(sd_cfg);

  // // FatFS Interface
  // fsi.Init(FatFSInterface::Config::MEDIA_SD);

  // // Mount SD Card
  // f_mount(&fsi.GetSDFileSystem(), "/", 1);

  hw.SetAudioBlockSize(8); // number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  float samplerate = hw.AudioSampleRate();

  for (int i = 0; i < 4; i++) {
    instruments.push_back(new Instrument);
    instruments.back()->Init(samplerate);
  }

  for (int i = 0; i < 4; i ++) {
    handler.push_back(new InstrumentHandler);
    handler[i]->Init(&instruments);
  }


  sequencer.Init(&display, handler, samplerate);
  
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

  // LeftShoulder.Init(hw.GetPin(LeftShoulderPin), samplerate);
  // RightShoulder.Init(hw.GetPin(RightShoulderPin), samplerate);

  control = &sequencer;

  tick.Init(0.5, samplerate);

  hw.StartAudio(AudioCallback);
  
  while(1) {}
}
