#pragma once
#include <string>

namespace notify
{
	void initialize_system( );
	void create_event( std::string content, std::wstring action_text );
}