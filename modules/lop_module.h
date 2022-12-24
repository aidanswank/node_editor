#include "modules/module.h"

struct lop_filter_data
{
    float* input_audio;
    int input_audio_attr;
    float* new_output;
    
    float cutoff;
    float resonance;
    CjFilter filter;
    
    float* cutoff_input;
    int cutoff_input_attr;
};

class lop_filter : public xmodule {
public:
    
    lop_filter()
    {
        xmodule::type = "lop filter";
    }
    
    void init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes) override
    {
        
        uinode2 ui_node;
        ui_node.type = this->type;
        ui_node.id = graph.insert_node( Node2( ui_node.type ) );
        
        lop_filter_data *lop_dat = new lop_filter_data;
        lop_dat->cutoff_input = new float[256];
        lop_dat->input_audio = new float[256];
        lop_dat->new_output = new float[256];
        lop_dat->cutoff = 0.5;
        lop_dat->resonance = 0.2;

        //push module struct as ui node
        push_value_node(lop_dat, ui_node, graph);
        
        lop_dat->input_audio_attr = push_value_node(lop_dat->input_audio, ui_node, graph);
        lop_dat->cutoff_input_attr = push_value_node(lop_dat->cutoff_input, ui_node, graph);
        
        for(int i = 0; i < ui_node.ui.size(); i++)
        {
            graph.insert_edge(ui_node.id, ui_node.ui[i]);
        }

        ui_nodes.push_back(ui_node);

        ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
    }
    
    void process(std::stack<void *> &value_stack) override
    {
        float *cutoff_input = (float*)value_stack.top();
        value_stack.pop();
            
        float *input_audio = (float*)value_stack.top();
        value_stack.pop();
        
        // pop off module struct last because it was pushed in first
        lop_filter_data *lop_dat = (lop_filter_data*)value_stack.top();
        value_stack.pop();
        
        float *new_output = lop_dat->new_output;
        
        for(int i = 0; i < 256; i++)
        {
    //        new_output[i] = *jf_data->filter.doFilter(input_audio[i], jf_data->cutoff, jf_data->resonance);
            
            float cutoff = -1;
            
            if(*cutoff_input==0) // if nothings plugged in
            {
                cutoff = lop_dat->cutoff; // use ui value
            } else {
                cutoff = cutoff_input[i];
            }
            
            assert(cutoff != -1);
            
            new_output[i] = *lop_dat->filter.doFilter(input_audio[i], cutoff, lop_dat->resonance);
     
        }

        value_stack.push(new_output);
    }
    
    void show(const uinode2 &node, example::Graph<Node2> &graph) override
    {
        lop_filter_data *lop_dat = (lop_filter_data*)graph.node(node.ui[STRUCT_IDX]).value; // store struct in index zero
        
        const float node_width = 100;
        ImNodes::BeginNode(node.id);

        DEBUG_NODE_TITLE_BAR(node.type);

        {
            ImNodes::BeginInputAttribute(lop_dat->input_audio_attr);
            ImGui::TextUnformatted("audio in");
            ImNodes::EndInputAttribute();
        }
        
        {
            ImNodes::BeginInputAttribute(lop_dat->cutoff_input_attr);
            ImGui::TextUnformatted("cutoff in");
            ImNodes::EndInputAttribute();
        }

        ImGui::PushItemWidth(node_width);
    //    if(*jf_data->cutoff_input==0)
    //    {
        ImGui::DragFloat("cutoff", &lop_dat->cutoff, 0.01f, 0.f, 1.0f);
    //    }
        ImGui::DragFloat("resonance", &lop_dat->resonance, 0.01f, 0.f, 1.0f);
        ImGui::PopItemWidth();

        ImNodes::BeginOutputAttribute(node.id);
        ImGui::Text("output");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
    }
    
};

// hardcode name
//const std::string xmodule::type = "lop filter";  // Initialize const member
