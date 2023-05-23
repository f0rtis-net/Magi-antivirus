#include "bison_driver.h"
#include "rule_wrapper.h"
#include <string.h>
#include <sstream>
#include <fstream>
#include <filesystem>

using namespace Melhior;

namespace fs = std::filesystem;

database_parser::database_parser( ) :
	m_rules( ),
	m_scanner( *this ),
	m_parser( m_scanner, *this ),
	m_location( 0 )
{

}

int database_parser::parse_condition( const std::string& expression )
{
	m_location = 0;
	istringstream* str = new istringstream( expression );
	this->switch_input( str );
	return m_parser.parse( );
}

void database_parser::catch_error( int pos, const std::string& file_name )
{
	//сделать разницу для про режима и деф юзерка. Для про режима будет выводиться то что внизу
	//а для юзерка просто выведем ошибку, что в бдшке ошибка, и выводим имя ошибочного файла.
	std::string error_message = "Some error was occured in: [ " + file_name + " ] on symbol position: [ " + std::to_string( pos ) + " ].\n";
	error_message += "Stop parsing this file.";

	exeptions.emplace_back( error_message );
}

void database_parser::process_database( )
{
	if ( !fs::exists( "./mdb" ) )
	{
		std::filesystem::create_directory( "./mdb" );
	}

	m_rules.clear( );

	for ( auto& object : fs::directory_iterator( "./mdb" ) )
	{
		if ( fs::path( object.path( ) ).extension( ).string( ) == ".mlh" )
		{
			std::ifstream* curr_file = new std::ifstream( object.path( ).c_str( ) );

			if ( this->parse( curr_file ) )
				catch_error( this->location( ), fs::path( object.path( ) ).filename( ).string( ) );

			++db_files_count;
		}
	}
}

int database_parser::parse( std::istream* file_handle )
{
	this->switch_input( file_handle );
	//сбросили читку позиции символа
	m_location = 0;
	return m_parser.parse( );
}

void database_parser::clear( )
{
	m_location = 0;
	m_rules.clear( );
}

std::string database_parser::str( ) const
{
	std::stringstream s;
	s << "Melhior: " << m_rules.size( ) << " rules parsed from file." << endl;
	for ( int i = 0; i < m_rules.size( ); i++ )
	{
		s << " * " << m_rules[ i ].str( ) << endl;
	}
	return s.str( );
}

void database_parser::switch_input( std::istream* is )
{
	m_scanner.switch_streams( is, &cout );
}

void database_parser::create_rule( const Rule& cmd )
{
	m_rules.push_back( cmd );
}

void database_parser::increase_location( unsigned int loc )
{
	m_location += loc;
}

unsigned int database_parser::location( ) const
{
	return m_location;
}


