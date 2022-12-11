#pragma once

#include "ImGui.h"
#include "vprint.h"
#include "Oscillator.h"
#include <imnodes.h>
#include "node_editor.h"
#include "graph.h"
#include <vector>
#include "NodeDefs.h"
#include <rtmidi/RtMidi.h>
#include <concurrentqueue/concurrentqueue.h>

struct MidiNoteMessage
{
    int noteNum = -1;
    float velocity = 0.0f;
    bool isNoteOn = false;
};

struct UserData {
        // EasyVst vst;
    uint64_t continuousSamples = 0;
    moodycamel::ConcurrentQueue<MidiNoteMessage> notesQueue;
};

void midiCallback(double deltaTime, std::vector<unsigned char> *message, void *pUserData);

void midiin_module_init(ImVec2 click_pos, example::Graph<Node> &audio_graph_, std::vector<UiNode> &ui_nodes_);
void midiin_module_process(std::stack<void *> &value_stack);
void midiin_module_show(const UiNode &node, example::Graph<Node> &audio_graph_);
