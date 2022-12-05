#include "test_module.h"

void test_module::init_module(ImVec2 click_pos, example::Graph<AudioNode> &audio_graph_, std::vector<UiNode> &ui_nodes_)
{
    print("test module init");

    float *osc_output = new float[256]();

    Oscillator *osc = new Oscillator();
    osc->setMode(Oscillator::OSCILLATOR_MODE_SINE);
    osc->setSampleRate(44100);

    float *freq_num = new float(440.0f);

    const AudioNode osc_node(NodeType::value, (void *)osc);
    const AudioNode freq_node(NodeType::value, (void *)freq_num);
    const AudioNode osc_type_node(NodeType::value, (void *)new int(0)); // type 0 sine
    const AudioNode osc_out_node(NodeType::value, (void *)osc_output);

    UiNode audio_ui_node;
    audio_ui_node.type                          = NodeType::test_external;
    audio_ui_node.id                            = audio_graph_.insert_node( AudioNode( audio_ui_node.type ) );
    audio_ui_node.ui.test_external.osc          = audio_graph_.insert_node( osc_node );
    audio_ui_node.ui.test_external.freq         = audio_graph_.insert_node( freq_node );
    audio_ui_node.ui.test_external.osc_type     = audio_graph_.insert_node( osc_type_node );
    audio_ui_node.ui.test_external.osc_output   = audio_graph_.insert_node( osc_out_node );

    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.osc);
    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.freq);
    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.osc_type);
    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.osc_output);
    //
    ui_nodes_.push_back(audio_ui_node);

    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos);
};

void test_module::process_module(std::stack<void *> &value_stack)
{
    // print("sine");

    float *output = (float *)value_stack.top();
    value_stack.pop();

    int *osc_type_ptr = (int *)value_stack.top();
    int osc_type_num = *osc_type_ptr;
    value_stack.pop();
    // print("osc type",osc_type_num);

    float *slider_ptr = (float *)value_stack.top();
    float freq = *slider_ptr;
    value_stack.pop();
    // print("yooo",freq);

    Oscillator *osc_ptr = (Oscillator *)value_stack.top();
    // print("yooo",gain_arr[0]);
    value_stack.pop();

    //            float* output = new float[buffer_size]();

    osc_ptr->setFrequency(freq);
    osc_ptr->setMode((Oscillator::OscillatorMode)osc_type_num);

    for (int i = 0; i < 256; i++)
    {
        double osc_output = osc_ptr->nextSample();

        // float whitenoise = rand() % 100;
        // whitenoise = whitenoise / 100;
        // output[i]  = whitenoise * 0.06;

        output[i] = osc_output;
    }
    
    value_stack.push(output);
    
};

void test_module::show_module(const UiNode &node, example::Graph<AudioNode> &audio_graph_)
{
    ImNodes::BeginNode(node.id);

    ImNodes::BeginNodeTitleBar();
    char num_str[16];
    char name[] = "osc gen";
    sprintf(num_str, "%s (%d)", name, node.id);
    ImGui::TextUnformatted(num_str);
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginOutputAttribute(node.id);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();

    ImGui::PushItemWidth(100.f);

    static std::vector<std::string> names = {"sine", "saw", "square", "triangle"};
    static const char *current_outputDeviceName = 0;

    float *freq_num = (float *)audio_graph_.node(node.ui.sine.freq).value;
    int *sc_type_num = (int *)audio_graph_.node(node.ui.sine.osc_type).value;
    test_module::combo_box("osc type", names, sc_type_num);
    // print("yooo", *sc_type_num);

    ImGui::DragFloat("freq", &*freq_num, 2.0f, 1.f, 1000.0f);

    ImGui::PopItemWidth();

    ImNodes::EndNode();
};
