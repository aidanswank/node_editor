float* res = new float[buffer_size]();
float* audio_input = new float[buffer_size*2]();
int audio_input_chan_count = 0;

float* audio_evaluate(const Graph<Node2>& graph, const int root_node, std::vector<node_module_funcs> &module_funcs)
{
//    float* res;
    std::stack<int> postorder;
    
    dfs_traverse(graph, root_node, [&postorder](const int node_id) -> void { postorder.push(node_id); });
    
    std::stack<void*> value_stack;
    
    while (!postorder.empty())
    {
        const int id = postorder.top();
        postorder.pop();
        const Node2 node = graph.node(id);

        if(node.type!="output") // skip
        {
            for(int i = 0; i < module_funcs.size(); i++)
            {
                if(module_funcs[i].type==node.type)
                {
                    module_funcs[i].process(value_stack);
                }
            }
        }
        
        if(node.type=="value")
        {
            // print("value");
            // If the edge does not have an edge connecting to another node, then just use the value
            // at this node. It means the node's input pin has not been connected to anything and
            // the value comes from the node's UI.
            if (graph.num_edges_from_node(id) == 0ull)
            {
                value_stack.push(node.value);
                // print("node val",node.value);
            }
        }
    }

    // The final output node isn't evaluated in the loop -- instead we just pop
    // the three values which should be in the stack.
     assert(value_stack.size() == 2ull); //<-- NUM PARAMS TO POP
//     print("stack sz", value_stack.size());

     output_module_process(res, value_stack);


    return res;
}
