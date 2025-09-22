# Project Name
TARGET = MiniTracker

USE_DAISYSP_LGPL=1

LDFLAGS = -u _printf_float

# Sources
CPP_SOURCES = MiniTracker.cpp
CPP_SOURCES += src/sd/waveFileLoader.cpp
CPP_SOURCES += src/UI/sequencer.cpp
CPP_SOURCES += src/UI/instDisplay.cpp
CPP_SOURCES += src/UI/fxDisplay.cpp
CPP_SOURCES += src/UI/sampDisplay.cpp
CPP_SOURCES += src/audio/sampleplayer.cpp
CPP_SOURCES += src/audio/instrument.cpp
CPP_SOURCES += src/audio/decode.cpp
CPP_SOURCES += DaisySeedGFX2/cDisplay.cpp
CPP_SOURCES += DaisySeedGFX2/GFX.cpp
CPP_SOURCES += DaisySeedGFX2/TFT_SPI.cpp

# Bootload into QSPI Flash instead of ARM Chip Flash
APP_TYPE = BOOT_QSPI

# Library Locations
LIBDAISY_DIR = ../DaisyExamples/libDaisy/
DAISYSP_DIR = ../DaisyExamples/DaisySP/

# Use FatFS so that I don't get some fuck ass undefined reference
USE_FATFS = 1

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
