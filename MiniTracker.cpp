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
#include "DaisySeedGFX2/cDisplay.h"
#include "src/UI/util/KodeMono_Regular8pt7b.h"
#include "src/sd/waveFileLoader.h"
#include "src/sd/dirLoader.h"
#include "src/sd/projSaverLoader.h"
// #include "src/displayDriver/ST7789Driver.h"

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
using namespace DadGFX;

// using this because issue with creating FIL objects on stack
#define DSY_TEXT __attribute__((section(".text")))

DaisySeed hw;

SdmmcHandler   sd;

DSY_TEXT FatFSInterface fsi;
DSY_TEXT FIL            readFIL;
DSY_TEXT FIL            writeFIL;

DECLARE_DISPLAY(__Display)
DECLARE_LAYER(MainLayer, 320, 240)
DadGFX::cFont MainFont(&KodeMono_Regular8pt7b);

std::vector<InstrumentHandler*> handler;

std::vector<Instrument*> instruments;

std::vector<Pattern*> patterns; // holds all the possible patterns made

std::vector<int> songOrder; // holds the order of the patterns

float bpm = 178.0f;

bool shift;

bool projSafe = true;

/**
 * SDRAM STUFF
 * Make sure to not allocate the entire SDRAM because TFT relies on SDRAM for frame buffer
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

bool sample_buffer_clear() {
  memset(sample_buffer, 0, CUSTOM_POOL_SIZE);
  buffer_index = 0;
  return true;
}

#define FILE_BUFFER_SIZE (1024*1024)

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
std::vector<buttonInterface*> interfaces;

std::vector<buttonInterface*>::iterator userInterface;

Sequencer sequencer;

InstrumentDisplay instDisplay;

FXDisplay fxDisplay;

SampDisplay sampDisplay;

SongDisplay songDisplay;


/**
 * BUTTONS
 * Callbacks
 * Objects
 * and Vectors
*/
class Command {
public:
  Command(){}
  ~Command(){}

  virtual void Execute(buttonInterface* interface, bool shift) = 0;
};

class ACommand : public Command {
public:
  ACommand(){}
  ~ACommand(){}

  void Execute(buttonInterface* interface, bool shift) {
    if (shift) interface->AltAButton();
    else interface->AButton();
  }
};

class BCommand : public Command {
public:
  BCommand(){}
  ~BCommand(){}

  void Execute(buttonInterface* interface, bool shift) {
    if (shift) interface->AltBButton();
    else interface->BButton();
  }
};

class PlayCommand : public Command {
public:
  PlayCommand(){}
  ~PlayCommand(){}

  void Execute(buttonInterface* interface, bool shift) {
    if (shift) interface->AltPlayButton();
    else interface->PlayButton();
  }
};

class UpCommand : public Command {
public:
  UpCommand(){}
  ~UpCommand(){}

  void Execute(buttonInterface* interface, bool shift) {
    if (shift) interface->AltUpButton();
    else interface->UpButton();
  }
};

class DownCommand : public Command {
public:
  DownCommand(){}
  ~DownCommand(){}

  void Execute(buttonInterface* interface, bool shift) {
    if (shift) interface->AltDownButton();
    else interface->DownButton();
  }
};

class LeftCommand : public Command {
public:
  LeftCommand(){}
  ~LeftCommand(){}

  void Execute(buttonInterface* interface, bool shift) {
    if (shift) interface->AltLeftButton();
    else interface->LeftButton();
  }
};

class RightCommand : public Command {
public:
  RightCommand(){}
  ~RightCommand(){}

  void Execute(buttonInterface* interface, bool shift) {
    if (shift) interface->AltRightButton();
    else interface->RightButton();
  }
};

class LeftShoulderCommand : public Command {
public:
  LeftShoulderCommand(){}
  ~LeftShoulderCommand(){}

  void Execute(buttonInterface* interface, bool shift) {
    if (userInterface == interfaces.begin()) {
      userInterface = interfaces.begin();
    } else {
      userInterface--;
    }
  }
};

class RightShoulderCommand : public Command {
public:
  RightShoulderCommand(){}
  ~RightShoulderCommand(){}

  void Execute(buttonInterface* interface, bool shift) {
    if (userInterface != interfaces.end() - 1) {
      userInterface++;
    } else {
      userInterface = interfaces.end() - 1;
    }
  }
};

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

