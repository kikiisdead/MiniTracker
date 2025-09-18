#include "waveFileLoader.h"



void WaveFileLoader::Init(float samplerate, void* (*sample_buffer_allocate)(size_t size)) {
    this->samplerate = samplerate;
    this->sample_buffer_allocate = sample_buffer_allocate;

    SdmmcHandler::Config sd_cfg;
    sd_cfg.Defaults();
    sd_cfg.speed = SdmmcHandler::Speed::SLOW;
    if (sd.Init(sd_cfg) == SdmmcHandler::Result::OK) {
        // FatFS Interface
        if (fsi.Init(FatFSInterface::Config::MEDIA_SD) == FatFSInterface::Result::OK) {
            // Mount SD Card
            f_mount(&fsi.GetSDFileSystem(), "/", 1);
            safe = true;
            OpenDir("/");
            return;
        }
    }
    safe = false;
}

void WaveFileLoader::OpenDir(const char* appendPath) {
    if (!safe) return; // does nothing if f_mount failed
    f_closedir(&dir);

    sprintf(searchpath + strlen(searchpath), appendPath);

    if (f_opendir(&dir, searchpath) != FR_OK) {
        safe = false;
        return;
    }

    fileNames.clear();

    bool result;
    do
    {
        result = f_readdir(&dir, &fno);
        // Exit if bad read or NULL fname
        if(result != FR_OK || fno.fname[0] == 0)
            break;
        // Skip if its a hidden file.
        if(fno.fattrib & (AM_HID))
            continue;
        
        file finfo;
        sprintf(finfo.searchpath, searchpath);
        sprintf(finfo.name, searchpath);
        sprintf(finfo.name + strlen(finfo.name), fno.fname);
        finfo.attrib = fno.fattrib;
        fileNames.push_back(finfo);

    } while(result == FR_OK);


}

std::vector<file> WaveFileLoader::GetFileNames() {
    return fileNames;
}

