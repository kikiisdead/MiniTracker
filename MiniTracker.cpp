#include "daisy_seed.h"
#include "daisysp.h"
#include "src/UI/sequencer.h"
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

MyOledDisplay display;

Sequencer sequencer;

InstrumentHandler* handler;

std::vector<Instrument*> instruments;

bool shift;

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

/** LOOP */
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) 
{
  float outBuff = 0;

  InputHandle();
  
  for (size_t i = 0; i < size; i++)
  {
    sequencer.Update();

    outBuff = 0;

    for (int j = 0; j < 4; j ++) {
      outBuff += handler[j].Process();
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

  hw.SetAudioBlockSize(8); // number of samples handled per callback
  // hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  
  float samplerate = hw.AudioSampleRate();

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

  for (int i = 0; i < 4; i ++) {
    handler[i] = InstrumentHandler();
    handler[i].Init(&instruments);
  }

  for (int i = 0; i < 4; i++) {
    instruments.push_back(new Instrument);
    instruments.back()->Init(samplerate);
  }
  
  control = &sequencer;

  hw.StartAudio(AudioCallback);
  
  while(1) {}
}