GPIO ok;
GPIO ok2;

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

  /**
   * Configuring TFT Display
   */
  INIT_DISPLAY(__Display);
  __Display.setOrientation(Rotation::Degre_90);
  DadGFX::cLayer* pMain = ADD_LAYER(MainLayer, 0, 0, 1);

  DadGFX::cFont MainFont(&KodeMono_Regular8pt7b);

  /**
   * Initializing Seed Audio
   */
  hw.SetAudioBlockSize(64); // number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  float samplerate = hw.AudioSampleRate();

  /**
   * Initialize SDIO & waveFileLoader
   */
  ok.Init(seed::D29, GPIO::Mode::OUTPUT);
  ok2.Init(seed::D28, GPIO::Mode::OUTPUT);

  SdmmcHandler::Config sd_cfg;
  sd_cfg.clock_powersave = true; // turn clock off when not in use to save power
  sd_cfg.speed = SdmmcHandler::Speed::VERY_FAST; // very fast because wav files are big
  sd_cfg.width = SdmmcHandler::BusWidth::BITS_4; // same here
  sd.Init(sd_cfg);

  // FatFS Interface
  if (fsi.Init(FatFSInterface::Config::MEDIA_SD) == FatFSInterface::Result::OK) ok.Write(true);

  // Mount SD Card
  if (f_mount(&fsi.GetSDFileSystem(), "/", 1) == FR_OK) {
    ok2.Write(true);

    /**
     * Initialize Wave File Loader
     */
    sampleDirLoader.Init(file_buffer_allocate, "/Samples"); // building separate trees; this one loads sample file info
    projDirLoader.Init(file_buffer_allocate, "/Projects"); // building separate trees; this one loads project file info
    waveFileLoader.Init(samplerate, sample_buffer_allocate, &readFIL);
    projSaverLoader.Init("/Projects/", &readFIL, &writeFIL, &sequencer, &instruments, &handler, CLEAR, SAFE, &waveFileLoader);
  }

  /**
   * Building instrument handlers
   */
  for (int i = 0; i < LANE_NUMBER; i ++) {
    handler.push_back(new InstrumentHandler);
    handler[i]->Init(&instruments, samplerate, &MainFont);
  }

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

  /**
   * Initializing UI objects
   * and adding to interfaces vector
   */
  limiter.Init();
  sequencer.Init(&handler, samplerate, &MainFont, &patterns, &songOrder);
  instDisplay.Init(&instruments, handler.at(0), &MainFont);
  fxDisplay.Init(samplerate, &handler, &MainFont);
  sampDisplay.Init(&waveFileLoader, &instruments, &MainFont, &buffer_index, sampleDirLoader.GetRootNode());
  songDisplay.Init(&sequencer, projDirLoader.GetRootNode(), &projSaverLoader, &MainFont, file_buffer_allocate);

  interfaces.push_back(&songDisplay);
  interfaces.push_back(&sampDisplay);
  interfaces.push_back(&sequencer);
  interfaces.push_back(&instDisplay);
  interfaces.push_back(&fxDisplay);
  
  userInterface = interfaces.begin() + 2;

  hw.StartAudio(AudioCallback);

  uint32_t time = System::GetNow();  

  float flushTime = 0; 
  float processTime = 0;

  char strbuff[20];

  for (;;) {
    if (System::GetNow() > time + 17) {
      (*userInterface)->UpdateDisplay(pMain); 
      sprintf(strbuff, "%.0fms", flushTime);
      pMain->setCursor(0, CHAR_HEIGHT);
      pMain->setFont(&MainFont);
      pMain->setTextFrontColor(MAIN);
      pMain->drawText(strbuff);
      sprintf(strbuff, "%.0fms", processTime);
      pMain->setCursor(0, CHAR_HEIGHT * 2 + 4);
      pMain->setFont(&MainFont);
      pMain->setTextFrontColor(MAIN);
      pMain->drawText(strbuff);
      processTime = ((float) (System::GetNow() - time));
      __Display.flush();
      flushTime = ((float) (System::GetNow() - time)) - processTime;
      time = System::GetNow();
    }
    while (eventQueue.size() != 0) {
      eventQueue.front()->Execute((*userInterface), shift);
      eventQueue.pop();
    }
  }
}
