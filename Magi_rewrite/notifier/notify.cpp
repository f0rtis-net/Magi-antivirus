#include "notify.h"
#include "windows_api/wintoastlib.h"
#include "../globals.h"

using namespace WinToastLib;

HWND find_my_window( )
{
	DWORD dwProcID = GetCurrentProcessId( );
	HWND hWnd = GetTopWindow( GetDesktopWindow( ) );
	while ( hWnd )
	{
		DWORD dwWndProcID = 0;
		GetWindowThreadProcessId( hWnd, &dwWndProcID );
		if ( dwWndProcID == dwProcID )
			return hWnd;
		hWnd = GetNextWindow( hWnd, GW_HWNDNEXT );
	}
	return NULL;
}

class CustomHandler : public IWinToastHandler
{
public:
	void toastActivated( ) const
	{
		SetForegroundWindow( find_my_window( ) );
	}

	void toastActivated( int actionIndex ) const
	{
		SetForegroundWindow( find_my_window( ) );
	}

	void toastDismissed( WinToastDismissalReason state ) const
	{ }

	void toastFailed( ) const
	{ }
};

enum Results
{
	ToastClicked,					// user clicked on the toast
	ToastDismissed,					// user dismissed the toast
	ToastTimeOut,					// toast timed out
	ToastHided,						// application hid the toast
	ToastNotActivated,				// toast was not activated
	ToastFailed,					// toast failed
	SystemNotSupported,				// system does not support toasts
	UnhandledOption,				// unhandled option
	MultipleTextNotSupported,		// multiple texts were provided
	InitializationFailure,			// toast notification manager initialization failure
	ToastNotLaunched				// toast could not be launched
};

void notify::initialize_system( )
{
	WinToast::instance( )->setAppName( L"magi_antivirus" );
	WinToast::instance( )->setAppUserModelId( L"Root" );
	WinToast::instance( )->initialize( );
}

void notify::create_event( std::string content, std::wstring action_text )
{
	if ( g_globals->is_window_focused )
		return;

	std::wstring widestr = std::wstring( content.begin( ), content.end( ) );

	WinToastTemplate templ( WinToastTemplate::Text02 );
	templ.setTextField( widestr.c_str( ), WinToastTemplate::FirstLine );
	templ.setAudioOption( WinToastTemplate::AudioOption::Default );
	templ.addAction( action_text );

	WinToast::instance( )->showToast( templ, new CustomHandler( ) );
}