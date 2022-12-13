#include "output_module.h"

void output_module_init(int &audio_root_node_id_, ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes)
{
    print("audio ouput node");
    float* arr = new float[256]();
    const Node2 value("value", arr);
    float* gain_ptr = new float(0.123);
    const Node2 gain("value", gain_ptr);
    const Node2 out("output");

    uinode2 audio_ui_node;
    audio_ui_node.type = "output";
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

    ui_nodes.push_back(audio_ui_node);
    ImNodes::SetNodeScreenSpacePos(audio_ui_node.id, click_pos);
    audio_root_node_id_ = audio_ui_node.id;
};
void output_module_show(const uinode2 &node, example::Graph<Node2> &graph)
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
};
void output_module_process(float* &res, std::stack<void *> &value_stack)
{
    float *gain_arr = (float*)value_stack.top();
    // print("yooo",gain_arr[0]);
    value_stack.pop();
    
    res = (float*)value_stack.top();
    value_stack.pop();
                    
//              print("REMAINING NODES!", value_stack.size());
    std::vector<float*> to_be_mixed;
    for(int i = 0; i < value_stack.size(); i++)
    {
        float* audio = (float*)value_stack.top();
        value_stack.pop();
        to_be_mixed.push_back(audio);
    }

    // adjust gain by knob
    for(int i = 0; i < 256; i++)
    {
        // MIXED UP REMAINING NODES
        for(int j = 0; j < to_be_mixed.size(); j++)
        {
            res[i] += to_be_mixed[j][i];
        }
    }
    
    for(int i = 0; i < 256; i++)
    {
        res[i] *= (*gain_arr); // accessing [0]
    }
};
