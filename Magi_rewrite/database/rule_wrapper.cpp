#include "rule_wrapper.h"

#include <iostream>
#include <sstream>

using namespace Melhior;
using std::cout;
using std::endl;

Rule::Rule( const std::string& name, std::vector<std::string> beakons, std::string& expression ) :
	m_name( name ),
	m_expression( expression ),
	m_beacons( beakons )
{

}

Rule::Rule( const std::string& name ) :
	m_name( name )
{ }

Rule::Rule( ) :
	m_name( )
{ }

Rule::~Rule( )
{ }

std::string Rule::str( ) const
{
	std::stringstream ts;
	ts << "name = [" << m_name << "], expression = [" << m_expression << "], Beacons: " << endl;
	int i = 0;
	for ( auto& rule : m_beacons )
	{
		ts << "\t( " << i << " )-> " << rule << endl;
		i++;
	}

	return ts.str( );
}

std::string Rule::name( ) const
{
	return m_name;
}

std::string Rule::expression( ) const
{
	return m_expression;
}

