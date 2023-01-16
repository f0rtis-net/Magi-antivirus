#pragma once
#include "database/bison_extention/scanner.h"
#include "database/bison_extention/parser.hpp"
#include "database/bison_driver.h"
#include "scanner/file_scan.h"
#include <string>

class c_globals
{
public:
	std::string username;
	std::string version;
	std::string bitness;
	std::string computer_name;
	std::string global_path;
	bool is_window_focused;
	bool is_active = true;
	bool notified_tabs[ 3 ]{ false, false, false };
};

inline c_globals* g_globals = new c_globals( );