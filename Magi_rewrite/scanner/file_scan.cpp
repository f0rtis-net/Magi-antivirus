#include "file_scan.h"
#include <thread>
#include <fstream>
#include "../gui/gui_wrapper/wrapper.h"
#include "utils/scan_utils.h"
#include <mutex>
#include "../database/bison_driver.h"
#include "../notifier/notify.h"
#include "../globals.h"

static std::mutex g_lock;

#define MUTEX_LOCK g_lock.lock()
#define MUTEX_UNLOCK g_lock.unlock()

void c_file_scanner::scan_trampoline( const std::vector<std::filesystem::path>& path_to_scan )
{
	//remove old data
	scan_timer = 0;
	target_ammount = 0;
	valid_targets = 0;
	scan_phase = none_phase;
	targets_for_scan.clear( );

	//yoss my nigga
	std::thread scanner_thread( [=] { main_thread( path_to_scan ); } );
	scanner_thread.detach( );

	scanner_started = true;
}

void capture_all_files( const std::string& path, std::vector <std::string>& files )
{
	if ( std::ifstream( path ) )
	{
		files.emplace_back( path );
		return;
	}

	WIN32_FIND_DATAW FindData;
	std::string const Mask = path + "\\*";
	std::wstring temp = std::wstring( Mask.begin( ), Mask.end( ) );
	HANDLE hFind = FindFirstFileW( temp.c_str( ), &FindData );

	if ( INVALID_HANDLE_VALUE != hFind )
	{
		do
		{
			if ( scan_utils::is_not_dot_name( &FindData.cFileName[ 0 ] ) )
			{
				std::wstring ss( &FindData.cFileName[ 0 ] );
				std::string converted( ss.begin( ), ss.end( ) );
				std::string const FullPath = path + "\\" + converted;

				if ( scan_utils::is_directory( FindData.dwFileAttributes ) )
					capture_all_files( FullPath, files );
				else
					files.emplace_back( FullPath );
			}
		} while ( NULL != FindNextFileW( hFind, &FindData ) );

		FindClose( hFind );
	}
}

void select_valid_targets( const std::string& path )
{
	auto is_executable = [=] ( const std::string& path_s ) -> bool
	{
		std::ifstream file( path_s, std::ios::binary );
		file.unsetf( std::ios::skipws );
		file.seekg( 0, std::ios::beg );
		char* header = new char[ sizeof( IMAGE_DOS_HEADER ) + 1 ]; header[ sizeof( IMAGE_DOS_HEADER ) ] = 0;
		file.read( header, sizeof( IMAGE_DOS_HEADER ) );
		file.close( );

		if ( reinterpret_cast< IMAGE_DOS_HEADER* >( header )->e_magic != IMAGE_DOS_SIGNATURE )
		{
			delete[] header;
			return false;
		}

		delete[] header;
		return true;
	};

	if ( is_executable( path ) )
	{
		MUTEX_LOCK;
		++g_file_scanner->target_ammount;
		g_file_scanner->targets_for_scan.emplace_back( path );
		MUTEX_UNLOCK;
	}
}

void c_file_scanner::select_targets_from_path( const std::string& root_path )
{
	std::vector<std::string> files_from_root = {};

	capture_all_files( root_path, files_from_root );

	std::vector<std::thread> threads;

	auto pools = scan_utils::worker<std::string>::make( files_from_root );

	for ( auto& pool : pools )
	{
		threads.push_back( pool.dispatch( [] ( auto iter )
		{
			select_valid_targets( *iter );
		} ) );
	}

	for ( auto& thread : threads )
		thread.join( );
}

void replace_first(
	std::string& s,
	std::string const& toReplace,
	std::string const& replaceWith
)
{
	std::size_t pos = s.find( toReplace );
	if ( pos == std::string::npos ) return;
	s.replace( pos, toReplace.length( ), replaceWith );
}

bool catch_target( LPVOID file, DWORD file_size, const Melhior::Rule& rule )
{
	int beac_index = 0;

	std::string expr = rule.expression( );

	Melhior::database_parser* parser = new Melhior::database_parser;

	for ( auto& beacon : rule.m_beacons )
	{
		int answer = scan_utils::match_signature( file, file_size, beacon );

		replace_first( expr, std::to_string( beac_index ), std::to_string( answer ) );

		++beac_index;
	}

	int anwser = parser->parse_condition( expr );
	delete parser;
	return anwser;
}

void scan_target( const std::string& path_to_target )
{
	//map file into my memory
	HANDLE h_file = CreateFileA( path_to_target.c_str( ), FILE_READ_ACCESS, NULL, nullptr, OPEN_EXISTING, NULL, nullptr );
	if ( h_file == INVALID_HANDLE_VALUE )
	{
		CloseHandle( h_file );
		return;
	}

	DWORD file_size = GetFileSize( h_file, nullptr );
	HANDLE hMap = CreateFileMappingA( h_file, nullptr, PAGE_READONLY, NULL, file_size, nullptr );

	if ( !hMap )
		return;

	LPVOID file = MapViewOfFile( hMap, FILE_MAP_READ, NULL, NULL, file_size );

	if ( file == nullptr )
	{
		CloseHandle( hMap );
		CloseHandle( h_file );
		return;
	}

	//main scan
	for ( auto& rule : Melhior::g_database->m_rules )
	{
		if ( catch_target( file, file_size, rule ) )
		{
			//fuck off
			UnmapViewOfFile( file );
			CloseHandle( hMap );
			CloseHandle( h_file );
			MUTEX_LOCK;
			--g_file_scanner->target_ammount;
			MUTEX_UNLOCK;

			std::filesystem::path p( path_to_target );
			MUTEX_LOCK;
			g_quarantine->handle_infected( p.filename( ).string( ), path_to_target, rule.name( ) );
			++g_file_scanner->valid_targets;
			MUTEX_UNLOCK;
			break;
		}
	}
}

void c_file_scanner::main_thread( const std::vector<std::filesystem::path>& path_to_scan )
{
	scan_phase = prepare_targets;

	for ( auto& path : path_to_scan )
		select_targets_from_path( path.string( ) );

	misc::file_dialog.ClearSelected( );

	scan_phase = main_scan;

	std::vector<std::thread> threads;

	auto scan_pools = scan_utils::worker<std::string>::make( targets_for_scan );

	for ( auto& pool : scan_pools )
	{
		threads.push_back( pool.dispatch( [] ( auto iter )
		{
			scan_target( *iter );
		} ) );
	}

	for ( auto& thread : threads )
		thread.join( );

	scanner_started = false;

	g_globals->notified_tabs[ 0 ] = true;
	g_globals->notified_tabs[ 1 ] = true;

	std::string base = "Scan completed! ";

	if ( g_file_scanner->valid_targets > 0 )
		base += "Some files are infected!";

	notify::create_event( base, L"Tap to see more info." );

	g_quarantine->save_database( );
}
