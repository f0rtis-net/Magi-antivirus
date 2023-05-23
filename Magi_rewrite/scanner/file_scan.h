#pragma once
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

enum scan_phases_e
{
	none_phase,
	prepare_targets,
	main_scan
};

class c_file_scanner
{
	bool scanner_started;
	int scan_timer;
	int scan_phase;
	void main_thread( const std::vector<std::filesystem::path>& path_to_scan );
	void select_targets_from_path( const std::string& root_path );
public:
	int target_ammount;
	int valid_targets;
	std::vector<std::string> targets_for_scan;
	bool started_scan( ) { return scanner_started; }
	int get_scan_time( ) { return scan_timer; }
	int get_scan_phase( ) { return scan_phase; }
	int get_target_ammo( ) { return target_ammount; }
	void scan_trampoline( const std::vector<fs::path>& path_to_scan );
};

inline const auto g_file_scanner = std::make_unique<c_file_scanner>( );
