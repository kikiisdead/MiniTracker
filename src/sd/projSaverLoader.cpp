#include "projSaverLoader.h"

void ProjSaverLoader::Init(const char* savePath, FIL* fil, FIL *writeFIL,  Sequencer* sequencer, std::vector<Instrument*>* instVector, std::vector<InstrumentHandler*>* instHandler, void (*CLEAR)(), void (*SAFE)(), WaveFileLoader* waveFileLoader) {
    this->savePath = std::string(savePath);
    safe = true; // function called which means mounted safely
    buffer_index = 0;
    memset(buf, 0, sizeof(buf));
    this->fil = fil;
    this->writeFIL = writeFIL;
    this->sequencer = sequencer;
    this->instVector = instVector;
    this->instHandler = instHandler;
    this->CLEAR = CLEAR;
    this->SAFE = SAFE;
    this->waveFileLoader = waveFileLoader;
}

template <typename T>
void ProjSaverLoader::Write(T val) {
    Write(val, sizeof(val));
}

template <typename T>
void ProjSaverLoader::Write(T val, size_t size) {
    // Write to file if exceeds capacity and resets the buffer
    if (buffer_index + size > WRITE_BUFFER_SIZE) { 
        size_t diff = WRITE_BUFFER_SIZE - buffer_index;
        void* ptr = &val;
        if (diff != 0) Write(((uint8_t *) ptr)[0], diff);
        ResetAndPush();
        Write(((uint8_t *) ptr)[diff], size - diff);
    } else {
        void* writePtr = &buf[buffer_index];
        std::memcpy(writePtr, &val, size);
        buffer_index += size;
    }
}

void ProjSaverLoader::ResetAndPush() {
    f_write(writeFIL, buf, buffer_index, &bytesWritten);
    buffer_index = 0;
    memset(buf, 0, sizeof(buf)); 
}

