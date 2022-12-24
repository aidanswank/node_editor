#pragma once

#include "ImGui.h"
#include "vprint.h"
#include <imnodes.h>
#include "node_editor.h"
#include "graph.h"
#include <vector>
#include "NodeDefs.h"
#include "module_helper.h"
//#include "CjFilter.h"

class xmodule {
public:
//    std::string type;
    std::string type;  // Declare const member

    virtual void init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes) = 0;
    virtual void process(std::stack<void *> &value_stack) = 0;
    virtual void show(const uinode2 &node, example::Graph<Node2> &graph) = 0;
};

//class lop_filter : xmodule {
//public:
//    
//    float* input_audio;
//    int input_audio_attr;
//    float* new_output;
//    
//    float cutoff;
//    float resonance;
//    CjFilter filter;
//    
//    float* cutoff_input;
//    int cutoff_input_attr;
//    
//    void init(ImVec2 click_pos, example::Graph<Node2> &graph, std::vector<uinode2> &ui_nodes)
//    {
//        this->type = "lop filter";
//        
//        uinode2 ui_node;
//        ui_node.type = this->type;
//        ui_node.id = graph.insert_node( Node2( ui_node.type ) );
//        
//        //push module struct as ui node
//        push_value_node(this, ui_node, graph);
//        
//        input_audio_attr = push_value_node(input_audio, ui_node, graph);
//        
//        cutoff_input_attr = push_value_node(cutoff_input, ui_node, graph);
//        
//        for(int i = 0; i < ui_node.ui.size(); i++)
//        {
//            graph.insert_edge(ui_node.id, ui_node.ui[i]);
//        }
//
//        ui_nodes.push_back(ui_node);
//
//        ImNodes::SetNodeScreenSpacePos(ui_node.id, click_pos);
//    }
//    void process(std::stack<void *> &value_stack)
//    {
//        float *cutoff_input = (float*)value_stack.top();
//        value_stack.pop();
//            
//        float *input_audio = (float*)value_stack.top();
//        value_stack.pop();
//        
//        // pop off module struct last
//        lop_filter *jf_data = (lop_filter*)value_stack.top();
//        value_stack.pop();
//        
//        float *new_output = jf_data->new_output;
//        
//        for(int i = 0; i < 256; i++)
//        {
//    //        new_output[i] = *jf_data->filter.doFilter(input_audio[i], jf_data->cutoff, jf_data->resonance);
//            
//            float cutoff = -1;
//            
//            if(*cutoff_input==0) // if nothings plugged in
//            {
//                cutoff = jf_data->cutoff; // use ui value
//            } else {
//                cutoff = cutoff_input[i];
//            }
//            
//            assert(cutoff != -1);
//            
//            new_output[i] = *jf_data->filter.doFilter(input_audio[i], cutoff, jf_data->resonance);
//     
//        }
//
//        value_stack.push(new_output);
//    }
//    void show(const uinode2 &node, example::Graph<Node2> &graph)
//    {
////        jfilter::jfilter_data* jf_data = (jfilter::jfilter_data*)graph.node(node.ui[STRUCT_IDX]).value; // store struct in index zero
//        
//        const float node_width = 100;
//        ImNodes::BeginNode(node.id);
//
//        DEBUG_NODE_TITLE_BAR(node.type);
//
//        {
//            ImNodes::BeginInputAttribute(input_audio_attr);
//            ImGui::TextUnformatted("audio in");
//            ImNodes::EndInputAttribute();
//        }
//        
//        {
//            ImNodes::BeginInputAttribute(cutoff_input_attr);
//            ImGui::TextUnformatted("cutoff in");
//            ImNodes::EndInputAttribute();
//        }
//
//        ImGui::PushItemWidth(node_width);
//    //    if(*jf_data->cutoff_input==0)
//    //    {
//        ImGui::DragFloat("cutoff", &cutoff, 0.01f, 0.f, 1.0f);
//    //    }
//        ImGui::DragFloat("resonance", &resonance, 0.01f, 0.f, 1.0f);
//        ImGui::PopItemWidth();
//
//        ImNodes::BeginOutputAttribute(node.id);
//        ImGui::Text("output");
//        ImNodes::EndOutputAttribute();
//
//        ImNodes::EndNode();
//    }
//};
