#include "jfilter_module.h"

void jfilter::glue_known(node_module_base *instance)
{
    instance->type = "jfilter";
    instance->init = jfilter::module_init;
    instance->show = jfilter::module_show;
    instance->process = jfilter::module_process;
};

void jfilter::module_init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes_)
{
    uinode2 ui_node;
    ui_node.type = "jfilter";
    ui_node.id = graph.insert_node( Node2( ui_node.type ) );

    // set up module struct
    jfilter::jfilter_data *jf_data = new jfilter::jfilter_data
    {
        new float[256](),
        NULL,
        new float[256](),
        0.5,
        0.2,
    };
    jf_data->cutoff_input = new float[256]();

    //push module struct as ui node
    push_value_node(jf_data, ui_node, graph);

    // set up io of struct
    jf_data->input_audio_attr = push_value_node(jf_data->input_audio, ui_node, graph);
    
    jf_data->cutoff_input_attr = push_value_node(jf_data->cutoff_input, ui_node, graph);

    for(int i = 0; i < ui_node.ui.size(); i++)
    {
        graph.insert_edge(ui_node.id, ui_node.ui[i]);
    }

    ui_nodes_.push_back(ui_node);

    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
};

void jfilter::module_process(std::stack<void *> &value_stack)
{
    // pop off audio input data from other modules
    
    float *cutoff_input = (float*)value_stack.top();
    value_stack.pop();
        
    float *input_audio = (float*)value_stack.top();
    value_stack.pop();
    
    // pop off module struct last
    jfilter::jfilter_data *jf_data = (jfilter::jfilter_data*)value_stack.top();
    value_stack.pop();
    
    float *new_output = jf_data->new_output;
    
    for(int i = 0; i < 256; i++)
    {
//        new_output[i] = *jf_data->filter.doFilter(input_audio[i], jf_data->cutoff, jf_data->resonance);
        
        float cutoff = -1;
        
        if(*cutoff_input==0) // if nothings plugged in
        {
            cutoff = jf_data->cutoff; // use ui value
        } else {
            cutoff = cutoff_input[i];
        }
        
        assert(cutoff != -1);
        
        new_output[i] = *jf_data->filter.doFilter(input_audio[i], cutoff, jf_data->resonance);
 
    }

    value_stack.push(new_output);
};

void jfilter::module_show(const uinode2 &node, example::Graph<Node2> &graph)
{
    jfilter::jfilter_data* jf_data = (jfilter::jfilter_data*)graph.node(node.ui[STRUCT_IDX]).value; // store struct in index zero
    
    const float node_width = 100;
    ImNodes::BeginNode(node.id);

    DEBUG_NODE_TITLE_BAR(node.type);

    {
        ImNodes::BeginInputAttribute(jf_data->input_audio_attr);
        ImGui::TextUnformatted("audio in");
        ImNodes::EndInputAttribute();
    }
    
    {
        ImNodes::BeginInputAttribute(jf_data->cutoff_input_attr);
        ImGui::TextUnformatted("cutoff in");
        ImNodes::EndInputAttribute();
    }

    ImGui::PushItemWidth(node_width);
//    if(*jf_data->cutoff_input==0)
//    {
    ImGui::DragFloat("cutoff", &jf_data->cutoff, 0.01f, 0.f, 1.0f);
//    }
    ImGui::DragFloat("resonance", &jf_data->resonance, 0.01f, 0.f, 1.0f);
    ImGui::PopItemWidth();

    ImNodes::BeginOutputAttribute(node.id);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
};