Instrument* WaveFileLoader::CreateInstrument(int index) {
    if (!safe) return nullptr; // can't find sd card
    UINT bytesread;

    if (fileNames.at(index).attrib & (AM_DIR)) {
        OpenDir(fileNames.at(index).name);
        return nullptr;
    }

    // std::string fileSearch = name + searchpath;

    if (f_open(&fil, fileNames.at(index).name, (FA_OPEN_EXISTING | FA_READ)) == FR_OK) {

        WavFile* sample = new WavFile; // CREATING ON HEAP

        WAV_FormatTypeDef wave;

        // f_read(&fil, (void*)&wave, sizeof(wave), &bytesread);

        /**
         * RIFF Header
         */
        f_read(&fil, (void*)&(wave.ChunkId), sizeof(wave.ChunkId), &bytesread);
        f_read(&fil, (void*)&(wave.FileSize), sizeof(wave.FileSize), &bytesread);
        f_read(&fil, (void*)&(wave.FileFormat), sizeof(wave.FileFormat), &bytesread);

        /**
         * SubChunk1
         */
        f_read(&fil, (void*)&(wave.SubChunk1ID), sizeof(wave.SubChunk1ID), &bytesread);
        f_read(&fil, (void*)&(wave.SubChunk1Size), sizeof(wave.SubChunk1Size), &bytesread);
        f_read(&fil, (void*)&(wave.AudioFormat), sizeof(wave.AudioFormat), &bytesread);
        f_read(&fil, (void*)&(wave.NbrChannels), sizeof(wave.NbrChannels), &bytesread);
        f_read(&fil, (void*)&(wave.SampleRate), sizeof(wave.SampleRate), &bytesread);
        f_read(&fil, (void*)&(wave.ByteRate), sizeof(wave.ByteRate), &bytesread);
        f_read(&fil, (void*)&(wave.BlockAlign), sizeof(wave.BlockAlign), &bytesread);
        f_read(&fil, (void*)&(wave.BitPerSample), sizeof(wave.BitPerSample), &bytesread);

        /**
         * Non PCM data has 2 byte extension size
         */
        if (wave.AudioFormat != WAVE_FORMAT_PCM) {
            f_read(&fil, (void*)&(wave.extensionSize), sizeof(wave.extensionSize), &bytesread);
        }

        /**
         * Wave format extensible
         */
        if (wave.AudioFormat == WAVE_FORMAT_EXTENSIBLE || wave.extensionSize == (uint16_t) 22) {
            f_read(&fil, (void*)&(wave.ValidBitsPerSample), sizeof(wave.ValidBitsPerSample), &bytesread);
            f_read(&fil, (void*)&(wave.dwChannelMask), sizeof(wave.dwChannelMask), &bytesread);
            f_read(&fil, (void*)&(wave.SubformatCode), sizeof(wave.SubformatCode), &bytesread);
            f_read(&fil, (void*)&(wave.SubformatPad1), sizeof(wave.SubformatPad1), &bytesread);
            f_read(&fil, (void*)&(wave.SubformatPad2), sizeof(wave.SubformatPad2), &bytesread);
        }

        /**
         * Fact Chunk
         */
        if (wave.AudioFormat != WAVE_FORMAT_PCM && wave.SubformatCode != WAVE_FORMAT_PCM) {
            f_read(&fil, (void*)&(wave.FactChunkID), sizeof(wave.FactChunkID), &bytesread);
            f_read(&fil, (void*)&(wave.FactChunkSize), sizeof(wave.FactChunkSize), &bytesread);
            f_read(&fil, (void*)&(wave.SampleLength), sizeof(wave.SampleLength), &bytesread);
        }

        /**
         * Data Chunk
         */
        f_read(&fil, (void*)&(wave.SubChunk2ID), sizeof(wave.SubChunk2ID), &bytesread);
        f_read(&fil, (void*)&(wave.SubCHunk2Size), sizeof(wave.SubCHunk2Size), &bytesread);

        sprintf(sample->name, fileNames.at(index).name);

        sample->format = wave;
        
        size_t readIndex = 0;

        /**
         * READ MULAW
         */
        if (wave.AudioFormat == WAVE_FORMAT_ULAW) {
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / sizeof(uint8_t)) * sizeof(float));
  
            void* writePtr = sample->start;
            while (readIndex < wave.SubCHunk2Size) {
                uint8_t val;
                if (f_read(&fil, (void*)&val, sizeof(val), &bytesread) == FR_OK) { // reading sd card 1 byte at a time
                    int16_t temp = MuLaw2Lin((val * -1) - 1); // decode mulaw
                    float samp = s162f(temp); // turn to float
                    readIndex += 1; 
                    *(static_cast<float*>(writePtr)) = samp; // add to sdram
                    writePtr = static_cast<float*>(writePtr) + 1;
                } else break;
            }
        } 

        /**
         * READ ALAW
         */
        else if (wave.AudioFormat == WAVE_FORMAT_ALAW) {
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / sizeof(uint8_t)) * sizeof(float));
  
            void* writePtr = sample->start;
            while (readIndex < wave.SubCHunk2Size) {

                uint8_t val;

                if (f_read(&fil, (void*)&val, sizeof(val), &bytesread) == FR_OK) { // reading sd card 1 byte at a time
                    int16_t temp = ALaw2Lin((val * -1) - 1); // decode mulaw
                    float samp = s162f(temp); // turn to float
                    readIndex += 1; 
                    *(static_cast<float*>(writePtr)) = samp; // add to sdram
                    writePtr = static_cast<float*>(writePtr) + 1;
                } else break;
            }
        } 

        /**
         * READ 16bit PCM
         */
        else if (wave.AudioFormat == WAVE_FORMAT_PCM && wave.BitPerSample == 16) {
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / sizeof(int16_t)) * sizeof(float));
  
            void* writePtr = sample->start;
            while (readIndex < wave.SubCHunk2Size / sizeof(int16_t)) {

                int16_t val;

                if (f_read(&fil, (void*)&val, sizeof(val), &bytesread) == FR_OK) { // reading sd card 2 bytes at a time
                    float samp = s162f(val); // turn to float
                    readIndex += 1; 
                    *(static_cast<float*>(writePtr)) = samp; // add to sdram
                    writePtr = static_cast<float*>(writePtr) + 1;
                } else break;
            }

            sample->format.SubCHunk2Size = sample->format.SubCHunk2Size / sizeof(int16_t);
        }
        /**
         * READ 24bit PCM
         */
        else if (((wave.AudioFormat == WAVE_FORMAT_EXTENSIBLE) 
            ||  wave.AudioFormat == WAVE_FORMAT_PCM)
            &&  wave.BitPerSample == 24) {
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / (sizeof(int8_t) * 3)) * sizeof(float));
  
            void* writePtr = sample->start;
            while (readIndex < wave.SubCHunk2Size / (sizeof(int8_t) * 3)) {

                int32_t val = 0; // need to have this initialized
                int8_t byte;

                for (int i = 0; i < 3; i++) { // reading 1 byte at a time and adding to val
                    f_read(&fil, (void*)&byte, sizeof(byte), &bytesread);
                    val = val | byte;
                    val = val << (i * 8);
                }

                float samp = s242f(val); // turn to float
                readIndex += 1; 
                *(static_cast<float*>(writePtr)) = samp; // add to sdram
                writePtr = static_cast<float*>(writePtr) + 1;
            }

            sample->format.SubCHunk2Size = sample->format.SubCHunk2Size / (sizeof(int8_t) * 3);
        }
        /**
         * READ 32bit float
         */
        else if (wave.AudioFormat == WAVE_FORMAT_IEEE_FLOAT) {
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / sizeof(int16_t)) * sizeof(float));
  
            void* writePtr = sample->start;
            while (readIndex < wave.SubCHunk2Size / sizeof(int16_t)) {

                float val;

                if (f_read(&fil, (void*)&val, sizeof(val), &bytesread) == FR_OK) { // reading sd card 2 bytes at a time
                    readIndex += 1; 
                    *(static_cast<float*>(writePtr)) = val; // add to sdram
                    writePtr = static_cast<float*>(writePtr) + 1;
                } else break;
            }
            sample->format.SubCHunk2Size = sample->format.SubCHunk2Size / sizeof(float);
        }

        f_close(&fil);

        Instrument* instrument = new Instrument; // CREATING ON HEAP

        instrument->Init(samplerate, sample);

        return instrument;
    } else {
        return nullptr;
    }
}

