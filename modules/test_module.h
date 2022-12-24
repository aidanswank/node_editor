#pragma once

#include "ImGui.h"
#include "vprint.h"
#include "Oscillator.h"
#include <imnodes.h>
#include "node_editor.h"
#include "graph.h"
#include <vector>
#include "NodeDefs.h"
// #include "magic_enum.hpp"

// // ids of nodes that point to data
// struct TestModule
// {
//     int osc; // should rename osc_ptr or id_osc_ptr??
//     int osc_type;
//     int osc_output;
//     int freq;
// };

struct oscillator_module
{
    Oscillator osc_ptr;
    float freq_num;
    int osc_type;
    float osc_out;
};

enum PARAM { osc_ptr, freq, osc_type, osc_out, kParams };

double midi2Freq(int n);

void osc_module_init(ImVec2 click_pos, example::Graph<Node2> &audio_graph_, std::vector<uinode2> &ui_nodes_);
void osc_module_process(std::stack<void *> &value_stack);
void osc_module_show(const uinode2 &node, example::Graph<Node2> &audio_graph_);
    
//more
void osc_combo_box(const char *combo_box_name, std::vector<std::string> &item_names, int *select_choice);
