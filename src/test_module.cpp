#include "test_module.h"
//#include "midi_in_module.h"

//more
#include "imgui_helpers.h"


#define a(x) x##_node

#define SKETCHY_CREATE_NODE(node_name, var_name) const Node2 node_name##_node("value", (void*)var_name); ui_node.ui.push_back( graph.insert_node( a(node_name) ) );

void osc_module_init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes_, std::string module_name)
{
    print("test module init", (std::string)magic_enum::enum_name(PARAM::osc_ptr));
    
    Oscillator *osc_ptr = new Oscillator();
    osc_ptr->setMode(Oscillator::OSCILLATOR_MODE_SINE);
    osc_ptr->setSampleRate(44100);

    uinode2 ui_node;
    ui_node.type = module_name;
    ui_node.id = graph.insert_node( Node2( ui_node.type ) );
    
    SKETCHY_CREATE_NODE(osc_ptr, osc_ptr);
    SKETCHY_CREATE_NODE(freq, new float(220.0));
    SKETCHY_CREATE_NODE(osc_type, new int(0));
    SKETCHY_CREATE_NODE(osc_out, new float[256]());

    for(int i = 0; i < PARAM::kParams; i++)
    {
        graph.insert_edge(ui_node.id, ui_node.ui[i]);
    }

    ui_nodes_.push_back(ui_node);

    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
};

void osc_module_process(std::stack<void *> &value_stack)
{
    // print("sine");
    
    float *output = (float *)value_stack.top();
    value_stack.pop();

    int *osc_type_ptr = (int *)value_stack.top();
    int osc_type_num = *osc_type_ptr;
    value_stack.pop();
    // print("osc type",osc_type_num);

    float *slider_ptr = (float *)value_stack.top();
//    float freq = *slider_ptr;
//    float freq;
    value_stack.pop();
    // print("yooo",freq);

    Oscillator *osc_ptr = (Oscillator *)value_stack.top();
    // print("yooo",gain_arr[0]);
    value_stack.pop();

////    float freq2 = 0.0;
//    UserData *userdata_ptr = (UserData *)value_stack.top();
//    while (true) {
//        MidiNoteMessage note;
//        bool hasNotes = userdata_ptr->notesQueue.try_dequeue(note);
//        if (!hasNotes) {
//                break;
//        }
//        if (note.isNoteOn) {
//            std::cout << "node on " << note.noteNum << std::endl;
//            *slider_ptr = midi2Freq(note.noteNum);
//        } else {
//            std::cout << "node off" << std::endl;
//        }
//    }
//    value_stack.pop();
    
    //            float* output = new float[buffer_size]();
    osc_ptr->setFrequency(*slider_ptr);
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

#define SKETCHY_GET(x) graph.node(node.ui[x]).value

void osc_module_show(const uinode2 &node, example::Graph<Node2> &graph)
{
    ImNodes::BeginNode(node.id);

    ImNodes::BeginNodeTitleBar();
    char num_str[16];
    char name[] = "osc gen";
    sprintf(num_str, "%s (%d)", name, node.id);
    ImGui::TextUnformatted(num_str);
    ImNodes::EndNodeTitleBar();
    
//    ImNodes::BeginInputAttribute(node.ui.test_external.midiin_data);
//    ImNodes::BeginInputAttribute(GET_UI(PARAM::midi_in));
//    ImGui::Text("midi input");
//    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(node.id);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();

    ImGui::PushItemWidth(100.f);

    static std::vector<std::string> names = {"sine", "saw", "square", "triangle"};
    static const char *current_outputDeviceName = 0;

//    float *freq_num = (float *)audio_graph_.node(node.ui.test_external.freq).value;
    int *sc_type_num = (int *)SKETCHY_GET(PARAM::osc_type);
    // print("yooo", *sc_type_num);

//    ImGui::DragFloat("freq", &*freq_num, 2.0f, 1.f, 1000.0f);
    combo_box("osc",names,sc_type_num);

    ImGui::PopItemWidth();

    ImNodes::EndNode();
};
