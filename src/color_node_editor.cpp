#include "node_editor.h"
#include "graph.h"
#include "modules/Oscillator.h"
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

//struct node_module_funcs
//{
//    std::string type;
//    using process_func = void(*)(std::stack<void *> &);
//    using init_func = void(*)(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes_);
//    using show_func = void(*)(const uinode2 &, example::Graph<Node2> &);
//    process_func process;
//    init_func init;
//    show_func show;
//};

#include "modules/module.h"

//#include "modules/test_module.h"
//#include "modules/midi_in_module.h"
#include "modules/output_module.h"
//#include "modules/xfader_module.h"
#include "modules/jfilter_module.h"
#include "modules/adsr_module.h"

// versions 2
#include "modules/lop_module.h"
#include "modules/out2_module.h"
#include "modules/oscillator_module.h"
#include "modules/midiin_module.h"
#include "modules/vst3_midi_module.h"

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

                
      
        
//                        midiin_module_init(click_pos, graph, ui_nodes);
                for(int i = 0; i < modules.size(); i++)
                {
                    if(modules[i]->type!="output") // skip
                    {
                        if (ImGui::MenuItem(modules[i]->type.c_str()))
                        {
//                            for(int a = 0; a < 100; a++)
//                            {
                            modules[i]->init(click_pos, graph2, ui_nodes2);
//                            }
                        }
                    }
                }

//                if (ImGui::MenuItem("TEST_EXTERNAL"))
//                {
//                    osc_module_init(click_pos, graph2, ui_nodes2, "osc");
//                }

                if (ImGui::MenuItem("AudioOutput") && audio_root_node_id_ == -1)
                {
                    output_module_init(audio_root_node_id_, click_pos, graph2, ui_nodes2);
                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();
        }

        for (const uinode2& node : ui_nodes2)
        {
            for(int i = 0; i < modules.size(); i++)
            {

                if(modules[i]->type==node.type)
                {
                    modules[i]->show(node, graph2);
                }
                
//                if(modules[i]->type=="output")
//                {
////                    print("ummm");
//                    output_module_show(node, graph2);
//                } else {
//                    if(modules[i]->type==node.type)
//                   {
//                       modules[i]->show(node, graph2);
//                   }
//                }
            }
        }
        
//        if(audio_root_node_id_ != -1)
//        {
//            output_module_show(node, graph2);
//        }
        
        for (const auto& edge : graph2.edges())
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
            if (graph2.node(edge.from).type == "value"){
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
                const std::string start_type = graph2.node(start_attr).type;
                const std::string end_type = graph2.node(end_attr).type;

                const bool valid_link = start_type != end_type;
                if (valid_link)
                {
                    // Ensure the edge is always directed from the value to
                    // whatever produces the value
                    if (start_type != "value")
                    {
                        std::swap(start_attr, end_attr);
                        print("swapped?", start_attr, end_attr);
                    }
                    print("is valid link", start_attr, end_attr);
                    
                    Node2 end_node = graph2.node(end_attr);
                    Node2 start_node = graph2.node(start_attr);
                    print("end type",end_node.type,"start type",start_node.type);
                    if(end_node.type=="midi in")
                    {
//                        midiin_module_data *module_data = (midiin_module_data*)end_node.value; // store struct in index zero
                        midiin_module_data *module_data = (midiin_module_data*)graph2.node(end_attr+1).value; // store struct in
                        module_data->num_consumers++;
//                        print("midi connected to vst instrument");
//                        print("add consumer", module_data->num_consumers);
                    }
                    
                    graph2.insert_edge(start_attr, end_attr);
                }
            }
        }


        // // Handle deleted links

        {
            int link_id;
            if (ImNodes::IsLinkDestroyed(&link_id))
            {
                graph2.erase_edge(link_id);
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
                    graph2.erase_edge(edge_id);
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
                    graph2.erase_node(node_id);
                    auto iter = std::find_if(
                        ui_nodes2.begin(), ui_nodes2.end(), [node_id](const uinode2& node) -> bool {
                            return node.id == node_id;
                        });
                    // Erase any additional internal nodes
//                    switch (iter->type)
//                    {
//                    case NodeType::output:
//                        graph.erase_node(iter->ui.output.input);
//                        audio_root_node_id_ = -1;
//                        break;
//                    default:
//                        break;
//                    }
                    if(iter->type=="output")
                    {
                        graph2.erase_node(iter->ui[0]); // 0 is INPUT
                        graph2.erase_node(iter->ui[1]); // 1 is GAIN todo change
                        audio_root_node_id_ = -1;
                    } else { // other node
//                        iter->ui.size()
                        print("deleting",iter->type,"id",iter->id);
                        
//                        uinode2 hey = iter;
                        void* mod_struct = (void*)graph2.node(iter->ui[STRUCT_IDX]).value; // store struct in index zero
//                        Node2 n = graph2.node(iter->ui[0]);

                        for(int i = 0; i < iter->ui.size(); i++)
                        {
                            print("erasing ui id",iter->ui[i]);
                            graph2.erase_node(iter->ui[i]);
                        }
          
//                        print("freeing ptr",&mod_struct);
                        free(mod_struct);
                        mod_struct = NULL;
                    }
                    ui_nodes2.erase(iter);
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
    Graph<Node2> graph2;
    std::vector<UiNode> ui_nodes;
    std::vector<uinode2> ui_nodes2;
    std::vector<std::string> node_types;
//    std::vector<node_module_base> module_funcs;
    std::vector<xmodule*> modules;

    int root_node_id_;
    int audio_root_node_id_;
    ImNodesMiniMapLocation minimap_location_;
};

} // namespace

