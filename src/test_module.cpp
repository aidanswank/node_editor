#include "test_module.h"
//#include "midi_in_module.h"

enum PARAM { osc, freq, osc_type, output, kParams };

//more
void osc_combo_box(const char *combo_box_name, std::vector<std::string> &item_names, int *select_choice)
{
    const char *current_name = item_names[*select_choice].c_str();

    if (ImGui::BeginCombo(combo_box_name, current_name)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < item_names.size(); n++)
        {
            bool is_selected = (current_name == item_names[n].c_str()); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(item_names[n].c_str(), is_selected))
            {
                current_name = item_names[n].c_str();
                print(current_name, "selected", n);
                *select_choice = n;
                // audioInterface->turnDeviceOn( audioInterface->openDevice(n,0) );
            }
            // if (is_selected)
            // {
            //     print("is selected");
            //     // ImGui::SetItemDefaultFocus(); // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            // }
        }
        ImGui::EndCombo();
    }
};


void osc_module_init(ImVec2 click_pos, example::Graph<Node2> &audio_graph_, std::vector<uinode2> &ui_nodes_, std::string module_name)
{
    print("test module init");

    float *osc_output = new float[256]();

    Oscillator *osc = new Oscillator();
    osc->setMode(Oscillator::OSCILLATOR_MODE_SINE);
    osc->setSampleRate(44100);

    float *freq_num = new float(220.0);

    const Node2 osc_node("value", (void *)osc);
    const Node2 freq_node("value", (void *)freq_num);
    const Node2 osc_type_node("value", (void *)new int(0)); // type 0 sine
    const Node2 osc_out_node("value", (void *)osc_output);
    
//    UserData* midiin_userdata_ptr = new UserData;
//    const Node midiin_data_node(NodeType::value, midiin_userdata_ptr);

    uinode2 ui_node;
    ui_node.type = module_name;
    ui_node.id = audio_graph_.insert_node( Node2( ui_node.type ) );
//    ui_node.ui.push_back( audio_graph_.insert_node( midiin_data_node ) );
    ui_node.ui.push_back( audio_graph_.insert_node( osc_node ) );
    ui_node.ui.push_back( audio_graph_.insert_node( freq_node ) );
    ui_node.ui.push_back( audio_graph_.insert_node( osc_type_node ) );
    ui_node.ui.push_back( audio_graph_.insert_node( osc_out_node ) );
    
    for(int i = 0; i < PARAM::kParams; i++)
    {
        audio_graph_.insert_edge(ui_node.id, ui_node.ui[i]);
    }
//    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui[0]);
//    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui[1]);
//    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui[2]);
//    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui[3]);
//    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui[4]);

    
//    UiNode audio_ui_node;
//    audio_ui_node.type                          = NodeType::test_external;
//    audio_ui_node.id                            = audio_graph_.insert_node( Node( audio_ui_node.type ) );
//    audio_ui_node.ui.test_external.midiin_data  = audio_graph_.insert_node( midiin_data_node );
//    audio_ui_node.ui.test_external.osc          = audio_graph_.insert_node( osc_node );
//    audio_ui_node.ui.test_external.freq         = audio_graph_.insert_node( freq_node );
//    audio_ui_node.ui.test_external.osc_type     = audio_graph_.insert_node( osc_type_node );
//    audio_ui_node.ui.test_external.osc_output   = audio_graph_.insert_node( osc_out_node );
//
//    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.test_external.midiin_data );
//    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.test_external.osc);
//    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.test_external.freq);
//    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.test_external.osc_type);
//    audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.test_external.osc_output);

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

#define GET_UI(x) node.ui[x]
void osc_module_show(const uinode2 &node, example::Graph<Node2> &audio_graph_)
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
    int *sc_type_num = (int *)audio_graph_.node(GET_UI(PARAM::osc_type)).value;
    osc_combo_box("osc type", names, sc_type_num);
    // print("yooo", *sc_type_num);

//    ImGui::DragFloat("freq", &*freq_num, 2.0f, 1.f, 1000.0f);

    ImGui::PopItemWidth();

    ImNodes::EndNode();
};
