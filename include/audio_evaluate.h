float* res = new float[buffer_size]();
float* audio_input = new float[buffer_size*2]();
int audio_input_chan_count = 0;

float* audio_evaluate(const Graph<Node>& graph, const int root_node)
{
//    float* res;
    std::stack<int> postorder;
    
    dfs_traverse(graph, root_node, [&postorder](const int node_id) -> void { postorder.push(node_id); });
    
    std::stack<void*> value_stack;
    
    while (!postorder.empty())
    {
        const int id = postorder.top();
        postorder.pop();
        const Node node = graph.node(id);

        switch (node.type)
        {
        case NodeType::test_external:
        {
            test_module::process_module(value_stack);
        }
        break;
        case NodeType::interface_in:
        {
            // print("white!!");

            // memory leak but i dont want to add a node right now lol
            float* output = new float[buffer_size]();
            
            for(int i = 0; i < buffer_size; i++)
            {

//                float whitenoise = rand() % 100;
//                whitenoise = whitenoise / 100;

                if(audio_input_chan_count==2)
                {
                    output[i]  = audio_input[i * 2];
                } else  if(audio_input_chan_count==1) {
                    output[i]  = audio_input[i];
                }

            }
            value_stack.push(output);

        }
        break;
        case NodeType::vst:
        {
//            float* output;

            float* input_buf = (float*)value_stack.top();
//            for(int i = 0; i < 10; i++)
//            {
//                print("inbuf",input_buf[i]);
//            }
            value_stack.pop();

            EasyVst *vst_ptr = (EasyVst*)value_stack.top();
            value_stack.pop();

            float *left_input = vst_ptr->channelBuffer32(Steinberg::Vst::kInput, 0);

            if (!vst_ptr->process(buffer_size))
            {
                std::cerr << "VST process() failed" << std::endl;
                // return 1;
            }

            //input
            for (unsigned long i = 0; i < buffer_size; ++i)
            {
                left_input[i] = input_buf[i];
//                 right_input[i * 2 + 1] = f * 0.1;
            }

            //output
            float *left = vst_ptr->channelBuffer32(Steinberg::Vst::kOutput, 0);
//             float *right = userData->vst.channelBuffer32(Steinberg::Vst::kOutput, 0);
//            for (unsigned long i = 0; i < buffer_size; ++i)
//            {
//                output[i] = left[i];
////                 outputBuffer[i * 2 + 1] = right[i];
//            }

            value_stack.push(left);
        }
        break;
        case NodeType::sine:
        {
            // print("sine");

            float* output = (float*)value_stack.top();
            value_stack.pop();
            
            int* osc_type_ptr = (int*)value_stack.top();
            int osc_type_num = *osc_type_ptr;
            value_stack.pop();
            // print("osc type",osc_type_num);

            float* slider_ptr = (float*)value_stack.top();
            float freq = *slider_ptr;
            value_stack.pop();
            // print("yooo",freq);

            Oscillator *osc_ptr = (Oscillator*)value_stack.top();
            // print("yooo",gain_arr[0]);
            value_stack.pop();


//            float* output = new float[buffer_size]();
            
            osc_ptr->setFrequency(freq);
            osc_ptr->setMode((Oscillator::OscillatorMode)osc_type_num);
            
            for(int i = 0; i < buffer_size; i++)
            {
                double osc_output =  osc_ptr->nextSample();

                // float whitenoise = rand() % 100;
                // whitenoise = whitenoise / 100;
                // output[i]  = whitenoise * 0.06;

                output[i] = osc_output;
            }
            value_stack.push(output);
        }
        break;
        case NodeType::waveviewer:
        {
            float* view_buf = (float*)value_stack.top();
            value_stack.pop();

            float* input_array = (float*)value_stack.top();
            memcpy(view_buf, input_array, sizeof(float)*256);
            value_stack.pop();

            value_stack.push(input_array);

        }
        break;
        case NodeType::white:
        {
            // print("white!!");

            // memory leak but i dont want to add a node right now lol
            float* output = new float[buffer_size]();
            for(int i = 0; i < buffer_size; i++)
            {

                float whitenoise = rand() % 100;
                whitenoise = whitenoise / 100;

                output[i]  = whitenoise;

            }
            value_stack.push(output);
        }
        break;
        case NodeType::xfader:
        {
            float* output = new float[buffer_size]();

            float *slider_ptr = (float*)value_stack.top();
            value_stack.pop();

            float* input_a = (float*)value_stack.top();
            value_stack.pop();

            float* input_b = (float*)value_stack.top();
            value_stack.pop();

//            print("size",value_stack.size(),"a",*input_a,"b",*input_b,"slide",*slider_ptr);

            float amount = *slider_ptr;

            for(int i = 0; i < buffer_size; i++)
            {

                output[i]  = (input_a[i] * amount) + (input_b[i] * (1.0-amount));
            }

            value_stack.push(output);
        }
        break;
        case NodeType::value:
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
        break;
        case NodeType::output:
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
            for(int i = 0; i < buffer_size; i++)
            {
                // MIXED UP REMAINING NODES
                for(int j = 0; j < to_be_mixed.size(); j++)
                {
                    res[i] += to_be_mixed[j][i];
                }
            }
            
            for(int i = 0; i < buffer_size; i++)
            {
                res[i] *= (*gain_arr); // accessing [0]
            }
                
        }

        break;
        default:
            break;
        }
    }

    // The final output node isn't evaluated in the loop -- instead we just pop
    // the three values which should be in the stack.
    // assert(value_stack.size() == 1ull);

    // print("stack sz", value_stack.size());

    // float* res = new float[buffer_size]();
    // if(value_stack.size() == 1ull)
    // {
    //     // print("yo");
    //     res = value_stack.top();
    //     // for(int i = 0; i < buffer_size; i++)
    //     // {
    //     //     print(res[i]);
    //     // }
    //     value_stack.pop();
    // }

    return res;
}
