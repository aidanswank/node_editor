#pragma once

#include "ImGui.h"
#include "vprint.h"
#include <imnodes.h>
#include "node_editor.h"
#include "graph.h"
#include <vector>
#include "NodeDefs.h"
#include "module_helper.h"

//struct osc_module
//{
//    Oscillator osc_ptr;
//    float freq_num;
//    int osc_type;
//    float osc_out;
//};

struct xfader_module {
    //inputs
    float* input_a;
    float* input_b;
    int input_a_attr;
    int input_b_attr;
    
    //other data
    float mixer_amount;
    float* new_output;    
};

void xfader_module_init(ImVec2 click_pos, example::Graph<Node2> &audio_graph_, std::vector<uinode2> &ui_nodes_);
void xfader_module_process(std::stack<void *> &value_stack);
void xfader_module_show(const uinode2 &node, example::Graph<Node2> &audio_graph_);
