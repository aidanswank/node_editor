#include "node_editor.h"
#include "graph.h"
#include "Oscillator.h"
#include "EasyVst/EasyVst.h"

#include <imnodes.h>
#include <imgui.h>

#include <SDL_keycode.h>
#include <SDL_timer.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <vector>
#include "vprint.h"

int buffer_size = 256;

#include "NodeDefs.h"

#include "test_module.h"
#include "midi_in_module.h"

namespace example
{
namespace
{

template<class T>
T clamp(T x, T a, T b)
{
    return std::min(b, std::max(x, a));
}

static float current_time_seconds = 0.f;
static bool  emulate_three_button_mouse = false;

#include "audio_evaluate.h"

void combo_box(const char *combo_box_name, std::vector<std::string> &item_names, int* select_choice)
{
    const char * current_name = item_names[*select_choice].c_str();

    if (ImGui::BeginCombo(combo_box_name, current_name)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < item_names.size(); n++)
        {
            bool is_selected = (current_name == item_names[n].c_str()); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(item_names[n].c_str(), is_selected))
            {
                current_name = item_names[n].c_str();
                print(current_name,"selected",n);
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

class ColorNodeEditor
{
public:
    ColorNodeEditor()
        : graph(), ui_nodes(), root_node_id_(-1),
          minimap_location_(ImNodesMiniMapLocation_BottomRight)
    {
        audio_root_node_id_ = -1;
        printf("color node init\n");
    }

    void show()
    {
        // Update timer context
        current_time_seconds = 0.001f * SDL_GetTicks();

        auto flags = ImGuiWindowFlags_MenuBar;

        // The node editor window
        ImGui::Begin("color node editor", NULL, flags);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Mini-map"))
            {
                const char* names[] = {
                    "Top Left",
                    "Top Right",
                    "Bottom Left",
                    "Bottom Right",
                };
                int locations[] = {
                    ImNodesMiniMapLocation_TopLeft,
                    ImNodesMiniMapLocation_TopRight,
                    ImNodesMiniMapLocation_BottomLeft,
                    ImNodesMiniMapLocation_BottomRight,
                };

                for (int i = 0; i < 4; i++)
                {
                    bool selected = minimap_location_ == locations[i];
                    if (ImGui::MenuItem(names[i], NULL, &selected))
                        minimap_location_ = locations[i];
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Style"))
            {
                if (ImGui::MenuItem("Classic"))
                {
                    ImGui::StyleColorsClassic();
                    ImNodes::StyleColorsClassic();
                }
                if (ImGui::MenuItem("Dark"))
                {
                    ImGui::StyleColorsDark();
                    ImNodes::StyleColorsDark();
                }
                if (ImGui::MenuItem("Light"))
                {
                    ImGui::StyleColorsLight();
                    ImNodes::StyleColorsLight();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // ImGui::TextUnformatted("Edit the color of the output color window using nodes.");
        // ImGui::Columns(2);
        // ImGui::TextUnformatted("A -- add node");
        // ImGui::TextUnformatted("X -- delete selected node or link");
        // ImGui::NextColumn();
        // if (ImGui::Checkbox("emulate_three_button_mouse", &emulate_three_button_mouse))
        // {
        //     ImNodes::GetIO().EmulateThreeButtonMouse.Modifier =
        //         emulate_three_button_mouse ? &ImGui::GetIO().KeyAlt : NULL;
        // }
        // ImGui::Columns(1);

        ImNodes::BeginNodeEditor();

        // Handle new nodes
        // These are driven by the user, so we place this code before rendering the nodes
        {
            const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                                    ImNodes::IsEditorHovered() &&
                                    ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_A);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
            if (!ImGui::IsAnyItemHovered() && open_popup)
            {
                ImGui::OpenPopup("add node");
            }

            if (ImGui::BeginPopup("add node"))
            {
                const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

                
                for(int i = 0; i < node_types.size(); i++)
                {
                    if (ImGui::MenuItem(node_types[i].c_str()))
                    {
//                        midiin_module_init(click_pos, graph, ui_nodes);
                    }
                }
                
//                if (ImGui::MenuItem("RtMidi Input"))
//                {
//                    midiin_module_init(click_pos, graph, ui_nodes);
//                }
//
                // TEST EXTERNAL
                if (ImGui::MenuItem("TEST_EXTERNAL"))
                {
                    test_module::init_module(click_pos, graph, ui_nodes2);
                }
////
//                if(ImGui::MenuItem("AudioInterfaceIn"))
//                {
//                    UiNode audio_ui_node;
//                    audio_ui_node.type = NodeType::interface_in;
//                    audio_ui_node.id = graph.insert_node( Node(NodeType::interface_in) );
//                    // audio_ui_node.ui.sine.osc = audio_graph_.insert_node( osc_node ); // id of node
//
//                    // audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.osc);
//
//                    ui_nodes.push_back(audio_ui_node);
//
//                    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos);
//                }
//
//                if(ImGui::MenuItem("AudioVST3Plug"))
//                {
//                    EasyVst *vst = new EasyVst();
//
//                    /////////////// vst setup TODO CLEAN !!!!
//
//                    if (!vst->init("/Library/Audio/Plug-Ins/VST3/Portal.vst3", 44100, 256, Steinberg::Vst::kSample32, true))
//                    {
//                        std::cerr << "Failed to initialize VST" << std::endl;
//                        // return 1;
//                    }
//
//                    int numEventInBuses = vst->numBuses(Steinberg::Vst::kAudio, Steinberg::Vst::kInput);
//                    int numAudioOutBuses = vst->numBuses(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput);
//                    print("num input buses", numEventInBuses, "out buses", numAudioOutBuses);
//                    if (numEventInBuses < 1 || numAudioOutBuses < 1)
//                    {
//                        std::cerr << "Incorrect bus configuration" << std::endl;
//                        // return 1;
//                    }
//
//                    const Steinberg::Vst::BusInfo *outBusInfo = vst->busInfo(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput, 0);
//                    if (outBusInfo->channelCount != 2)
//                    {
//                        std::cerr << "Invalid output channel configuration" << std::endl;
//                        // return 1;
//                    }
//
//                    vst->setBusActive(Steinberg::Vst::kAudio, Steinberg::Vst::kInput, 0, true);
//                    vst->setBusActive(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput, 0, true);
//                    vst->setProcessing(true);
//
//                    if (!vst->createView())
//                    {
//                        std::cerr << "Failed to create VST view" << std::endl;
//                        // return 1;
//                    }
//
//                    ////////////// end setup
//
//                    float* mono_buf = new float[256]();
//
//                    const Node vst_node(NodeType::value, (void*)vst);
//                    const Node input_buf_node(NodeType::value, (void*)mono_buf);
//
//                    UiNode ui_node;
//                    ui_node.type                = NodeType::vst;
//                    ui_node.id                  = graph.insert_node( Node(NodeType::vst) );
//                    ui_node.ui.vst.vst_obj      = graph.insert_node( vst_node ); // id of node
//                    ui_node.ui.vst.input_buf    = graph.insert_node( input_buf_node ); // id of node
//
//                    graph.insert_edge(ui_node.id, ui_node.ui.vst.vst_obj);
//                    graph.insert_edge(ui_node.id, ui_node.ui.vst.input_buf);
//
//                    ui_nodes.push_back(ui_node);
//
//                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
//                }
//
//                // AUDIO SINE
//                if (ImGui::MenuItem("AudioOsc"))
//                {
////                    print("something happens");
//                    // float* arr = new float[buffer_size]();
//                    // const AudioNode value(AudioNodeType::value, arr);
//                    // const AudioNode op(AudioNodeType::sine);
//
//                    float *osc_output = new float[buffer_size]();
//
//                    Oscillator *osc_ptr = new Oscillator();
//                    osc_ptr->setMode(Oscillator::OSCILLATOR_MODE_SAW);
//                    osc_ptr->setSampleRate(44100);
//
//                    float *freq_num = new float(440.0f);
//                    // *freq_num = 440.0f;
//                    const Node osc_node(NodeType::value, (void*)osc_ptr);
//                    const Node freq_node(NodeType::value, (void*)freq_num);
//
//                    const Node osc_type_node(NodeType::value, (void*)new int(0)); //0 = sine mode
//                    const Node osc_out_node(NodeType::value, (void*)osc_output);
//
//                    UiNode audio_ui_node;
//                    audio_ui_node.type               = NodeType::sine;
//                    audio_ui_node.id                 = graph.insert_node( Node(NodeType::sine) );
//                    audio_ui_node.ui.sine.osc        = graph.insert_node( osc_node ); // id of node
//                    audio_ui_node.ui.sine.freq       = graph.insert_node( freq_node ); // id of node
//                    audio_ui_node.ui.sine.osc_type   = graph.insert_node( osc_type_node ); // id of node
//                    audio_ui_node.ui.sine.osc_output = graph.insert_node( osc_out_node ); // id of node
//
//                    graph.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.osc);
//                    graph.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.freq);
//                    graph.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.osc_type);
//                    graph.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.osc_output);
//
//                    ui_nodes.push_back(audio_ui_node);
//
//                    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos);
//                }
//
//                if (ImGui::MenuItem("AudioWhiteNoise"))
//                {
//
//                    UiNode audio_ui_node;
//                    audio_ui_node.type = NodeType::white;
//                    audio_ui_node.id = graph.insert_node( Node(NodeType::white) );
//                    // audio_ui_node.ui.sine.osc = audio_graph_.insert_node( osc_node ); // id of node
//
//                    // audio_graph_.insert_edge(audio_ui_node.id, audio_ui_node.ui.sine.osc);
//
//                    ui_nodes.push_back(audio_ui_node);
//
//                    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos);
//                }
//
//                if (ImGui::MenuItem("AudioXfader"))
//                {
//                    float* arr = new float[buffer_size]();
//                    float* amount_ptr = new float(0.5);
//                    // print(*amount_ptr);
//                    const Node value(NodeType::value, arr);
//                    const Node amount(NodeType::value, amount_ptr);
//                    const Node op(NodeType::xfader);
//
//                    UiNode ui_node;
//                    ui_node.type = NodeType::xfader;
//                    ui_node.ui.xfader.input_a = graph.insert_node(value);
//                    ui_node.ui.xfader.input_b = graph.insert_node(value);
//                    ui_node.ui.xfader.amount = graph.insert_node(amount);
//                    ui_node.id = graph.insert_node(op);
//
//                    graph.insert_edge(ui_node.id, ui_node.ui.xfader.input_a);
//                    graph.insert_edge(ui_node.id, ui_node.ui.xfader.input_b);
//                    graph.insert_edge(ui_node.id, ui_node.ui.xfader.amount);
//
//                    ui_nodes.push_back(ui_node);
//                    ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
//                }

                if (ImGui::MenuItem("AudioOutput") && audio_root_node_id_ == -1)
                {
                    print("audio ouput node");
                    float* arr = new float[buffer_size]();
                    const Node value(NodeType::value, arr);
                    float* gain_ptr = new float(0.123);
                    const Node gain(NodeType::value, gain_ptr);
                    const Node out(NodeType::output);

                    uinode2 audio_ui_node;
                    audio_ui_node.type = NodeType::output;
                    audio_ui_node.ui.push_back( graph.insert_node(value) );
                    audio_ui_node.ui.push_back( graph.insert_node(gain) ); // data storage
                    audio_ui_node.id = graph.insert_node(out);

                    
//                    UiNode audio_ui_node;
//                    audio_ui_node.type = NodeType::output;
//                    audio_ui_node.ui.output.input = graph.insert_node(value);
//                    audio_ui_node.ui.output.gain = graph.insert_node(gain); // data storage
//                    audio_ui_node.id = graph.insert_node(out);

                    graph.insert_edge(audio_ui_node.id, audio_ui_node.ui[0]);
                    graph.insert_edge(audio_ui_node.id, audio_ui_node.ui[1]);

                    ui_nodes2.push_back(audio_ui_node);
                    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos);
                    audio_root_node_id_ = audio_ui_node.id;
                }

//                if (ImGui::MenuItem("WaveViewer"))
//                {
//                    print("Waveviewer insert");
//                    float* arr = new float[buffer_size]();
//                    arr[0] = 0.0;
//                    arr[1] = 1.0;
//                    arr[2] = 0.5;
//                    arr[3] = 0.25;
//
//                    const Node value(NodeType::value, arr);
//
//                    const Node view_buf_node(NodeType::value, new float[buffer_size]());
//
//                    const Node op(NodeType::waveviewer);
//
//                    UiNode audio_ui_node;
//                    audio_ui_node.type = NodeType::waveviewer;
//                    audio_ui_node.ui.waveviewer.input = graph.insert_node(value);
//                    audio_ui_node.ui.waveviewer.view_buf = graph.insert_node(view_buf_node);
//                    audio_ui_node.id = graph.insert_node(op);
//
////                    audio_graph_.insert_edge(audio_ui_node.ui.waveviewer.input, audio_root_node_id_);
//
//                    graph.insert_edge(audio_ui_node.id, audio_ui_node.ui.waveviewer.input);
//                    graph.insert_edge(audio_ui_node.id, audio_ui_node.ui.waveviewer.view_buf);
//
////                    print("root node", audio_root_node_id_);
//
//                    ui_nodes.push_back(audio_ui_node);
//                    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos);
//
//                    for(int i = 0; i < ui_nodes.size(); i++)
//                    {
//                        if(ui_nodes[i].type==NodeType::output)
//                        {
//                            graph.insert_edge(ui_nodes[i].ui.output.input, audio_ui_node.id);
//                        }
//                    }
////                    graph.insert_edge(ui_nodes[0].ui.output.input, audio_ui_node.id);
////                    print("'output' node input",ui_nodes[0].ui.output.input, "wv 'audio_ui_node.id'",audio_ui_node.id);
////                    print("waveviewer audio_root_node_id_",audio_root_node_id_,"audio_ui_node.id",audio_ui_node.id,"audio_ui_node.ui.waveviewer.input",audio_ui_node.ui.waveviewer.input);
//
//                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();
        }

        for (const uinode2& node : ui_nodes2)
        {
            switch (node.type)
            {
                case NodeType::test_external:
                {
                    test_module::show_module(node, graph);
                }
                break;
                case NodeType::output:
                {
                    const float node_width = 100.0f;
    
                    ImNodes::BeginNode(node.id);
    
                    ImNodes::BeginNodeTitleBar();
                    char num_str[16], name[] = "output";
                    sprintf(num_str, "%s (%d)", name, node.id);
                    ImGui::TextUnformatted(num_str);
                    ImNodes::EndNodeTitleBar();
    
                    ImNodes::BeginInputAttribute(node.ui[0]);
                    ImGui::Text("input");
                    ImNodes::EndInputAttribute();
    
                    ImGui::PushItemWidth(node_width);
                    // // if (audio_graph_.num_edges_from_node(node.ui.output.gain) == 0ull)
                    // // {
                    //     print("value!!",audio_graph_.node(node.ui.output.gain).value[0]);
                    // // }
                    float* fa_gain_stupid = (float*)graph.node(node.ui[1]).value;
                    ImGui::DragFloat("gain", &*fa_gain_stupid, 0.01f, 0.f, 1.0f);
    
    
                    // float* float_arr = (float*)audio_graph_.node(node.ui.output.input).value;
                    // for(int i = 0; i < 256; i++)
                    // {
                    //     print(float_arr[i]);
                    // }
                    // ImGui::PlotLines("##hidelabel", float_arr, 256);
    
                    ImGui::PopItemWidth();
    
                    ImNodes::EndNode();
                }
                break;
            }
        }
        
//        for (const UiNode& node : ui_nodes)
//        {
//            switch (node.type)
//            {
//            case NodeType::midi_in:
//            {
//                midiin_module_show(node, graph);
//            }
//            break;
//            case NodeType::test_external:
//            {
//                test_module::show_module(node, graph);
//            }
//            break;
//            case NodeType::interface_in:
//            {
//                ImNodes::BeginNode(node.id);
//
//                ImNodes::BeginNodeTitleBar();
//                char num_str[16];
//                char name[] = "interface in";
//                sprintf(num_str, "%s (%d)", name, node.id);
//                ImGui::TextUnformatted(num_str);
//                ImNodes::EndNodeTitleBar();
//
////                static std::vector<std::string> names = {"sine","saw","square","triangle"};
////                static const char *current_outputDeviceName = 0;
////
////                int* sc_type_num = (int*)    audio_graph_.node(node.ui.sine.osc_type).value;
////                comboBox("osc type", names, sc_type_num);
//
////                ImNodes::BeginInputAttribute(node.ui.interface_in.input_buf);
////                ImGui::TextUnformatted("input");
////                ImNodes::EndInputAttribute();
//
//                ImNodes::BeginOutputAttribute(node.id);
//                ImGui::Text("chan 1");
//                ImNodes::EndOutputAttribute();
//
//                ImNodes::EndNode();
//            }
//            break;
//            case NodeType::vst:
//            {
//                ImNodes::BeginNode(node.id);
//
//                ImNodes::BeginNodeTitleBar();
//                char num_str[16];
//                char name[] = "vst dummy";
//                sprintf(num_str, "%s (%d)", name, node.id);
//                ImGui::TextUnformatted(num_str);
//                ImNodes::EndNodeTitleBar();
//
//
//                ImNodes::BeginInputAttribute(node.ui.vst.input_buf);
//                ImGui::TextUnformatted("input");
//                ImNodes::EndInputAttribute();
//
//                ImNodes::BeginOutputAttribute(node.id);
//                ImGui::Text("output");
//                ImNodes::EndOutputAttribute();
//
//                ImGui::PushItemWidth(100.f);
//
//                ImGui::PopItemWidth();
//
//                ImNodes::EndNode();
//            }
//            break;
//            case NodeType::sine:
//            {
//                ImNodes::BeginNode(node.id);
//
//                ImNodes::BeginNodeTitleBar();
//                char num_str[16];
//                char name[] = "osc gen";
//                sprintf(num_str, "%s (%d)", name, node.id);
//                ImGui::TextUnformatted(num_str);
//                ImNodes::EndNodeTitleBar();
//
//                ImNodes::BeginOutputAttribute(node.id);
//                ImGui::Text("output");
//                ImNodes::EndOutputAttribute();
//
//                ImGui::PushItemWidth(100.f);
//
//                static std::vector<std::string> names = {"sine","saw","square","triangle"};
//                static const char *current_outputDeviceName = 0;
//
//                float*  freq_num    = (float*)  graph.node(node.ui.sine.freq).value;
//                int*    sc_type_num = (int*)    graph.node(node.ui.sine.osc_type).value;
//                combo_box("osc type", names, sc_type_num);
//                // print("yooo", *sc_type_num);
//
//                ImGui::DragFloat("freq", &*freq_num, 2.0f, 1.f, 1000.0f);
//
//                ImGui::PopItemWidth();
//
//                ImNodes::EndNode();
//            }
//            break;
//            case NodeType::white:
//            {
//                ImNodes::BeginNode(node.id);
//
//                ImNodes::BeginNodeTitleBar();
//                char num_str[16];
//                char name[] = "white noise";
//                sprintf(num_str, "%s (%d)", name, node.id);
//                ImGui::TextUnformatted(num_str);
//                ImNodes::EndNodeTitleBar();
//
//                ImNodes::BeginOutputAttribute(node.id);
//                ImGui::Text("output");
//                ImNodes::EndOutputAttribute();
//
//                ImNodes::EndNode();
//            }
//            break;
//            case NodeType::waveviewer:
//            {
//                ImNodes::BeginNode(node.id);
//
//                ImNodes::BeginNodeTitleBar();
//                char num_str[16];
//                char name[] = "wave viewer";
//                sprintf(num_str, "%s (%d)", name, node.id);
//                ImGui::TextUnformatted(num_str);
//                ImNodes::EndNodeTitleBar();
//
//                ImNodes::BeginInputAttribute(node.ui.waveviewer.input);
//                ImGui::Text("input");
//                ImNodes::EndOutputAttribute();
//
//                ImGui::PushItemWidth(150.f);
//                float* float_arr = (float*)graph.node(node.ui.waveviewer.view_buf).value;
////                 for(int i = 0; i < buffer_size; i++)
////                 {
////                     print(float_arr[i]);
////                 }
//
//                ImGui::PlotLines("##hidelabel", float_arr, 256);
//                ImGui::PopItemWidth();
//
//                ImNodes::BeginOutputAttribute(node.id);
//                ImGui::Text("out");
//                ImNodes::EndOutputAttribute();
//
//                ImNodes::EndNode();
//            }
//            break;
//            case NodeType::xfader:
//            {
//                const float node_width = 100.0f;
//                ImNodes::BeginNode(node.id);
//
//                ImNodes::BeginNodeTitleBar();
//                char num_str[16];
//                char name[] = "xfader";
//                sprintf(num_str, "%s (%d)", name, node.id);
//                ImGui::TextUnformatted(num_str);
//                ImNodes::EndNodeTitleBar();
//
//            // static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
//            // ImGui::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
//
//                {
//                    ImNodes::BeginInputAttribute(node.ui.xfader.input_a);
////                    const float label_width = ImGui::CalcTextSize("a").x;
//                    ImGui::TextUnformatted("a");
//                    ImNodes::EndInputAttribute();
//                }
//
//
//                {
//                    ImNodes::BeginInputAttribute(node.ui.xfader.input_b);
////                    const float label_width = ImGui::CalcTextSize("b").x;
//                    ImGui::TextUnformatted("b");
//                    ImNodes::EndInputAttribute();
//                }
//
//                ImGui::PushItemWidth(node_width);
//                float* amount_ptr = (float*)graph.node(node.ui.xfader.amount).value;
//                ImGui::DragFloat("##hidelabel", &*amount_ptr, 0.01f, 0.f, 1.0f);
//                ImGui::PushItemWidth(node_width);
//
//                ImNodes::BeginOutputAttribute(node.id);
//                ImGui::Text("out");
//                ImNodes::EndOutputAttribute();
//
//                ImNodes::EndNode();
//
//            }
//            break;
//            case NodeType::output:
//            {
//                const float node_width = 100.0f;
//
//                ImNodes::BeginNode(node.id);
//
//                ImNodes::BeginNodeTitleBar();
//                char num_str[16], name[] = "output";
//                sprintf(num_str, "%s (%d)", name, node.id);
//                ImGui::TextUnformatted(num_str);
//                ImNodes::EndNodeTitleBar();
//
//                ImNodes::BeginInputAttribute(node.ui.output.input);
//                ImGui::Text("input");
//                ImNodes::EndInputAttribute();
//
//                ImGui::PushItemWidth(node_width);
//                // // if (audio_graph_.num_edges_from_node(node.ui.output.gain) == 0ull)
//                // // {
//                //     print("value!!",audio_graph_.node(node.ui.output.gain).value[0]);
//                // // }
//                float* fa_gain_stupid = (float*)graph.node(node.ui.output.gain).value;
//                ImGui::DragFloat("gain", &*fa_gain_stupid, 0.01f, 0.f, 1.0f);
//
//
//                // float* float_arr = (float*)audio_graph_.node(node.ui.output.input).value;
//                // for(int i = 0; i < 256; i++)
//                // {
//                //     print(float_arr[i]);
//                // }
//                // ImGui::PlotLines("##hidelabel", float_arr, 256);
//
//                ImGui::PopItemWidth();
//
//                ImNodes::EndNode();
//            }
//            break;
//            }
//        }

        for (const auto& edge : graph.edges())
        {
            
            // audio_graph_.node(edge.from).type
            // If edge doesn't start at value, then it's an internal edge, i.e.
            // an edge which links a node's operation to its input. We don't
            // want to render node internals with visible links.
            // print("AUDIO NODE TYPE as INT",  (int)AudioNodeType::value);
            // print(  
            //         "edgeid",edge.id,
            //         "fromtype", (int)audio_graph_.node(edge.from).type,
            //         "fromval", audio_graph_.node(edge.from).value,
            //         "totype", (int)audio_graph_.node(edge.to).type,
            //         "toval", audio_graph_.node(edge.to).value
            //     );
            if (graph.node(edge.from).type == NodeType::value){
                // print("skipped?");
                // continue; // skip 1 iteration of the loop, not linking below
                ImNodes::Link(edge.id, edge.from, edge.to);
            }

        }

        ImNodes::MiniMap(0.2f, minimap_location_);
        ImNodes::EndNodeEditor();

        // // Handle new links
        // // These are driven by Imnodes, so we place the code after EndNodeEditor().

        // audio node linking
        {
            int start_attr, end_attr;
            if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
            {
                // print("hey!!!",start_attr, end_attr);
                const NodeType start_type = graph.node(start_attr).type;
                const NodeType end_type = graph.node(end_attr).type;

                const bool valid_link = start_type != end_type;
                if (valid_link)
                {
                    // Ensure the edge is always directed from the value to
                    // whatever produces the value
                    if (start_type != NodeType::value)
                    {
                        std::swap(start_attr, end_attr);
                        print("swapped?", start_attr, end_attr);
                    }
                    print("is valid link", start_attr, end_attr);
                    graph.insert_edge(start_attr, end_attr);
                }
            }
        }


        // // Handle deleted links

        {
            int link_id;
            if (ImNodes::IsLinkDestroyed(&link_id))
            {
                graph.erase_edge(link_id);
            }
        }

        // audio nodes deleting links
        {
            const int num_selected = ImNodes::NumSelectedLinks();
            if (num_selected > 0 && ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_X))
            {
                static std::vector<int> selected_links;
                selected_links.resize(static_cast<size_t>(num_selected));
                ImNodes::GetSelectedLinks(selected_links.data());
                for (const int edge_id : selected_links)
                {
                    graph.erase_edge(edge_id);
                }
            }
        }

        {
            const int num_selected = ImNodes::NumSelectedNodes();
            if (num_selected > 0 && ImGui::IsKeyReleased((ImGuiKey)SDL_SCANCODE_X))
            {
                static std::vector<int> selected_nodes;
                selected_nodes.resize(static_cast<size_t>(num_selected));
                ImNodes::GetSelectedNodes(selected_nodes.data());
                for (const int node_id : selected_nodes)
                {
                    graph.erase_node(node_id);
                    auto iter = std::find_if(
                        ui_nodes.begin(), ui_nodes.end(), [node_id](const UiNode& node) -> bool {
                            return node.id == node_id;
                        });
                    // Erase any additional internal nodes
                    switch (iter->type)
                    {
                    case NodeType::output:
                        graph.erase_node(iter->ui.output.input);
                        audio_root_node_id_ = -1;
                        break;
                    default:
                        break;
                    }
                    ui_nodes.erase(iter);
                }
            }
        }

        ImGui::End();

        // The color output window

        // const ImU32 color =
        //     root_node_id_ != -1 ? evaluate(graph_, root_node_id_) : IM_COL32(255, 20, 147, 255);
    
        // ImU32 color;
        // // print("root node", root_node_id_);
        // if(root_node_id_ != -1)
        // {
        //     // color = evaluate(graph_, root_node_id_);
        // } else {
        //     color = IM_COL32(255, 20, 147, 255); // default out
        // }

        // ImGui::PushStyleColor(ImGuiCol_WindowBg, color);
        // ImGui::Begin("output color");
        // ImGui::End();
        // ImGui::PopStyleColor();
    }

    Graph<Node> graph;
    std::vector<UiNode> ui_nodes;
    std::vector<uinode2> ui_nodes2;
    std::vector<std::string> node_types;
    int root_node_id_;
    int audio_root_node_id_;
    ImNodesMiniMapLocation minimap_location_;
};

} // namespace

static ColorNodeEditor color_editor;

void NodeEditorInitialize()
{
    print("node editor init");
    ImNodesIO& io = ImNodes::GetIO();
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
    ImNodes::StyleColorsClassic();
    
//    color_editor.node_types.push_back("value");
//    color_editor.ui_nodes2.push_back({ 0 });
//
//    color_editor.node_types.push_back("osc");
//    color_editor.ui_nodes2.push_back({ 1 });
//
//    color_editor.node_types.push_back("output");
//    color_editor.ui_nodes2.push_back({ 2 });
    
//    // Function signature for the functions that the vector will store
//    using ProcessFunc = void (*)(std::stack<void *> &);
//
//    // Vector to store the pointers to the functions
//    std::vector<ProcessFunc> funcs;
//    
//    funcs.push_back(midiin_module_process);
}

float* audio_output = new float[buffer_size]();

void NodeEditorAudioLoadInput(float* input_stream, int chan_count)
{
    audio_input = input_stream;
    audio_input_chan_count = chan_count;
}

float* NodeEditorAudioCallback() 
{ 
    // print("weeee");
    // float* audio_output;

    // print("audio_root_node_id",color_editor.audio_root_node_id_);
    if(color_editor.audio_root_node_id_ != -1)
    {
        audio_output = audio_evaluate(color_editor.graph, color_editor.audio_root_node_id_);
    }
    return audio_output;
}

void NodeEditorShow() { color_editor.show(); }

void NodeEditorShutdown() {}
} // namespace example
