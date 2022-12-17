#include "module_helper.h"

int push_value_node(void *input_ptr, uinode2 &ui_node, example::Graph<Node2> &graph)
{
    const Node2 input_node("value", (void*)input_ptr);
    int ui_id = graph.insert_node( input_node );
    ui_node.ui.push_back( ui_id );
    
//    print("allocating ",&input_ptr);
    
    return ui_id;
}
