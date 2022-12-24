#pragma once

#include "modules/module.h"
#include "midiin_module.h"
#include "Oscillator.h"

//#define a(x) x##_node
//
//#define SKETCHY_CREATE_NODE(node_name, var_name) const Node2 node_name##_node("value", (void*)var_name); ui_node.ui.push_back( graph.insert_node( a(node_name) ) );

struct oscillator_module_data
{
    Oscillator *osc_ptr;
    float freq;
    int osc_type;
    float* output;
    
    int midiin_attr;
    midiin_module_data *midiin_module_data;
};

class oscillator_module : public xmodule {
    
public:
    oscillator_module()
    {
        xmodule::type = "osc";
    };
    
    void init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes) override;
    
    void process(std::stack<void *> &value_stack) override;
    
    void show(const uinode2 &node, example::Graph<Node2> &graph) override;
    
};
