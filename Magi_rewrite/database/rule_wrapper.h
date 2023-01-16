#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
#include <stdint.h>

namespace Melhior
{
	class Rule
	{
	public:
		Rule( const std::string& name, std::vector<std::string> beakons, std::string& m_expression );
		Rule( const std::string& name );
		Rule( );
		~Rule( );

		std::string str( ) const;
		std::string name( ) const;
		std::string expression( ) const;
		std::vector<std::string> m_beacons;
		bool is_valid;
	private:
		std::string m_name;
		std::string m_expression;
	};
}

#endif // COMMAND_H
