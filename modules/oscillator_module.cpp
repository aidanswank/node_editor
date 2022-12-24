#include "oscillator_module.h"

void oscillator_module::init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes)
{
    Oscillator *osc_ptr = new Oscillator();
    osc_ptr->setMode(Oscillator::OSCILLATOR_MODE_SINE);
    osc_ptr->setSampleRate(44100);
    
    uinode2 ui_node;
    ui_node.type = "osc";
    ui_node.id = graph.insert_node( Node2( ui_node.type ) );
    
    oscillator_module_data *module_data = new oscillator_module_data;
    module_data->osc_ptr = osc_ptr;
    module_data->freq = 220.0;
    module_data->osc_type = 1;
    module_data->output = new float[256]();

    push_value_node(module_data, ui_node, graph);
    
    module_data->midiin_attr = push_value_node(module_data->midiin_module_data, ui_node, graph);

//        SKETCHY_CREATE_NODE(osc_ptr, osc_ptr);
//        SKETCHY_CREATE_NODE(freq, new float(220.0));
//        SKETCHY_CREATE_NODE(osc_type, new int(1));
//        SKETCHY_CREATE_NODE(osc_out, new float[256]());

    for(int i = 0; i < ui_node.ui.size(); i++)
    {
        graph.insert_edge(ui_node.id, ui_node.ui[i]);
    }

    ui_nodes.push_back(ui_node);

    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
}

void oscillator_module::process(std::stack<void *> &value_stack)
{
    
    midiin_module_data *midiin_data = (midiin_module_data*)value_stack.top();
    value_stack.pop();
    
//    std::cout << midiin_data->notes.size() << std::endl;
//    midiin_data->notes.clear();
    
    oscillator_module_data *module_data = (oscillator_module_data*)value_stack.top();
    value_stack.pop();

//    module_data->osc_ptr->setFrequency(module_data->freq);
    if(midiin_data->notes.size()>0)
    {
        if(midiin_data->notes[0].isNoteOn)
        {
            module_data->osc_ptr->setFrequency(midi2Freq(midiin_data->notes[0].noteNum));
        }
    }
    module_data->osc_ptr->setMode((Oscillator::OscillatorMode)module_data->osc_type);

    for (int i = 0; i < 256; i++)
    {
        double osc_output = module_data->osc_ptr->nextSample();

        // float whitenoise = rand() % 100;
        // whitenoise = whitenoise / 100;
        // output[i]  = whitenoise * 0.06;

        module_data->output[i] = osc_output;
    }
    
    value_stack.push(module_data->output);
    
    midiin_data->notes.clear();
}

void oscillator_module::show(const uinode2 &node, example::Graph<Node2> &graph)
{
    
    oscillator_module_data *module_data = (oscillator_module_data*)graph.node(node.ui[STRUCT_IDX]).value; // store struct in index zero
    
    ImNodes::BeginNode(node.id);

    ImNodes::BeginNodeTitleBar();
    char num_str[16];
    char name[] = "osc gen";
    sprintf(num_str, "%s (%d)", name, node.id);
    ImGui::TextUnformatted(num_str);
    ImNodes::EndNodeTitleBar();
    
    ImNodes::BeginInputAttribute(module_data->midiin_attr);
    ImGui::Text("midi input");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(node.id);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();

    ImGui::PushItemWidth(100.f);

    static std::vector<std::string> names = {"sine", "saw", "square", "triangle"};
    static const char *current_outputDeviceName = 0;

//    float *freq_num = (float *)audio_graph_.node(node.ui.test_external.freq).value;
//        int *sc_type_num = (int *)SKETCHY_GET(PARAM::osc_type);
    // print("yooo", *sc_type_num);
//        int *sc_type_num = (int *)graph.node(node.ui[2]).value;

//    ImGui::DragFloat("freq", &*freq_num, 2.0f, 1.f, 1000.0f);
//        combo_box("osc",names,sc_type_num);

    ImGui::PopItemWidth();

    ImNodes::EndNode();
}
