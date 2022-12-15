#include "xfader_module.h"

int push_value_node(void *input_ptr, uinode2 &ui_node, example::Graph<Node2> &graph)
{
    const Node2 input_node("value", (void*)input_ptr);
    int ui_id = graph.insert_node( input_node );
    ui_node.ui.push_back( ui_id );
    
    return ui_id;
}

void xfader_module_init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes_, std::string module_name)
{
    uinode2 ui_node;
    ui_node.type = module_name;
    ui_node.id = graph.insert_node( Node2( ui_node.type ) );

    // set up module struct
    xfader_module *xfmod = new xfader_module
    {
        new float[256](),
        new float[256](),
        NULL,
        NULL,
        0.5,
        new float[256]()
    };
    
    //push module struct as ui node
    push_value_node(xfmod, ui_node, graph);
    
    // set up io of struct
    xfmod->input_a_attr = push_value_node(xfmod->input_a, ui_node, graph);
    xfmod->input_b_attr = push_value_node(xfmod->input_b, ui_node, graph);

    for(int i = 0; i < ui_node.ui.size(); i++)
    {
        graph.insert_edge(ui_node.id, ui_node.ui[i]);
    }
    
    ui_nodes_.push_back(ui_node);

    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);


};

void xfader_module_process(std::stack<void *> &value_stack)
{
    // pop off audio input data from other modules
    float* input_a = (float*)value_stack.top();
    value_stack.pop();
    float* input_b = (float*)value_stack.top();
    value_stack.pop();
    
    // pop off module struct last
    xfader_module *xfmod = (xfader_module*)value_stack.top();
    value_stack.pop();

    float *new_output = xfmod->new_output;
    float amount = xfmod->mixer_amount;

    for(int i = 0; i < 256; i++)
    {
        new_output[i]  = (input_a[i] * amount) + (input_b[i] * (1.0-amount));
    }

    value_stack.push(new_output);
};

#define DEBUG_NODE_TITLE_BAR(name) ImNodes::BeginNodeTitleBar(); char num_str[16]; sprintf(num_str, "%s (%d,%lu)", name.c_str(), node.id, node.ui.size()); ImGui::TextUnformatted(num_str); ImNodes::EndNodeTitleBar();

void xfader_module_show(const uinode2 &node, example::Graph<Node2> &graph)
{
    xfader_module* xfmod = (xfader_module*)graph.node(node.ui[0]).value; // store struct in index zero
    
    const float node_width = 100.0f;
    ImNodes::BeginNode(node.id);

    DEBUG_NODE_TITLE_BAR(node.type);

    {
        ImNodes::BeginInputAttribute(xfmod->input_a_attr);
//                    const float label_width = ImGui::CalcTextSize("a").x;
        ImGui::TextUnformatted("a");
        ImNodes::EndInputAttribute();
    }


    {
        ImNodes::BeginInputAttribute(xfmod->input_b_attr);
//                    const float label_width = ImGui::CalcTextSize("b").x;
        ImGui::TextUnformatted("b");
        ImNodes::EndInputAttribute();
    }

    ImGui::PushItemWidth(node_width);
    ImGui::DragFloat("##hidelabel", &xfmod->mixer_amount, 0.01f, 0.f, 1.0f);
    ImGui::PushItemWidth(node_width);

    ImNodes::BeginOutputAttribute(node.id);
    ImGui::Text("out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
};
