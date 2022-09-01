#include "myGui.h"

// json to structs
// #include "j2s.h"

#include "node_editor.h"
#include "node_editor2.h"

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

    audioInterface = new AudioInterface();
    audioInterface->scanAudioDevices();
    
    example::NodeEditorInitialize();

    audioInterface->openDevice(1,0);
};

void CustomImGui::DropDownMenu(const char *name, std::vector<std::string> &itemNames, const char *&current_item)
{
    
    if (ImGui::BeginCombo(name, current_item)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < itemNames.size(); n++)
        {
            bool is_selected = (current_item == itemNames[n].c_str()); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(itemNames[n].c_str(), is_selected))
            {
                current_item = itemNames[n].c_str();
                print(current_item,"selected",n);
                audioInterface->turnDeviceOn( audioInterface->openDevice(n,0) );
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

    if (ImGui::Button("scan devices"))
    {
        print("hey???");
        audioInterface->scanAudioDevices();
    }

    static const char *current_outputDeviceName = NULL;
    DropDownMenu("output devices", audioInterface->outputDeviceNames, current_outputDeviceName);

    static const char *current_inputDeviceName = NULL;
    DropDownMenu("input devices", audioInterface->inputDeviceNames, current_inputDeviceName);

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

    // Submit the DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

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