#include "waveFileLoader.h"



void WaveFileLoader::Init(float samplerate, void* (*sample_buffer_allocate)(size_t size)) {
    this->samplerate = samplerate;
    this->sample_buffer_allocate = sample_buffer_allocate;
    safe = true;
    rootNode.up = nullptr;
    sprintf(rootNode.data.name, "/");
    OpenDir(rootNode.data.name, &rootNode);
}

void WaveFileLoader::OpenDir(const char* path, Node<File>* node) {
    if (!safe) return; // does nothing if f_mount failed

    DIR            dir; // exist locally for each recursive call as opposed to globally
    FILINFO        fno;

    if (f_opendir(&dir, path) != FR_OK) {
        safe = false;
        return;
    }
    
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

        node->down.push_back(new Node<File>);
        node->down.back()->up = node;

        sprintf(node->down.back()->data.name, "%.254s/", node->data.name);
        sprintf(node->down.back()->data.name + strlen(node->down.back()->data.name), fno.fname);
        node->down.back()->data.attrib = fno.fattrib;

        if (node->down.back()->data.attrib & (AM_DIR)) {
            // only recursive if it is a directory
            OpenDir(node->down.back()->data.name, node->down.back());
        } else {
            node->down.back()->down.push_back(nullptr);
        }

    } while(result == FR_OK);

    f_closedir(&dir);
}

Instrument* WaveFileLoader::CreateInstrument(Node<File>* node) {
    if (!safe) return nullptr; // can't find sd card

    
    UINT bytesread;

    if (node->data.attrib & (AM_DIR)) {
        return nullptr;
    }

    if (f_open(&fil, node->data.name, (FA_OPEN_EXISTING | FA_READ)) == FR_OK) {


        WavFile* sample = new WavFile; // CREATING ON HEAP

        WAV_FormatTypeDef wave;

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
         * Non PCM data has 2 byte extension size (PCM data meaning 16bit PCM soa nything not that gets an extension)
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

        std::string file = std::string(node->data.name);
        size_t index = file.find_last_of('/') + 1; 
        std::string name = file.substr(index, file.length() - index);
        sprintf(sample->name, "%s", name.c_str());

        sample->format = wave;
        
        size_t readIndex = 0;

        /**
         * READ MULAW
         */
        if (wave.AudioFormat == WAVE_FORMAT_ULAW) {
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / sizeof(uint8_t)) * sizeof(float));
            if (sample->start == 0) return nullptr;
  
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
            if (sample->start == 0) return nullptr;
  
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
            if (sample->start == 0) return nullptr;
  
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
            if (sample->start == 0) return nullptr;
  
            void* writePtr = sample->start;
            while (readIndex < wave.SubCHunk2Size / sizeof(Bit24x4)) {

                Bit24x4 bit;
                f_read(&fil, (void*)&bit, sizeof(bit), &bytesread);
                
                int32_t temp1 = ((bit.val1      ) & 0x00FFFFFF);
                int32_t temp2 = ((bit.val1 >> 24) & 0x000000FF) | ((bit.val2 <<  8) & 0x00FFFF00);
                int32_t temp3 = ((bit.val2 >> 16) & 0x0000FFFF) | ((bit.val3 << 16) & 0x00FF0000);
                int32_t temp4 = ((bit.val3 >>  8) & 0x00FFFFFF);

                
                float samp1 = s242f(temp1);
                float samp2 = s242f(temp2);
                float samp3 = s242f(temp3);
                float samp4 = s242f(temp4);
                readIndex += 1; 
                *(static_cast<float*>(writePtr)) = samp1; // add to sdram
                writePtr = static_cast<float*>(writePtr) + 1;
                *(static_cast<float*>(writePtr)) = samp2; // add to sdram
                writePtr = static_cast<float*>(writePtr) + 1;
                *(static_cast<float*>(writePtr)) = samp3; // add to sdram
                writePtr = static_cast<float*>(writePtr) + 1;
                *(static_cast<float*>(writePtr)) = samp4; // add to sdram
                writePtr = static_cast<float*>(writePtr) + 1;
            }

            sample->format.SubCHunk2Size = sample->format.SubCHunk2Size / (sizeof(int8_t) * 3);
        }
        /**
         * READ 32bit float
         */
        else if (wave.AudioFormat == WAVE_FORMAT_IEEE_FLOAT) {
            sample->start = sample_buffer_allocate((wave.SubCHunk2Size / sizeof(float)) * sizeof(float));
            if (sample->start == 0) return nullptr;
  
            void* writePtr = sample->start;
            while (readIndex < wave.SubCHunk2Size / sizeof(float)) {

                float val;

                if (f_read(&fil, (void*)&val, sizeof(val), &bytesread) == FR_OK) { // reading sd card 2 bytes at a time
                    readIndex += 1; 
                    *(static_cast<float*>(writePtr)) = val; // add to sdram
                    writePtr = static_cast<float*>(writePtr) + 1;
                } else break;
            }
            sample->format.SubCHunk2Size = sample->format.SubCHunk2Size / sizeof(float);
        } 
        else { // if code does not match any type we can read
            f_close(&fil);
            return nullptr;
        }

        f_close(&fil);

        Instrument* instrument = new Instrument; // CREATING ON HEAP

        instrument->Init(samplerate, sample);

        // __enable_irq();

        return instrument;
    } else {
        return nullptr;
    }
}

