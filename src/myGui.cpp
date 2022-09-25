#include "myGui.h"

// json to structs
// #include "j2s.h"

#include "node_editor.h"

CustomImGui::CustomImGui()
{
    current_id = 0;
    current_ui_id = 0;
    audio_settings_open = false;
}

// init2
void CustomImGui::Attach(bool *p_main_running)
{
    main_running = p_main_running;

    load();

//    audioInterface = new AudioInterface();
//    audioInterface->scanAudioDevices();
    
    audio_interface = new my_interface();
    audio_interface->scan_devices();
    audio_interface->init_devices(44100, 256, 1, 2);
    audio_interface->open_stream();

    
    example::NodeEditorInitialize();
//    audioInterface->openDevice(1,0);
};

void CustomImGui::audio_settings_combo_box(const char *combo_box_name, std::vector<std::string> &item_names, int* select_choice, bool is_capture)
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
                *select_choice = n;
//                print(current_name,"selected",n);
                auto thing = audio_interface->device_id_map.find(current_name);
                print(thing->first,thing->second);
                if(is_capture) {
                    audio_interface->init_devices(44100, 256, thing->second, audio_interface->output_parameters.device);
                } else {
                    audio_interface->init_devices(44100, 256, audio_interface->input_parameters.device, thing->second);
                }
//                print("current input device", audio_interface->input_parameters.device);
                audio_interface->close_stream();
                audio_interface->open_stream();
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

#include <functional>

void CustomImGui::DropDownMenu(const char *name, std::vector<std::string> &itemNames, const char *&current_item, int isCapture)
{
    
    if (ImGui::BeginCombo(name, current_item)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < itemNames.size(); n++)
        {
            bool is_selected = (current_item == itemNames[n].c_str()); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(itemNames[n].c_str(), is_selected))
            {
                current_item = itemNames[n].c_str();
//                print(current_item,"selected",n);
//                audioInterface->openDevice(n, isCapture);
//                audioInterface->turnDeviceOn( audioInterface->openDevice(n,0) );
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus(); // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
        }
        ImGui::EndCombo();
    }
};


void CustomImGui::AudioSettings(bool *p_audioSettingsOpen)
{
    ImGui::Begin("audio settings", p_audioSettingsOpen);

//    if (ImGui::Button("scan devices"))
//    {
//        print("hey???");
////        audioInterface->scanAudioDevices();
//        audio_interface->scan_devices();
//    }

//    static const char *current_outputDeviceName = NULL;
    static int output_selection;
    audio_settings_combo_box("output devices", audio_interface->output_devices, &output_selection, false);
    static int input_selection;
    audio_settings_combo_box("input devices", audio_interface->input_devices, &input_selection, false);
//    auto thing = audio_interface->device_id_map.find(audio_interface->input_devices[input_selection]);
//    print(thing->first,thing->second);
//    DropDownMenu("output devices", audio_interface->output_devices, current_outputDeviceName, 0);
    

//    static const char *current_inputDeviceName = NULL;
//    static int output_selection;
//    combo_box("output devices", audio_interface->output_devices, &output_selection);
//    audio_interface->input_devices

    ImGui::End();
};

void CustomImGui::save()
{

    // json j_vec(nodes);

    // Save the internal imnodes state
    ImNodes::SaveCurrentEditorStateToIniFile("save_load.ini");

    // json j2 = {
    //     {"pi", 3.141},
    //     {"happy", true},
    //     {"nodes", j_vec}
    // };

    // // write prettified JSON to another file
    // std::ofstream o("pretty.json");
    // o << std::setw(4) << j2 << std::endl;

};

void CustomImGui::load()
{
    // Load the internal imnodes state
    ImNodes::LoadCurrentEditorStateFromIniFile("save_load.ini");

    
    // // read a JSON file
    // std::ifstream i("pretty.json");
    // //if file doesnt exist create blank and load from memory
    // if(i.fail()){
    //     save();
    // } else {
    //     json j;
    //     i >> j;
    //     nodes = j["nodes"];
    //     current_id = j["nodes"].size();
    // }

};

void CustomImGui::Shutdown2()
{
    print("shutdown2");
    example::NodeEditorShutdown();
}

#include <iostream>
#include <fstream>

void CustomImGui::Update()
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

//    // Submit the DockSpace
    ImGuiIO &io = ImGui::GetIO();
//    io.IniFilename = "/Users/aidan/imgui.ini";

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    
//    ofstream myfile;
//    myfile.open ("example.txt");
//    std::ifstream *myfile = new std::ifstream();
//    myfile->open("/Users/aidan/imgui.ini");
    
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Audio settings"))
            {
                audio_settings_open = !audio_settings_open;
                std::cout << audio_settings_open << std::endl;
            }
            if (ImGui::MenuItem("Save"))
            {
                std::cout << "save" << std::endl;
                save();
            }
            if (ImGui::MenuItem("Exit"))
            {
                std::cout << "exit" << std::endl;
                *main_running = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // ImGui::Begin("editor");
    // ImGui::Text("This is some useful text.");

    // node_space();

    example::NodeEditorShow();

    // ImGui::ShowDemoWindow();
    if (audio_settings_open)
    {
        AudioSettings(&audio_settings_open);
        // std::cout << audio_settings_open << std::endl;
    }

    ImGui::End();
};
