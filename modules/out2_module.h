#include "modules/module.h"

class output_module : public xmodule {
public:
    
    int *audio_root_node_id;
    
    output_module(int *p_audio_root_node_id)
    {
        type = "output";
        audio_root_node_id = p_audio_root_node_id;
    }
    
    void init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes) override
    {
//        print("audio ouput node");
//        float* arr = new float[256]();
//        const Node2 value("value", arr);
//        float* gain_ptr = new float(0.0f);
//        const Node2 gain("value", gain_ptr);
//        const Node2 out("output");
//
//        uinode2 audio_ui_node;
//        audio_ui_node.type = "output";
//        audio_ui_node.ui.push_back( graph.insert_node(value) );
//        audio_ui_node.ui.push_back( graph.insert_node(gain) ); // data storage
//        audio_ui_node.id = graph.insert_node(out);
//
//        
//    //                    UiNode audio_ui_node;
//    //                    audio_ui_node.type = NodeType::output;
//    //                    audio_ui_node.ui.output.input = graph.insert_node(value);
//    //                    audio_ui_node.ui.output.gain = graph.insert_node(gain); // data storage
//    //                    audio_ui_node.id = graph.insert_node(out);
//
//        graph.insert_edge(audio_ui_node.id, audio_ui_node.ui[0]);
//        graph.insert_edge(audio_ui_node.id, audio_ui_node.ui[1]);
//
//        ui_nodes.push_back(audio_ui_node);
//    //    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos); // comment out when refactoring module
//        audio_root_node_id = new int(audio_ui_node.id); //copy constructor
    }
    
    void process(std::stack<void *> &value_stack) override
    {

    }
    
    void show(const uinode2 &node, example::Graph<Node2> &graph) override
    {
        const float node_width = 50;

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
    //    ImGui::DragFloat("gain", &*fa_gain_stupid, 0.01f, 0.f, 1.0f);
        
        MyKnob("gain", &*fa_gain_stupid, 0.0f, 1.0f);


        // float* float_arr = (float*)audio_graph_.node(node.ui.output.input).value;
        // for(int i = 0; i < 256; i++)
        // {
        //     print(float_arr[i]);
        // }
        // ImGui::PlotLines("##hidelabel", float_arr, 256);

        ImGui::PopItemWidth();

        ImNodes::EndNode();
    }
    
};

