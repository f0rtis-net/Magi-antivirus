#include "quarantine.h"
#include <windows.h>
#include <filesystem>
#include <ostream>
#include <fstream>
#include <iostream>
#include "../scanner/file_scan.h"

std::string db_file_path = "./mqrt/mqdb.json";

void c_quarantine::handle_infected( std::string name, const std::string& old_path, const std::string& type )
{
	std::string _name = name + ".infq" + std::to_string( m_total_quarantined + m_quaratined_in_session );

	m_infected_data.emplace_back( quarantine_sample_t{ _name, old_path, type, true } );

	this->file_action_procedure( old_path, _name );

	++m_quaratined_in_session;
}

void c_quarantine::file_action_procedure( const std::string& old_path, std::string& name )
{
	// i ll remake it in the future
	//if ( m_remove_to_quarantine )

	std::string path = "mqrt\\" + name;

	try
	{
		std::filesystem::rename( old_path, path );
	}
	catch ( std::filesystem::filesystem_error& e )
	{
		std::cout << e.what( ) << '\n';
	}

//else
	//std::filesystem::remove( old_path );
}

void c_quarantine::initialize_database_file( )
{
	if ( !std::filesystem::exists( "./mqrt" ) )
		std::filesystem::create_directory( "./mqrt" );

	if ( std::ifstream( db_file_path ) )
	{
		load_database( );
		return;
	}

	std::ofstream file( db_file_path );
	file.close( );
}

void c_quarantine::save_database( )
{
	json_t json;

	auto& root_node = json[ "mqdbc" ];

	if ( !m_infected_data.empty( ) )
	{
		for ( auto& sample : m_infected_data )
		{
			if ( sample.m_name.empty( ) )
				continue;

			auto& sample_note = root_node[ sample.m_path ];
			{
				sample_note[ "name" ] = sample.m_name;
				sample_note[ "type" ] = sample.m_type;
			}
		}
	}

	std::ofstream file_out( db_file_path );

	file_out << json.toStyledString( );
	file_out.close( );
}

void c_quarantine::load_database( )
{
	int length;
	char* buffer;

	std::ifstream is( db_file_path );

	if ( !is )
		return;

	is.seekg( 0, std::ios::end );
	length = is.tellg( );
	is.seekg( 0, std::ios::beg );

	// allocate memory:
	buffer = new char[ length ];

	// read data as a block:
	is.read( buffer, length );
	is.close( );

	json_t json{};

	std::stringstream json_stream;
	json_stream << buffer;
	json_stream >> json;

	if ( !json.isMember( "mqdbc" ) )
		return;

	for ( auto var = json[ ( "mqdbc" ) ].begin( ); var != json[ ( "mqdbc" ) ].end( ); ++var )
	{
		if ( var->type( ) == Json::ValueType::objectValue )
		{
			if ( var.name( ).empty( ) )
				continue;

			auto itertor = var->begin( );

			m_infected_data.emplace_back( quarantine_sample_t{ itertor->asString( ), var.name( ), ( ++itertor )->asString( ), false } );
			++m_total_quarantined;
		}
	}
}

void c_quarantine::remove_from_quarantine( const quarantine_sample_t& target, bool _delete )
{
	int pos = target.m_name.find( ".infq" );
	std::string sub = target.m_name.substr( 0, pos );

	std::string q_path = "mqrt\\" + target.m_name;

	if ( _delete )
		remove( target.m_name.c_str( ) );
	else
		MoveFileA( q_path.c_str( ), target.m_path.c_str( ) );

	if ( target.in_session )
		--m_quaratined_in_session;
	else
		--m_total_quarantined;

	std::vector<quarantine_sample_t>::iterator iter = std::find_if( m_infected_data.begin( ), m_infected_data.end( ),
		[=] ( quarantine_sample_t& ap )
	{
		return ap.m_path == target.m_path;
	} );

	size_t index = std::distance( m_infected_data.begin( ), iter );

	m_infected_data.erase( m_infected_data.cbegin( ) + index );

	this->save_database( );
}