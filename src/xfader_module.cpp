#include "xfader_module.h"

#define a(x) x##_node
#define SKETCHY_CREATE_NODE(node_name, var_name) const Node2 node_name##_node("value", (void*)var_name); ui_node.ui.push_back( audio_graph_.insert_node( a(node_name) ) );

void xfader_module_init(ImVec2 click_pos, example::Graph<Node2> &audio_graph_, std::vector<uinode2> &ui_nodes_, std::string module_name)
{
    uinode2 ui_node;
    ui_node.type = module_name;
    ui_node.id = audio_graph_.insert_node( Node2( ui_node.type ) );
    
    float* input_a_ptr = new float[256]();
    float* input_b_ptr = new float[256]();
    float* mix_amount_ptr = new float(0.5);
    float* new_output_ptr = new float[256]();
    xfader_module *xfmod = new xfader_module { input_a_ptr, input_b_ptr, mix_amount_ptr, new_output_ptr };

//    SKETCHY_CREATE_NODE(modstruct, xfmod);

    SKETCHY_CREATE_NODE(input_a, input_a_ptr);
    SKETCHY_CREATE_NODE(input_b, input_b_ptr);
    SKETCHY_CREATE_NODE(mix_amount, mix_amount_ptr);
    SKETCHY_CREATE_NODE(new_output, new_output_ptr);

    for(int i = 0; i < XFADER_PARAM::kParamsXfader; i++)
    {
        audio_graph_.insert_edge(ui_node.id, ui_node.ui[i]);
    }
    
    ui_nodes_.push_back(ui_node);

    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);


};

void xfader_module_process(std::stack<void *> &value_stack)
{

    float *new_output = (float*)value_stack.top();
    value_stack.pop();
    
    float *slider_ptr = (float*)value_stack.top();
    value_stack.pop();

    float* input_a = (float*)value_stack.top();
    value_stack.pop();

    float* input_b = (float*)value_stack.top();
    value_stack.pop();

//    print("size",value_stack.size(),"a",*input_a,"b",*input_b,"slide",*slider_ptr);

    float amount = *slider_ptr;

    for(int i = 0; i < 256; i++)
    {

        new_output[i]  = (input_a[i] * amount) + (input_b[i] * (1.0-amount));
    }

    value_stack.push(new_output);
};

void xfader_module_show(const uinode2 &node, example::Graph<Node2> &audio_graph_)
{
//    xfader_module* xfmod = (xfader_module*)audio_graph_.node(node.ui[XFADER_PARAM::modstruct]).value;
    
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
        ImNodes::BeginInputAttribute(node.ui[XFADER_PARAM::input_a]);
//                    const float label_width = ImGui::CalcTextSize("a").x;
        ImGui::TextUnformatted("a");
        ImNodes::EndInputAttribute();
    }


    {
        ImNodes::BeginInputAttribute(node.ui[XFADER_PARAM::input_b]);
//                    const float label_width = ImGui::CalcTextSize("b").x;
        ImGui::TextUnformatted("b");
        ImNodes::EndInputAttribute();
    }

    ImGui::PushItemWidth(node_width);
    float* amount_ptr = (float*)audio_graph_.node(node.ui[XFADER_PARAM::mix_amount]).value;
    ImGui::DragFloat("##hidelabel", &*amount_ptr, 0.01f, 0.f, 1.0f);
    ImGui::PushItemWidth(node_width);

    ImNodes::BeginOutputAttribute(node.id);
    ImGui::Text("out");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
};
