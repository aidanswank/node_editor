#pragma once
#include "imgui.h"
#include <vector>
#include <string>
#include "vprint.h"

void combo_box(const char *combo_box_name, std::vector<std::string> &item_names, int *select_choice);

void combo_box_callback(const char *combo_box_name, std::vector<std::string> &item_names, void (*callback)(int));