static ColorNodeEditor color_editor;

static node_module_base extracted() {
    node_module_base module3;
    return module3;
}

void NodeEditorInitialize()
{
    print("node editor init");
    ImNodesIO& io = ImNodes::GetIO();
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
    
    io.MultipleSelectModifier.Modifier = &ImGui::GetIO().KeyCtrl;

    ImNodesStyle& style = ImNodes::GetStyle();
    style.Flags |= ImNodesStyleFlags_GridLinesPrimary | ImNodesStyleFlags_GridSnapping;
    
    ImNodes::StyleColorsClassic();
    
//    std::string module1_name = "output";
//    node_module_base module1;
//    module1.type = module1_name;
//    module1.init = output_module_init; // DIFF FUNCTION BECAUSE ROOT_NODE_ID !!! BUT SHOW FUNC WORKS
//    module1.show = output_module_show;
//    module1.process = output_module_process; // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//    color_editor.node_types.push_back(module1_name);
//    color_editor.module_funcs.push_back(module1);
//
//    color_editor.modules.push_back(module1_name);
    
//    node_module_base module2;
//    module2.type = "osc";
//    module2.init = osc_module_init;
//    module2.show = osc_module_show;
//    module2.process = osc_module_process;
////    jfilter_loader(&module2);
//    color_editor.node_types.push_back(module2.type);
//    color_editor.module_funcs.push_back(module2);
//
//    node_module_base module3 = extracted();
//    module3.type = "xfader";
//    module3.init = xfader_module_init;
//    module3.show = xfader_module_show;
//    module3.process = xfader_module_process;
//
//    color_editor.node_types.push_back(module3.type);
//    color_editor.module_funcs.push_back(module3);
//
//    node_module_base module4;
////    module4.type = "jfilter";
////    module4.init = jfilter::module_init;
////    module4.show = jfilter::module_show;
////    module4.process = jfilter::module_process;
//
//    jfilter::glue_known(&module4);
//
//    color_editor.node_types.push_back(module4.type);
//    color_editor.module_funcs.push_back(module4);
//
//    node_module_base module5;
//    module5.type = "adsr";
//    module5.init = adsr_module_init;
//    module5.show = adsr_module_show;
//    module5.process = adsr_module_process;
//
//    color_editor.node_types.push_back(module5.type);
//    color_editor.module_funcs.push_back(module5);
    
    lop_filter *lop_module = new lop_filter();
    color_editor.modules.push_back(lop_module);
    color_editor.node_types.push_back(lop_module->type);
    
    output_module *out_module = new output_module(&color_editor.audio_root_node_id_);
    color_editor.modules.push_back(out_module);
    color_editor.node_types.push_back(out_module->type);
    ///
    oscillator_module *osc_module = new oscillator_module();
    color_editor.modules.push_back(osc_module);
    color_editor.node_types.push_back(osc_module->type);
    
    midiin_module *midiin_mod = new midiin_module();
    color_editor.modules.push_back(midiin_mod);
    color_editor.node_types.push_back(midiin_mod->type);
    
    vst3_midi_module *vst3_midi_mod = new vst3_midi_module();
    color_editor.modules.push_back(vst3_midi_mod);
    color_editor.node_types.push_back(vst3_midi_mod->type);
    
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
        audio_output = audio_evaluate(color_editor.graph2, color_editor.audio_root_node_id_, color_editor.modules);
    }
    return audio_output;
}

void NodeEditorShow() { color_editor.show(); }

void NodeEditorShutdown() {}
} // namespace example
