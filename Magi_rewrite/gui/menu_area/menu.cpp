#include "menu.h"
#include "../../globals.h"
#include "../../libs/imgui/imgui.h"
#include "../../libs/imgui/imgui_internal.h"
#include "blur/post_processing.h"
#include "../gui_wrapper/wrapper.h"
#include "custom_elements/subtab.h"

void c_menu::quarantine_menu( )
{
	ImGui::PushFont( fonts::def_text );
	ImGui::SetCursorPos( ImVec2( 170, 20 ) );
	ImGui::MainChild( "", ImVec2( 670, 400 ), true );
	{
		ImGui::SetCursorPosY( 9 );

		if ( g_quarantine->m_infected_data.empty( ) )
		{
			ImGui::SetCursorPos( ImVec2( 200, 150 ) );
			ImGui::PushFont( fonts::big_text );
			ImGui::Text( "Nothing Infected." );
			ImGui::PopFont( );
		}
		else
		{
			for ( auto& sample : g_quarantine->m_infected_data )
			{
				ImGui::SetCursorPosX( 11 );
				gui::file_plate( sample );
			}
		}
	}
	ImGui::EndChild( );
	ImGui::PopFont( );
}

void c_menu::misc_menu( )
{
	ImGui::PushFont( fonts::def_text );
	ImGui::SetCursorPos( ImVec2( 130, 50 ) );
	ImGui::MainChild( "Computer info", ImVec2( 350, 150 ) );
	{
		ImGui::SetCursorPos( ImVec2( 10, 50 ) );
		ImGui::Text( ( "Name: " + g_globals->computer_name ).c_str( ) );
		ImGui::SetCursorPosX( 10 );
		ImGui::Text( ( "System version: " + g_globals->version ).c_str( ) );
		ImGui::SetCursorPosX( 10 );
		ImGui::Text( ( "Bitness: " + g_globals->bitness ).c_str( ) );
	}
	ImGui::EndChild( );

	ImGui::SetCursorPos( ImVec2( 520, 50 ) );
	ImGui::MainChild( "Update info", ImVec2( 350, 150 ) );
	{
		ImGui::SetCursorPos( ImVec2( 10, 50 ) );
		ImGui::Text( ( "Last app update: " + std::string( __DATE__ ) ).c_str( ) );
		ImGui::SetCursorPosX( 10 );
		ImGui::Text( ( "App version: v 0.0.1" ) );
		ImGui::SetCursorPosX( 10 );
		ImGui::Text( ( "Last database update: 24.01.23" ) );
	}
	ImGui::EndChild( );

	ImGui::PopFont( );
}

void c_menu::scan_menu( )
{
	ImGui::PushFont( fonts::def_text );
	ImGui::SetCursorPos( ImVec2( 130, 50 ) );
	ImGui::MainChild( "Scanner history", ImVec2( 350, 150 ) );
	{
		int recalc = g_quarantine->m_total_quarantined + g_quarantine->m_quaratined_in_session;

		ImGui::SetCursorPos( ImVec2( 10, 50 ) );
		ImGui::Text( "Last scan: -" );
		ImGui::SetCursorPosX( 10 );
		ImGui::Text( ( "New in Quarantine: " + std::to_string( g_quarantine->m_quaratined_in_session ) ).c_str( ) );
		ImGui::SetCursorPosX( 10 );
		ImGui::Text( ( "Total in Quarantine: " + std::to_string( recalc ) ).c_str( ) );
	}
	ImGui::EndChild( );

	ImGui::SetCursorPos( ImVec2( 130, 240 ) );
	ImGui::MainChild( "Database info", ImVec2( 350, 150 ) );
	{
		ImGui::SetCursorPos( ImVec2( 10, 50 ) );
		ImGui::Text( ( "Samples loaded: " + std::to_string( Melhior::g_database->m_rules.size( ) ) ).c_str( ) );
		ImGui::SetCursorPosX( 10 );
		ImGui::Text( ( "Database files ammo: " + std::to_string( Melhior::g_database->files_ammo( ) ) ).c_str( ) );
		ImGui::Spacing( );
		ImGui::SetCursorPos( ImVec2( 10, 110 ) );
		if ( Melhior::g_database->exeptions.size( ) == 0 )
			ImGui::Text( "Database is valid" );
		else
		{
			ImGui::SetCursorPosX( 85 );
			ImGui::SmallButton( "Some errors occured" );
		}
	}
	ImGui::EndChild( );

	ImGui::SetCursorPos( ImVec2( 520, 50 ) );
	ImGui::MainChild( "Magi Scanner", ImVec2( 360, 340 ) );
	{
		if ( !g_file_scanner->started_scan( ) )
		{
			if ( g_file_scanner->valid_targets == 0 )
			{
				ImGui::SetCursorPos( ImVec2( 155, 100 ) );
				ImGui::PushFont( fonts::terms_ico );
				ImGui::TextColored( ImColor( 46, 218, 172 ), "A" );
				ImGui::PopFont( );

				ImGui::SetCursorPos( ImVec2( 95, 185 ) );
				ImGui::TextColored( ImColor( 46, 218, 172 ), "Your computer is clean!" );
			}
			else
			{
				ImGui::SetCursorPos( ImVec2( 155, 100 ) );
				ImGui::PushFont( fonts::terms_ico );
				ImGui::TextColored( ImColor( 255, 20, 0 ), "B" );
				ImGui::PopFont( );
				ImGui::SetCursorPos( ImVec2( 80, 185 ) );
				ImGui::TextColored( ImColor( 255, 20, 0 ), "Your computer is not clean!" );
				ImGui::SetCursorPosX( 80 );
				ImGui::TextColored( ImColor( 255, 20, 0 ), "See more info in quarantine." );
			}

			ImGui::SetCursorPos( ImVec2( 136, 240 ) );
			if ( ImGui::Button( "Scan now" ) )
				misc::file_dialog.Open( );

			if ( misc::file_dialog.HasSelected( ) )
				g_file_scanner->scan_trampoline( misc::file_dialog.GetMultiSelected( ) );
		}
		else
		{
			gui::circle_progress( ImVec2( 180, 120 ), 35.f, ImColor( 25, 35, 70 ), ImColor( 119, 119, 119 ) );
			ImGui::SetCursorPos( ImVec2( 120, 185 ) );
			ImGui::Text( ( "Scannig phase: " + std::to_string( g_file_scanner->get_scan_phase( ) ) ).c_str( ) );
			ImGui::SetCursorPos( ImVec2( 100, 250 ) );

			if ( g_file_scanner->get_scan_phase( ) == prepare_targets )
			{
				ImGui::SetCursorPosX( 115 );
				ImGui::Text( "Preparing targets..." );
			}
			else if ( g_file_scanner->get_scan_phase( ) == main_scan )
				ImGui::Text( ( "Targets for scan: " + std::to_string( g_file_scanner->target_ammount ) ).c_str( ) );

			if ( g_file_scanner->valid_targets != 0 )
			{
				ImGui::SetCursorPos( ImVec2( 140, 300 ) );
				ImGui::TextColored( ImColor( 255, 20, 0 ), ( "Infected: " + std::to_string( g_file_scanner->valid_targets ) ).c_str( ) );
			}
		}
	}
	ImGui::EndChild( );
	ImGui::PopFont( );
}