void ProjSaverLoader::LoadProject(const char* name) {
    std::string save = savePath + std::string(name);

    if (f_open(fil, save.c_str(), (FA_OPEN_EXISTING) | (FA_READ)) == FR_OK) {

        CLEAR();

        uint32_t HEADER;

        // Sequencer Header
        f_read(fil, &HEADER, sizeof(HEADER), &bytesWritten);

        // BPM
        float bpm;
        f_read(fil, &bpm, sizeof(bpm), &bytesWritten);
        sequencer->SetBPM(bpm);

        //SONG HEADER
        f_read(fil, &HEADER, sizeof(HEADER), &bytesWritten);

        // Song Order length
        size_t songOrderLength;
        f_read(fil, &songOrderLength, sizeof(songOrderLength), &bytesWritten);

        // indeces * length
        for (size_t i = 0; i < songOrderLength; i ++) {
            int pattIndex;
            f_read(fil, &pattIndex, sizeof(pattIndex), &bytesWritten);
            sequencer->GetSongOrder()->push_back(pattIndex);
        }

        // Num Patterns
        size_t numPatterns;
        f_read(fil, &numPatterns, sizeof(numPatterns), &bytesWritten);

        for (size_t patt = 0; patt < numPatterns; patt ++) {
            //PATTERN HEADER
            f_read(fil, &HEADER, sizeof(HEADER), &bytesWritten);

            sequencer->GetPatterns()->push_back(new Pattern);
            sequencer->GetPatterns()->back()->index = patt;

            // Num Lanes
            int numLanes;
            f_read(fil, &numLanes, sizeof(numLanes), &bytesWritten);
            sequencer->GetPatterns()->back()->numLanes = numLanes;

            for (int lane = 0; lane < numLanes; lane ++) {
                // LANE HEADER
                f_read(fil, &HEADER, sizeof(HEADER), &bytesWritten);
                sequencer->GetPatterns()->back()->lanes.push_back(new Lane);
                sequencer->GetPatterns()->back()->lanes.back()->index = lane;

                // LENGTH
                int sequenceLength; 
                f_read(fil, &sequenceLength, sizeof(sequenceLength), &bytesWritten);
                sequencer->GetPatterns()->back()->lanes.back()->length = sequenceLength;

                for (int step = 0; step < sequenceLength; step++) {
                    // LANE HEADER
                    f_read(fil, &HEADER, sizeof(HEADER), &bytesWritten);

                    int8_t inst;
                    uint8_t note;
                    uint8_t fx;
                    uint8_t fxAmount;

                    // inst
                    f_read(fil, &inst, sizeof(inst), &bytesWritten);
                    // note
                    f_read(fil, &note, sizeof(note), &bytesWritten);
                    // fx
                    f_read(fil, &fx, sizeof(fx), &bytesWritten);
                    // fxAmount
                    f_read(fil, &fxAmount, sizeof(fxAmount), &bytesWritten);

                    sequencer->GetPatterns()->back()->lanes.back()->sequence.push_back(new Step(inst, note, fx, fxAmount, step));
                }
            }
        }

        //Fcon HEADER
        f_read(fil, &HEADER, sizeof(HEADER), &bytesWritten);

        // lanecount
        uint32_t lanecount;
        f_read(fil, &lanecount, sizeof(lanecount), &bytesWritten);

        for (uint32_t lane = 0; lane < lanecount; lane ++) {
            for (int fx = 0; fx < 3; fx++) {

                //FX HEADER
                f_read(fil, &HEADER, sizeof(HEADER), &bytesWritten);

                // effect type
                int type;
                f_read(fil, &type, sizeof(type), &bytesWritten);

                // effect specific params
                char effectBuff[32];

                for (size_t i = 0; i < sizeof(effectBuff); i ++) {
                    f_read(fil, &effectBuff[i], sizeof(char), &bytesWritten);
                }
                // creating effect
                instHandler->at(lane)->AddEffect((Effect::EFFECT_TYPE) type, effectBuff);
            }
        }

        //Icon HEADER
        f_read(fil, &HEADER, sizeof(HEADER), &bytesWritten);

        uint32_t numInst;
        f_read(fil, &numInst, sizeof(numInst), &bytesWritten);
        // reading 0 for num instruments
        Instrument::Config* instCfg[numInst];
        
        std::string* searchPaths[numInst];

        // std::vector<Instrument::Config*> instCfg;
        // std::vector<std::string*> searchPaths;
        
        for (uint32_t inst = 0; inst < numInst; inst ++) {
            //INST HEADER
            f_read(fil, &HEADER, sizeof(HEADER), &bytesWritten);

            instCfg[inst] = new Instrument::Config;

            uint32_t numSlices, pathLength;

            // attack
            f_read(fil, &instCfg[inst]->Attack, sizeof(instCfg[inst]->Attack), &bytesWritten);

            // decay
            f_read(fil, &instCfg[inst]->Decay, sizeof(instCfg[inst]->Decay), &bytesWritten);

            // sustain
            f_read(fil, &instCfg[inst]->Sustain, sizeof(instCfg[inst]->Sustain), &bytesWritten);

            // release
            f_read(fil, &instCfg[inst]->Release, sizeof(instCfg[inst]->Release), &bytesWritten);

            // gain
            f_read(fil, &instCfg[inst]->Gain, sizeof(instCfg[inst]->Gain), &bytesWritten);

            // pitch
            f_read(fil, &instCfg[inst]->Pitch, sizeof(instCfg[inst]->Pitch), &bytesWritten);

            // numSlices
            f_read(fil, &numSlices, sizeof(numSlices), &bytesWritten);

            for (uint32_t i = 0; i < numSlices; i ++) {
                float slice;

                // numSlices
                f_read(fil, &slice, sizeof(slice), &bytesWritten);
                instCfg[inst]->slices.push_back(slice);
            }

            // pathLength
            f_read(fil, &pathLength, sizeof(pathLength), &bytesWritten);

            // search path
            searchPaths[inst] = new std::string("");

            for (uint32_t c = 0; c < pathLength; c ++) {
                char chr;
                f_read(fil, &chr, sizeof(chr), &bytesWritten);
                *searchPaths[inst] += chr;
            }

        }

        f_close(fil);

        // creating instruments
        for (uint32_t i = 0; i < numInst; i ++) {
            Instrument* inst = waveFileLoader->CreateInstrument(*searchPaths[i]);
            if (inst != nullptr) {
                inst->Load(*instCfg[i]);
                instVector->push_back(inst);
            }
            delete searchPaths[i]; // free up that memory
            delete instCfg[i];
        }

    }

    SAFE();
}

