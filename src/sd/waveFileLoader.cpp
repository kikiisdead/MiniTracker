#include "waveFileLoader.h"

void WaveFileLoader::Init(float samplerate, void* (*sample_buffer_allocate)(size_t size), FIL* fil) {
    this->samplerate = samplerate;
    this->sample_buffer_allocate = sample_buffer_allocate;
    this->fil = fil;
}

Instrument* WaveFileLoader::CreateInstrument(std::string path) {

    UINT bytesread;

    if (f_open(fil, path.c_str(), (FA_OPEN_EXISTING | FA_READ)) == FR_OK) {

        WavFile* sample = new WavFile; // CREATING ON HEAP

        WAV_FormatTypeDef wave;

        /**
         * RIFF Header
         */
        f_read(fil, (void*)&(wave.ChunkId), sizeof(wave.ChunkId), &bytesread);
        f_read(fil, (void*)&(wave.FileSize), sizeof(wave.FileSize), &bytesread);
        f_read(fil, (void*)&(wave.FileFormat), sizeof(wave.FileFormat), &bytesread);

        /**
         * SubChunk1
         */
        f_read(fil, (void*)&(wave.SubChunk1ID), sizeof(wave.SubChunk1ID), &bytesread);
        f_read(fil, (void*)&(wave.SubChunk1Size), sizeof(wave.SubChunk1Size), &bytesread);
        f_read(fil, (void*)&(wave.AudioFormat), sizeof(wave.AudioFormat), &bytesread);
        f_read(fil, (void*)&(wave.NbrChannels), sizeof(wave.NbrChannels), &bytesread);
        f_read(fil, (void*)&(wave.SampleRate), sizeof(wave.SampleRate), &bytesread);
        f_read(fil, (void*)&(wave.ByteRate), sizeof(wave.ByteRate), &bytesread);
        f_read(fil, (void*)&(wave.BlockAlign), sizeof(wave.BlockAlign), &bytesread);
        f_read(fil, (void*)&(wave.BitPerSample), sizeof(wave.BitPerSample), &bytesread);

        /**
         * Non PCM data has 2 byte extension size (PCM data meaning 16bit PCM soa nything not that gets an extension)
         */
        if (wave.AudioFormat != WAVE_FORMAT_PCM) {
            f_read(fil, (void*)&(wave.extensionSize), sizeof(wave.extensionSize), &bytesread);
        }

        /**
         * Wave format extensible
         */
        if (wave.AudioFormat == WAVE_FORMAT_EXTENSIBLE || wave.extensionSize == (uint16_t) 22) {
            f_read(fil, (void*)&(wave.ValidBitsPerSample), sizeof(wave.ValidBitsPerSample), &bytesread);
            f_read(fil, (void*)&(wave.dwChannelMask), sizeof(wave.dwChannelMask), &bytesread);
            f_read(fil, (void*)&(wave.SubformatCode), sizeof(wave.SubformatCode), &bytesread);
            f_read(fil, (void*)&(wave.SubformatPad1), sizeof(wave.SubformatPad1), &bytesread);
            f_read(fil, (void*)&(wave.SubformatPad2), sizeof(wave.SubformatPad2), &bytesread);
        }

        /**
         * Fact Chunk
         */
        if (wave.AudioFormat != WAVE_FORMAT_PCM && wave.SubformatCode != WAVE_FORMAT_PCM) {
            f_read(fil, (void*)&(wave.FactChunkID), sizeof(wave.FactChunkID), &bytesread);
            f_read(fil, (void*)&(wave.FactChunkSize), sizeof(wave.FactChunkSize), &bytesread);
            f_read(fil, (void*)&(wave.SampleLength), sizeof(wave.SampleLength), &bytesread);
        } 

        /**
         * Data Chunk
         */
        f_read(fil, (void*)&(wave.SubChunk2ID), sizeof(wave.SubChunk2ID), &bytesread);
        f_read(fil, (void*)&(wave.SubCHunk2Size), sizeof(wave.SubCHunk2Size), &bytesread);

        std::string name;
        size_t index = path.find_last_of('/');
        if (index != std::string::npos) {
            name = path.substr(index + 1, path.length() - (index + 1));
        } else {
            name = path;
        }

        sprintf(sample->name, "%s", name.c_str());

        sample->format = wave;
        
        size_t readIndex = 0;

        /**
         * READ MULAW
         */
        if (wave.AudioFormat == WAVE_FORMAT_ULAW) {

            __disable_irq();
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / sizeof(uint8_t)) * sizeof(float));
            __enable_irq();

            if (sample->start == 0) return nullptr;

            __disable_irq();
            void* writePtr = sample->start;
            __enable_irq();

            while (readIndex < wave.SubCHunk2Size) {
                uint8_t val;

                if (f_read(fil, (void*)&val, sizeof(val), &bytesread) == FR_OK) { // reading sd card 1 byte at a time
                    int16_t temp = MuLaw2Lin((val * -1) - 1); // decode mulaw
                    float samp = s162f(temp); // turn to float
                    readIndex += 1; 

                    __disable_irq();
                    *(static_cast<float*>(writePtr)) = samp; // add to sdram
                    writePtr = static_cast<float*>(writePtr) + 1;
                    __enable_irq();
                } else 
                    break;
            }
        } 

        /**
         * READ ALAW
         */
        else if (wave.AudioFormat == WAVE_FORMAT_ALAW) {

            __disable_irq();
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / sizeof(uint8_t)) * sizeof(float));
            __enable_irq();

            if (sample->start == 0) return nullptr;
  
            __disable_irq();
            void* writePtr = sample->start;
            __enable_irq();

            while (readIndex < wave.SubCHunk2Size) {

                uint8_t val;

                if (f_read(fil, (void*)&val, sizeof(val), &bytesread) == FR_OK) { // reading sd card 1 byte at a time
                    int16_t temp = ALaw2Lin((val * -1) - 1); // decode mulaw
                    float samp = s162f(temp); // turn to float
                    readIndex += 1; 

                    __disable_irq();
                    *(static_cast<float*>(writePtr)) = samp; // add to sdram
                    writePtr = static_cast<float*>(writePtr) + 1;
                    __enable_irq();

                } else 
                    break;
            }
        } 

        /**
         * READ 16bit PCM
         */
        else if (wave.AudioFormat == WAVE_FORMAT_PCM && wave.BitPerSample == 16) {

            __disable_irq();
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / sizeof(int16_t)) * sizeof(float));
            __enable_irq();
            
            if (sample->start == 0) return nullptr;
  
            __disable_irq();
            void* writePtr = sample->start;
            __enable_irq();
            
            while (readIndex < wave.SubCHunk2Size / sizeof(int16_t)) {

                int16_t val;

                if (f_read(fil, (void*)&val, sizeof(val), &bytesread) == FR_OK) { // reading sd card 2 bytes at a time

                    float samp = s162f(val); // turn to float
                    readIndex += 1; 

                    __disable_irq();
                    *(static_cast<float*>(writePtr)) = samp; // add to sdram
                    writePtr = static_cast<float*>(writePtr) + 1;
                    __enable_irq();
                } else 
                    break;
            }

            sample->format.SubCHunk2Size = sample->format.SubCHunk2Size / sizeof(int16_t);
        }
        /**
         * READ 24bit PCM
         */
        else if (((wave.AudioFormat == WAVE_FORMAT_EXTENSIBLE) 
            ||  wave.AudioFormat == WAVE_FORMAT_PCM)
            &&  wave.BitPerSample == 24) {

            __disable_irq();
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / (sizeof(int8_t) * 3)) * sizeof(float));
            __enable_irq();

            if (sample->start == 0) return nullptr;
  
            __disable_irq();
            void* writePtr = sample->start;
            __enable_irq();

            while (readIndex < wave.SubCHunk2Size / sizeof(Bit3x8)) {

                Bit3x8 bit;

                if (f_read(fil, (void*)&bit, sizeof(bit), &bytesread) == FR_OK) {
                    int32_t MASK = 0x000000FF;
                    // little endian order so biggest goes first
                    int32_t temp = bit.b3 & MASK;
                    temp = (temp << 8) | (bit.b2 & MASK);
                    temp = (temp << 8) | (bit.b1 & MASK);
                    
                    float samp = s242f(temp);
                    readIndex += 1; 

                    __disable_irq(); // disabling interrupts while adding to sdram
                    *(static_cast<float*>(writePtr)) = samp; // add to sdram
                    writePtr = static_cast<float*>(writePtr) + 1;
                    __enable_irq();
                } else 
                    break;
            }

            sample->format.SubCHunk2Size = sample->format.SubCHunk2Size / (sizeof(int8_t) * 3);
        }
        /**
         * READ 32bit float
         */
        else if (wave.AudioFormat == WAVE_FORMAT_IEEE_FLOAT) {

            __disable_irq(); // accessing void pointer to SDRAM, interrupts disabled for priority
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / sizeof(float)) * sizeof(float));
            __enable_irq();

            if (sample->start == 0) return nullptr;
  
            __disable_irq();
            void* writePtr = sample->start;
            __enable_irq();

            while (readIndex < wave.SubCHunk2Size / sizeof(float)) {

                float val;

                __disable_irq();
                if (f_read(fil, (void*)&val, sizeof(val), &bytesread) == FR_OK) { // reading sd card 2 bytes at a time
                    readIndex += 1; 
                    *(static_cast<float*>(writePtr)) = val; // add to sdram
                    writePtr = static_cast<float*>(writePtr) + 1;
                } else break;
                __enable_irq();
            }
            sample->format.SubCHunk2Size = sample->format.SubCHunk2Size / sizeof(float);
        } 
        else { // wave format does not match any type we can read
            f_close(fil);
            return nullptr;
        }

        f_close(fil);

        Instrument* instrument = new Instrument; // CREATING ON HEAP

        instrument->Init(samplerate, sample, path);

        // __enable_irq();

        return instrument;
    } else {
        return nullptr;
    }
}

Instrument* WaveFileLoader::CreateInstrument(Node<File>* node) {

    if (node->data.attrib & (AM_DIR)) {
        return nullptr;
    }
    
    std::string path = std::string(*node->data.name);

    if (path.find(".wav") == std::string::npos && path.find(".WAV") == std::string::npos) 
        return nullptr;

    Node<File> *temp = node->parent;

    while (temp != nullptr) {
        path = std::string(*temp->data.name) + std::string("/") + path;
        temp = temp->parent;
    }

    return CreateInstrument(path);

}

