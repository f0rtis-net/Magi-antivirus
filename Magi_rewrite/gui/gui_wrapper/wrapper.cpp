#include "wrapper.h"
#include "../../globals.h"
#include "../menu_area/blur/post_processing.h"
#include "../menu_area/menu.h"
#include "images.h"
#include "fonts.h"
#include <dwmapi.h>
#include "../../notifier/notify.h"

HWND main_hwnd = nullptr;

LPDIRECT3DDEVICE9        g_pd3dDevice;
D3DPRESENT_PARAMETERS    g_d3dpp;
LPDIRECT3D9              g_pD3D;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

bool CreateDeviceD3D( HWND hWnd )
{
	if ( ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) == NULL )
		return false;

	// Create the D3DDevice
	ZeroMemory( &g_d3dpp, sizeof( g_d3dpp ) );
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	g_d3dpp.EnableAutoDepthStencil = TRUE;

	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if ( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice ) < 0 )
		return false;

	return true;
}

void CleanupDeviceD3D( )
{
	if ( g_pd3dDevice ) { g_pd3dDevice->Release( ); g_pd3dDevice = NULL; }
	if ( g_pD3D ) { g_pD3D->Release( ); g_pD3D = NULL; }
}

void ResetDevice( )
{
	ImGui_ImplDX9_InvalidateDeviceObjects( );
	HRESULT hr = g_pd3dDevice->Reset( &g_d3dpp );
	if ( hr == D3DERR_INVALIDCALL )
		IM_ASSERT( 0 );
	ImGui_ImplDX9_CreateDeviceObjects( );
}

void c_winpai_imgui::gui_exec( )
{
	// Create application window
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, "magi-antivirus", NULL };
	RegisterClassEx( &wc );
	int x = GetSystemMetrics( SM_CXSCREEN ) / 2 - 250;
	int y = GetSystemMetrics( SM_CYSCREEN ) / 2 - 150;

	main_hwnd = CreateWindow( wc.lpszClassName, "magi-antivirus", WS_POPUP, x, y, 1, 1, NULL, NULL, wc.hInstance, NULL );

	SetWindowRgn( main_hwnd, CreateRoundRectRgn( 0, 0, 930, 460, 21, 21 ), FALSE );

	MARGINS margins{ -1 };
	DwmExtendFrameIntoClientArea( main_hwnd, &margins );
	SetLayeredWindowAttributes( main_hwnd, RGB( 0, 0, 0 ), 255, LWA_ALPHA );

	// Initialize Direct3D
	if ( !CreateDeviceD3D( main_hwnd ) )
	{
		CleanupDeviceD3D( );
		UnregisterClass( wc.lpszClassName, wc.hInstance );
		return;
	}

	// Show the window
	ShowWindow( main_hwnd, SW_HIDE );
	UpdateWindow( main_hwnd );

	// Setup Dear ImGui context
	ImGui::CreateContext( );

	ImGuiIO& io = ImGui::GetIO( );
	io.IniFilename = nullptr; //crutial for not leaving the imgui.ini file
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

	ImVec4* colors = ImGui::GetStyle( ).Colors;

	auto config = ImFontConfig( );
	config.FontDataOwnedByAtlas = false;

	fonts::tab_ico = io.Fonts->AddFontFromMemoryTTF( tab_icons, sizeof( tab_icons ), 20.f, &config, io.Fonts->GetGlyphRangesDefault( ) );
	fonts::tabs = io.Fonts->AddFontFromMemoryTTF( title_font, sizeof( title_font ), 10.f, &config, io.Fonts->GetGlyphRangesCyrillic( ) );
	fonts::terms_ico = io.Fonts->AddFontFromMemoryTTF( sys_ico, sizeof( sys_ico ), 50.f, &config, io.Fonts->GetGlyphRangesCyrillic( ) );
	fonts::def_text = io.Fonts->AddFontFromMemoryTTF( title_font, sizeof( title_font ), 18.f, &config, io.Fonts->GetGlyphRangesCyrillic( ) );
	fonts::big_text = io.Fonts->AddFontFromMemoryTTF( title_font, sizeof( title_font ), 40.f, &config, io.Fonts->GetGlyphRangesCyrillic( ) );
	fonts::quarantine = io.Fonts->AddFontFromMemoryTTF( quarantine_icons, sizeof( quarantine_icons ), 15.f, &config, io.Fonts->GetGlyphRangesCyrillic( ) );
	fonts::hz_shrift = io.Fonts->AddFontFromFileTTF( "C:/windows/fonts/verdana.ttf", 18.f, &config, io.Fonts->GetGlyphRangesCyrillic( ) );

	ImGuiStyle* style = &ImGui::GetStyle( );
	{
		style->WindowPadding = ImVec2( 4, 4 );
		style->WindowBorderSize = 0.f;
		style->WindowRounding = 15.f;

		style->FramePadding = ImVec2( 8, 6 );
		style->FrameRounding = 3.f;
		style->FrameBorderSize = 1.f;
		style->Colors[ ImGuiCol_WindowBg ].w = 0.f;
	}

	if ( images::menu_logo == nullptr )
		D3DXCreateTextureFromFileInMemory( g_pd3dDevice, &logo_image, sizeof( logo_image ), &images::menu_logo );

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init( main_hwnd );
	ImGui_ImplDX9_Init( g_pd3dDevice );

	notify::initialize_system( );

	misc::file_dialog.SetTitle( "file-browser" );
	misc::file_dialog.SetTypeFilters( { ".*" } );
	misc::file_dialog.SetWindowSize( 765, 440 );
	misc::file_dialog.SetPwd( "C:\\" );

	// Main loop
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame( );
		ImGui_ImplWin32_NewFrame( );
		ImGui::NewFrame( );
		{
			static int init = false;
			if ( !init )
			{
				g_menu_framework->init( );
				init = true;
			}
			else
				g_menu_framework->main_renderable( );
		}
		ImGui::EndFrame( );

		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
		if ( g_pd3dDevice->BeginScene( ) >= 0 )
		{
			ImGui::Render( );
			ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
			g_pd3dDevice->EndScene( );
		}

		// Update and Render additional Platform Windows
		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			ImGui::UpdatePlatformWindows( );
			ImGui::RenderPlatformWindowsDefault( );
		}

		HRESULT result = g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

		// Handle loss of D3D9 device
		if ( result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel( ) == D3DERR_DEVICENOTRESET )
		{
			ResetDevice( );
		}
		if ( !g_globals->is_active )
		{
			msg.message = WM_QUIT;
		}
	}

	ImGui_ImplDX9_Shutdown( );
	ImGui_ImplWin32_Shutdown( );
	ImGui::DestroyContext( );

	CleanupDeviceD3D( );
	DestroyWindow( main_hwnd );
	UnregisterClass( wc.lpszClassName, wc.hInstance );

	return;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
		return true;

	switch ( msg )
	{
		case WM_SIZE:
			if ( g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED )
			{
				g_d3dpp.BackBufferWidth = LOWORD( lParam );
				g_d3dpp.BackBufferHeight = HIWORD( lParam );
				ResetDevice( );
			}
			return 0;
		case WM_SYSCOMMAND:
			if ( ( wParam & 0xfff0 ) == SC_KEYMENU ) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}