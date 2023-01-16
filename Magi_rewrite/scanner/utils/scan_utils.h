#pragma once
#include <string>
#include <Windows.h>
#include <utility>
#include <vector>
#include <thread>
#include <functional>

namespace scan_utils
{
	inline bool is_not_dot_name( std::wstring const& Filename )
	{
		return ( ( Filename != L"." ) && ( Filename != L".." ) );
	}

	inline bool is_directory( DWORD FileAttrs )
	{
		return (
			( 0 != ( FileAttrs & FILE_ATTRIBUTE_DIRECTORY ) ) &&
			( 0 == ( FileAttrs & FILE_ATTRIBUTE_REPARSE_POINT ) )
			);
	}

	template<typename T>
	class worker
	{
		using array_type = std::vector<T>;
		using iterator = typename array_type::iterator;

	public:
		worker( array_type& arr, size_t cur, size_t count ) : arr( arr ), cur( cur ), end( cur + count ) { }

		static std::vector<worker<T>> make( array_type& arr )
		{
			const auto max_threads = std::thread::hardware_concurrency( ) - 1;
			const auto chunk_size = arr.size( ) / max_threads;
			std::vector<worker<T>> pools;
			if ( arr.size( ) < max_threads )
			{
				pools.emplace_back( worker<T>( arr, 0, arr.size( ) ) );
				return pools;
			}

			for ( size_t i{}; i < arr.size( ); i += chunk_size )
			{
				worker<T> pool( arr, i, min( chunk_size, arr.size( ) - i ) );
				pools.push_back( pool );
			}

			return pools;
		}

		std::thread dispatch( const std::function<void( iterator )>& fn )
		{
			auto thread = std::thread( [&, fn] ( )
			{
				while ( cur != end )
				{
					fn( arr.begin( ) + cur++ );
				}
			} );

			return thread;
		}

	private:
		array_type& arr;
		size_t cur;
		size_t end;
	};

	inline bool match_signature( const  LPVOID& file, const DWORD fsize, const std::string& signature )
	{
		static auto pattern_to_byte = [] ( const char* pattern )
		{
			auto bytes = std::vector<int>{};
			auto start = const_cast< char* >( pattern );
			auto end = const_cast< char* >( pattern ) + strlen( pattern );

			for ( auto current = start; current < end; ++current )
			{
				if ( *current == '?' )
				{
					++current;
					if ( *current == '?' )
						++current;
					bytes.push_back( -1 );
				}
				else
				{
					bytes.push_back( strtoul( current, &current, 16 ) );
				}
			}
			return bytes;
		};

		auto pattern_bytes = pattern_to_byte( signature.c_str( ) );
		auto scan_bytes = reinterpret_cast< std::uint8_t* >( file );

		auto s = pattern_bytes.size( );
		auto d = pattern_bytes.data( );

		if ( fsize < s )
			return false;

		for ( auto i = 0; i < fsize - s; ++i )
		{
			bool found = true;
			for ( auto j = 0; j < s; ++j )
			{
				if ( scan_bytes[ i + j ] != d[ j ] && d[ j ] != -1 )
				{
					found = false;
					break;
				}
			}
			if ( found )
			{
				return true;
			}
		}
		return false;
	}
}