#include "graph.h"
#include "NodeDefs.h"

#define STRUCT_IDX 0

#define DEBUG_NODE_TITLE_BAR(name) ImNodes::BeginNodeTitleBar(); char num_str[16]; sprintf(num_str, "%s (%d,%lu)", name.c_str(), node.id, node.ui.size()); ImGui::TextUnformatted(num_str); ImNodes::EndNodeTitleBar();

int push_value_node(void *input_ptr, uinode2 &ui_node, example::Graph<Node2> &graph);

