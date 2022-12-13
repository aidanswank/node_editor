#pragma once

#include "ImGui.h"
#include "vprint.h"
#include <imnodes.h>
#include "node_editor.h"
#include "graph.h"
#include <vector>
#include "NodeDefs.h"

void output_module_init(int &audio_root_node_id_, ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes);
void output_module_show(const uinode2 &node, example::Graph<Node2> &audio_graph_);
void output_module_process(float* &res, std::stack<void *> &value_stack);
