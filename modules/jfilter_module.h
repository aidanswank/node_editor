#pragma once

#include "ImGui.h"
#include "vprint.h"
#include <imnodes.h>
#include "node_editor.h"
#include "graph.h"
#include <vector>
#include "NodeDefs.h"
#include "module_helper.h"
#include "CjFilter.h"

struct jfilter_data {
    //io
    float* input_audio;
    int input_audio_attr;
    float* new_output;
    
    float cutoff;
    float resonance;
    CjFilter filter;
    
};

void jfilter_module_init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes_, std::string module_name);
void jfilter_module_process(std::stack<void *> &value_stack);
void jfilter_module_show(const uinode2 &node, example::Graph<Node2> &graph);

