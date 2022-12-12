#pragma once

#include "ImGui.h"
#include "vprint.h"
#include "Oscillator.h"
#include <imnodes.h>
#include "node_editor.h"
#include "graph.h"
#include <vector>
#include "NodeDefs.h"

// // ids of nodes that point to data
// struct TestModule
// {
//     int osc; // should rename osc_ptr or id_osc_ptr??
//     int osc_type;
//     int osc_output;
//     int freq;
// };

double midi2Freq(int n);

class test_module
{
public:
//    template <typename T>
    static void init_module(ImVec2 click_pos, example::Graph<Node> &audio_graph_, std::vector<uinode2> &ui_nodes_);
    static void process_module(std::stack<void *> &value_stack);
//    template <typename T>
    static void show_module(const uinode2 &node, example::Graph<Node> &audio_graph_);
    
    //more
    static void combo_box(const char *combo_box_name, std::vector<std::string> &item_names, int *select_choice)
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
};
