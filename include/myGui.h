#include "ImWrap.h"
#include "vprint.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
// #include "Node.h"
using json = nlohmann::json;

static bool dockspaceOpen = true;

#include "AudioInterface.h"

struct CustomImGui : public ImWrap
{

    bool *main_running; //main loop pointer
    // ugly?
    void Attach(bool *p_main_running);

    bool audio_settings_open;

    int current_id;
    int current_ui_id;
    // std::vector<Node> nodes;
    AudioInterface *audioInterface;

    CustomImGui();
    void DropDownMenu(const char *name, std::vector<std::string> &itemNames, const char *&current_item, int isCapture);
    void AudioSettings(bool *p_audioSettingsOpen);
    void save();
    void load();
    void Shutdown2();

    virtual void Update() override;
};
