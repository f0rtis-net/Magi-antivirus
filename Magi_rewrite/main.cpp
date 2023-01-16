#include <iostream>
#include <Windows.h>
#include "globals.h"
#include <fstream>
#include "gui/gui_wrapper/wrapper.h"
#pragma warning(disable : 4996)

using namespace Melhior;
using namespace std;


void startup_init( )
{
	unsigned long size = 255;

	GetComputerName( &g_globals->computer_name[ 0 ], &size );

	_OSVERSIONINFOA os;
	os.dwOSVersionInfoSize = sizeof( os );
	GetVersionExA( &os );

	g_globals->version = std::to_string( os.dwMajorVersion ) + '.' + std::to_string( os.dwMinorVersion );

	typedef BOOL( WINAPI* LPFN_ISWOW64PROCESS ) ( HANDLE, PBOOL );
	LPFN_ISWOW64PROCESS fnWOW64PROCESS;
	BOOL os64 = false;
	fnWOW64PROCESS = ( LPFN_ISWOW64PROCESS )GetProcAddress( GetModuleHandle( TEXT( "kernel32" ) ), "IsWow64Process" );

	fnWOW64PROCESS( GetCurrentProcess( ), &os64 );

	g_globals->bitness = os64 ? "64 bit." : "32 bit.";
}

int APIENTRY WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	startup_init( );

	Melhior::g_database->process_database( );

	g_gui_kernel.gui_exec( );


	return 0;
}