void ProjSaverLoader::SaveProject(const char* name) {

    std::string save = savePath + std::string(name);

    if (f_open(writeFIL, save.c_str(), (FA_CREATE_ALWAYS) | (FA_WRITE)) == FR_OK && safe) {

        // SEQUENCER HEADER
        Write(0x20514553); // "SEQ "
 
        // BPM
        Write(sequencer->GetBPM());

        // SONG ORDER HEADER
        Write(0x474E4F53); // "SONG"

        // Song order length
        Write(sequencer->GetSongOrder()->size());

        // Writing Song indeces
        for (int index : *sequencer->GetSongOrder()) {
            Write(index);
        }

        // Num of Patterns
        Write(sequencer->GetPatterns()->size());

        if (sequencer->GetPatterns()->size() != 0) {
            for (Pattern* pattern : *sequencer->GetPatterns()) {
                // PATTERN HEADER
                Write(0x54544150); // "PATT"

                // num lanes
                Write(pattern->numLanes);

                for (Lane* lane : pattern->lanes) {
                    // LANE HEADER
                    Write(0x454E414C); // "LANE"

                    // pattern length
                    Write(lane->length);

                    for (Step* step : lane->sequence) {
                        // STEP HEADER
                        Write(0x50455453); // "STEP"

                        // instrument
                        Write(step->instrument);

                        // note
                        Write(step->note);

                        // fx
                        Write(step->fx);

                        // fx amount
                        Write(step->fxAmount);
                    }
                }
            }
        }
        
        // FX CONTAINER HEADER
        Write(0x6E6F6346); // "Fcon"

        // Num lanes
        Write((uint32_t) instHandler->size());

        for (uint32_t lane = 0; lane < instHandler->size(); lane ++) {

            InstrumentHandler* handler = instHandler->at(lane);

            for (Effect* effect : handler->effects) {

                //FX HEADER
                Write(0x20205846); // "FX  "

                // FX TYPE
                Write((uint32_t) effect->effectType);

                char effectBuff[32];
                memset(effectBuff, 0, sizeof(effectBuff));
                effect->GetSnapshot(effectBuff); 

                // FX WRITE BUFFER
                for (size_t i = 0; i < sizeof(effectBuff); i++) {
                    Write(effectBuff[i]);
                }
            }
        }

        // INSTRUMENT CONTAINER HEADER
        Write(0x6E6F6349); // "Icon"

        // Num Instruments
        Write((uint32_t) instVector->size());

        if (instVector->size() != 0) {
            for (Instrument* inst : *instVector) {

                // INSTRUMENT HEADER
                Write(0x54534E49, 4); // "INST"

                // Attack
                Write(inst->GetAttack(), 4);

                // Decay
                Write(inst->GetDecay(), 4);

                // Sustain
                Write(inst->GetSustain(), 4);

                // Release
                Write(inst->GetRelease(), 4);

                // Gain
                Write(inst->GetGain(), 4);

                // Pitch
                Write(inst->GetPitch(), 4);

                // Num Slices
                Write((uint32_t) inst->GetSlices().size());

                for (double slice : inst->GetSlices()) {
                    // Slice
                    Write((float) slice);
                }

                std::string path = inst->GetPath();

                // SEARCH PATH LENGTH
                Write((uint32_t) path.length()); 

                for (int8_t chr : path) {
                    Write(chr);
                }
            }
        }

        ResetAndPush(); 
        f_close(writeFIL);
    }

}