#pragma once

#include "ImGui.h"
#include "vprint.h"
#include <imnodes.h>
#include "node_editor.h"
#include "graph.h"
#include <vector>
#include "NodeDefs.h"

//struct osc_module
//{
//    Oscillator osc_ptr;
//    float freq_num;
//    int osc_type;
//    float osc_out;
//};

struct xfader_module {
    float* input_a;
    float* input_b;
    float* mixer_amount;
    float* new_output;
};

enum XFADER_PARAM { input_a, input_b, mix_amount, new_output, kParamsXfader };
//
//enum XFADER_PARAM { modstruct, kParamsXfader };

void xfader_module_init(ImVec2 click_pos, example::Graph<Node2> &audio_graph_, std::vector<uinode2> &ui_nodes_, std::string module_name);
void xfader_module_process(std::stack<void *> &value_stack);
void xfader_module_show(const uinode2 &node, example::Graph<Node2> &audio_graph_);
