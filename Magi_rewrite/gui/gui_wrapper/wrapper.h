#pragma once
#include <functional>
#include <windows.h>

#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")

#include "../../libs/imgui/imgui.h"
#include "../../libs/imgui/imgui_impl_dx9.h"
#include "../../libs/imgui/imgui_impl_win32.h"
#include "../../libs/imgui/imgui_internal.h"
#include "../../libs/imgui/imfilebrowser.h"

class c_winpai_imgui
{
public:
	void gui_exec( );
};

namespace images
{
	inline IDirect3DTexture9* menu_logo = nullptr;
}

namespace fonts
{
	inline ImFont* def_text = nullptr;
	inline ImFont* tabs = nullptr;
	inline ImFont* tab_ico = nullptr;
	inline ImFont* terms_ico = nullptr;
}

namespace misc
{
	inline ImGui::FileBrowser file_dialog( ImGuiFileBrowserFlags_NoTitleBar | ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_MultipleSelection );
}

inline c_winpai_imgui g_gui_kernel;