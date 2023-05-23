#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>

#include "bison_extention/scanner.h"

#include "bison_extention/parser.hpp"

namespace Melhior
{
	class Rule;

	class database_parser
	{
	public:
		database_parser( );

		int parse_condition( const std::string& expression );

		void process_database( );

		void clear( );

		int files_ammo( ) { return db_files_count; }

		std::string str( ) const;

		friend class Parser;
		friend class Scanner;
		std::vector<Rule> m_rules;
		std::vector<std::string> exeptions;

	private:
		void catch_error( int pos, const std::string& file_name );

		int parse( std::istream* file_handle );

		void switch_input( std::istream* is );

		void create_rule( const Rule& cmd );

		void increase_location( unsigned int loc );

		unsigned int location( ) const;

	private:
		int db_files_count;
		Scanner m_scanner;
		Parser m_parser;
		unsigned int m_location;
	};

	inline const auto g_database = std::make_unique<database_parser>( );
}

#endif // INTERPRETER_H