void c_menu::menu_bar( )
{
	ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( win_pos.x, win_pos.y ), ImVec2( win_pos.x + 100, win_pos.y + win_size.y ), ImColor( 19, 23, 54, 255 ), 10.f, ImDrawFlags_RoundCornersLeft );

	static int active_tab = 1;

	ImGui::GetWindowDrawList( )->AddImage( images::menu_logo, ImVec2( win_pos.x + 32, win_pos.y + 40 ), ImVec2( win_pos.x + 67, win_pos.y + 74 ) );

	ImGui::SetCursorPos( ImVec2( 0, 130 ) );
	ImGui::PushFont( fonts::tabs );

	g_globals->notified_tabs[ active_tab - 1 ] = false;

	if ( gui::subtab( "Scanner", ImVec2( 100, 50 ), active_tab == 1, "B", fonts::tab_ico, g_globals->notified_tabs[ 0 ] ) ) active_tab = 1;

	if ( gui::subtab( "Quarantine", ImVec2( 100, 50 ), active_tab == 2, "A", fonts::tab_ico, g_globals->notified_tabs[ 1 ] ) ) active_tab = 2;

	if ( gui::subtab( "Misc", ImVec2( 100, 50 ), active_tab == 3, "C", fonts::tab_ico, g_globals->notified_tabs[ 2 ] ) ) active_tab = 3;

	ImGui::PopFont( );

	switch ( active_tab )
	{
		case 1:
			scan_menu( ); break;
		case 2:
			g_file_scanner->valid_targets = 0;
			quarantine_menu( ); break;
		case 3:
			misc_menu( ); break;
	}
}

void c_menu::main_renderable( )
{
	if ( !g_globals->is_active ) return;
	ImGui::SetNextWindowPos( win_pos, ImGuiCond_Once );
	ImGui::SetNextWindowSize( ImVec2( 920, 440 ) );
	misc::file_dialog.SetWindowPos( win_pos.x + 130, win_pos.y );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 0,0 } );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 15.f );
	ImGui::Begin( "magi-antivirus", &g_globals->is_active, window_flags );
	{

		win_pos = ImGui::GetWindowPos( );
		win_size = ImGui::GetWindowSize( );

		ImGui::GetWindowDrawList( )->AddRectFilledMultiColor( ImVec2( win_pos.x + 20, win_pos.y ), ImVec2( win_pos.x + win_size.x, win_pos.y + win_size.y ),
			ImColor( 45, 59, 95 ), ImColor( 24, 25, 45 ), ImColor( 38, 56, 83 ), ImColor( 46, 48, 93 ) );

		menu_bar( );
	}
	ImGui::End( );
	ImGui::PushFont( fonts::hz_shrift );
	misc::file_dialog.Display( );
	ImGui::PopFont( );
	ImGui::PopStyleVar( 2 );
}

void c_menu::init( )
{
	if ( win_pos.x == 0 )
	{
		RECT screen_rect{};
		GetWindowRect( GetDesktopWindow( ), &screen_rect );
		auto screen_res = ImVec2( float( screen_rect.right ), float( screen_rect.bottom ) );
		win_pos = ( screen_res - win_size ) * 0.3f;
	}
}