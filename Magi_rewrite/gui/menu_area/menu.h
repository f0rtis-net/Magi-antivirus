#pragma once
#include <d3d9.h>
#include "../../libs/imgui/imgui.h"
#include <memory>

class c_menu
{
	bool file_dialog;
	ImVec2 win_pos, win_size;
	DWORD  window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	void scan_menu( );
	void quarantine_menu( );
	void misc_menu( );
	void menu_bar( );
public:
	void main_renderable( );
	void init( );
};

inline const auto g_menu_framework = std::make_unique<c_menu>( );