#pragma once

#include "ImGui.h"
#include "vprint.h"
#include <imnodes.h>
#include "node_editor.h"
#include "graph.h"
#include <vector>
#include "NodeDefs.h"
#include "module_helper.h"
#include "EnvelopeGenerator.h"

struct adsr_data {
    //io
    float* envelope_signal;
    int output_attr;

    EnvelopeGenerator *envelope_generator;

    float attack;
    float decay;
    float sustain;
    float release;
};

void adsr_module_init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes);
void adsr_module_process(std::stack<void *> &value_stack);
void adsr_module_show(const uinode2 &node, example::Graph<Node2> &graph);

