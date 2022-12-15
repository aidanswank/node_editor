#include "xfader_module.h"

#define SKETCHY_CREATE_NODE(node_name, var_name) const Node2 node_name##_node("value", (void*)var_name); ui_node.ui.push_back( graph.insert_node( node_name##_node ) );

int input_init(void *input_ptr, uinode2 &ui_node, example::Graph<Node2> &graph)
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
    
    //inputs
    float* input_a_ptr = new float[256]();
    float* input_b_ptr = new float[256]();
    //other
    float mix_amount = 0.5;
    float* new_output_ptr = new float[256]();
    
    xfader_module *xfmod = new xfader_module { input_a_ptr, input_b_ptr, 0, 0, mix_amount, new_output_ptr };
    
    xfmod->input_a_attr = input_init(input_a_ptr,ui_node,graph);
    xfmod->input_b_attr = input_init(input_b_ptr,ui_node,graph);
//    const Node2 input_node("value", (void*)input_b_ptr);
//    int ui_id = graph.insert_node( input_node );
//    ui_node.ui2.insert({"input_a",ui_id});
//    ui_node.ui.push_back( ui_id );
//
    xfmod->mixer_amount = mix_amount;
    xfmod->new_output = new_output_ptr;

//    SKETCHY_CREATE_NODE(mix_amount, mix_amount_ptr);
//    SKETCHY_CREATE_NODE(new_output, new_output_ptr);
    
    const Node2 xfmod_node("value", (void*)xfmod);
    ui_node.ui.push_back( graph.insert_node( xfmod_node ) );

    for(int i = 0; i < ui_node.ui.size(); i++)
    {
        graph.insert_edge(ui_node.id, ui_node.ui[i]);
    }
    
    ui_nodes_.push_back(ui_node);

    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);


};

void xfader_module_process(std::stack<void *> &value_stack)
{

//    float *new_output = (float*)value_stack.top();
//    value_stack.pop();
//
//    float *slider_ptr = (float*)value_stack.top();
//    value_stack.pop();
    
    // pop off other data inside module
    xfader_module *xfmod = (xfader_module*)value_stack.top();
    value_stack.pop();
    
    // pop off audio input data from other modules
    float* input_a = (float*)value_stack.top();
    value_stack.pop();
    float* input_b = (float*)value_stack.top();
    value_stack.pop();

//    print("size",value_stack.size(),"a",*input_a,"b",*input_b,"slide",*slider_ptr);
    float *new_output = xfmod->new_output;
    float amount = xfmod->mixer_amount;

    for(int i = 0; i < 256; i++)
    {

        new_output[i]  = (input_a[i] * amount) + (input_b[i] * (1.0-amount));
    }

    value_stack.push(new_output);
};

void xfader_module_show(const uinode2 &node, example::Graph<Node2> &graph)
{
    xfader_module* xfmod = (xfader_module*)graph.node(node.ui[XFADER_PARAM::xfmod]).value;
    
    const float node_width = 100.0f;
    ImNodes::BeginNode(node.id);

    ImNodes::BeginNodeTitleBar();
    char num_str[16];
    char name[] = "xfader";
    sprintf(num_str, "%s (%d)", name, node.id);
    ImGui::TextUnformatted(num_str);
    ImNodes::EndNodeTitleBar();

// static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
// ImGui::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));

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
//    float amount = xfmod->mixer_amount;
    ImGui::DragFloat("##hidelabel", &xfmod->mixer_amount, 0.01f, 0.f, 1.0f);
    ImGui::PushItemWidth(node_width);

    ImNodes::BeginOutputAttribute(node.id);
    ImGui::Text("out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
};
