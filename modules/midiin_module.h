
#pragma once

#include "modules/module.h"
#include <rtmidi/RtMidi.h>
#include <concurrentqueue/concurrentqueue.h>
#include <cmath>

struct MidiNoteMessage
{
    int noteNum = -1;
    float velocity = 0.0f;
    bool isNoteOn = false;
};

//struct UserData {
//    // EasyVst vst;
//    uint64_t continuousSamples = 0;
//    moodycamel::ConcurrentQueue<MidiNoteMessage> notesQueue;
//};

struct midiin_module_data
{
    RtMidiIn* midiin_ptr;
    //    UserData* userdata_ptr;
//    std::vector<MidiNoteMessage> notes;
//    uint64_t continuousSamples = 0;
    moodycamel::ConcurrentQueue<MidiNoteMessage> notesQueue;
    
    int node_id;
    int num_consumers = 0;
};

double midi2Freq(int n);


void midiCallback(double deltaTime, std::vector<unsigned char> *message, void *pUserData);

class midiin_module : public xmodule {
public:
    
    midiin_module()
    {
        xmodule::type = "midi in";
    }
    
    void init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes) override;
    
    void process(std::stack<void *> &value_stack) override;
    
    void show(const uinode2 &node, example::Graph<Node2> &graph) override;
    
};
