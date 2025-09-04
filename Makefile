# Project Name
TARGET = MiniTracker

# Sources
CPP_SOURCES = MiniTracker.cpp
CPP_SOURCES += src/UI/sequencer.cpp
CPP_SOURCES += src/audio/sampleplayer.cpp
CPP_SOURCES += src/audio/instrument.cpp
CPP_SOURCES += src/samples/cowbell.cpp

# Bootload into QSPI Flash instead of ARM Chip Flash
APP_TYPE = BOOT_QSPI

# Library Locations
LIBDAISY_DIR = ../DaisyExamples/libDaisy/
DAISYSP_DIR = ../DaisyExamples/DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
