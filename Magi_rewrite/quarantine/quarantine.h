#pragma once
#include <string>
#include <memory>
#include <vector>
#include "../libs/json/json.h"

typedef Json::Value json_t;

struct quarantine_sample_t
{
	std::string m_name{};
	std::string m_path{};
	std::string m_type{};
	bool in_session{};

	quarantine_sample_t( const std::string& name, const std::string& path, const std::string& type, bool _session )
		: m_name( name ), m_path( path ), m_type( type ), in_session( _session )
	{ }
};

class c_quarantine
{
	bool m_remove_to_quarantine{};
	void file_action_procedure( const std::string& old_path, std::string& name );
public:
	void load_database( );
	void save_database( );
	void handle_infected( std::string name, const std::string& old_path, const std::string& type );
	void initialize_database_file( );
	void remove_from_quarantine( const quarantine_sample_t& target, bool _delete = true );
	int m_total_quarantined{};
	int m_quaratined_in_session{};
	std::vector<quarantine_sample_t> m_infected_data{};
};

inline const auto g_quarantine = std::make_unique<c_quarantine>( );