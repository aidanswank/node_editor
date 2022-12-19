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

namespace jfilter {

void module_init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes_);
void module_process(std::stack<void *> &value_stack);
void module_show(const uinode2 &node, example::Graph<Node2> &graph);
void glue_known(node_module_base *instance);

struct jfilter_data {
    
    float* input_audio;
    int input_audio_attr;
    float* new_output;
    
    float cutoff;
    float resonance;
    CjFilter filter;
    
    float* cutoff_input;
    int cutoff_input_attr;
    
    std::string type { "jfilter" };
//    node_module_funcs::init_func jfilter_module_init;
//    node_module_funcs::process_func jfilter_module_process;
//    node_module_funcs::show_func jfilter_module_show;
//    node_module_funcs loader { "jfilter", jfilter_module_init, jfilter_module_show, jfilter_module_process };

};

}
