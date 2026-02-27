/*
		Project:		CONFIG
		Module:			GLOBAL.CPP
		Description:	GlobalConfig Dialog
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2026 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Linz, Austria ``AS IS''
		AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
		TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
		PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
		CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
		LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
		USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
		ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
		OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
		SUCH DAMAGE.
*/

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/fcopy.h>

#include <winlib/stddlg.h>
#include <winlib/regedit.h>

#include "global.h"

#include "autorun.h"
#include "assoc.h"
#include "directry.h"

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	ifdef __WIN32__
#		pragma option -a4
#		pragma option -pc
#	else
#		pragma option -po
#		pragma option -a2
#	endif
#endif

using namespace winlib;
using namespace gak;

struct Debugger
{
	const char *title, *command;
} debuggerList[] =
{
	{ "Borland C++ IDE", "bcw -d%ld;%ld" },
	{ "Borland Debugger 4.1", "bordbg41 -aeargs %ld %ld" },
	{ "Borland Debugger 5.0", "bordbg50 -aeargs %ld %ld" },
	{ "Dr. Watson", "drwtsn32 -p %ld -e %ld -g" },
	{ "Turbo Debugger", "td32 -as%ld;-ae%ld" },
	{ "Visual Studio", "vsjitdebugger.exe -p %ld -e %ld" },
	{ "NONE", "" }
};

static size_t replaceEnvironment( char valueBuff[], const char *env )
{
	char	*envVar = getenv( env );
	size_t	envLen = envVar ? strlen( envVar ) : 0;

	if( envLen )
	{
		STRING tmpBuff = "%";
		tmpBuff += env;
		tmpBuff += '%';
		tmpBuff += valueBuff + envLen;
		strcpy( valueBuff, tmpBuff );
	}

	return strlen( valueBuff ) + 1;
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus GlobalConfig::handleCreate( void )
{
	long		openResult;
	HKEY		desktopKey;
	HKEY		explorerKey;
	HKEY		debuggerKey;
	HKEY		internetKey;
	DWORD		valueType;
	DWORD		valueSize;
	char		valueBuff[256];

	openResult = RegOpenKeyEx(	HKEY_USERS,
								".default\\Control Panel\\Desktop", 0,
								KEY_READ, &desktopKey );
	if( openResult == ERROR_SUCCESS )
	{
		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( desktopKey, "Wallpaper", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueType == REG_SZ )
			setEditText( IDE_WALLPAPER, valueBuff );

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( desktopKey, "TileWallpaper", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			if( *valueBuff == '0' )
				clrButtonCheck( IDC_TILE_WALL );
			else
				setButtonCheck( IDC_TILE_WALL );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( desktopKey, "SCRNSAVE.EXE", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );

		if( openResult == ERROR_SUCCESS
		&&  valueType == REG_SZ )
			setEditText( IDE_SCREEN_SAVER, valueBuff );

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( desktopKey, "ScreenSaveTimeOut", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );

		if( openResult == ERROR_SUCCESS
		&&  valueType == REG_SZ )
		{
			int		timeOut = atoi( valueBuff ) / 60;

			itoa( timeOut, valueBuff, 10 );
			setEditText( IDE_SAVER_TIMEOUT, valueBuff );
		}

		RegCloseKey( desktopKey );
	}

	askCheckBox.initHandle( this, IDC_ASK_CHECK );
	debuggerCombo.initHandle( this, IDC_DEBUG_COMBO );

	for( size_t i=0; i<arraySize( debuggerList ); ++i )
		debuggerCombo.addEntry( debuggerList[i].title );

	openResult = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
								"Software\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug", 0,
								KEY_READ, &debuggerKey );
	if( openResult == ERROR_SUCCESS )
	{
		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( debuggerKey, "Debugger", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			bool	found = false;
			for( int i=0; i<arraySize( debuggerList ); ++i )
				if( !strcmpi( valueBuff, debuggerList[i].command ) )
				{
					debuggerCombo.selectEntry(i);
					found = true;
					break;
				}
			if( !found )
				debuggerCombo.selectEntry(arraySize( debuggerList )-1);
		}
		else
			debuggerCombo.selectEntry(arraySize( debuggerList )-1);

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( debuggerKey, "Auto", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			if( valueBuff[0] == '0' )
				askCheckBox.setActive();
			else
				askCheckBox.clrActive();
		}

		RegCloseKey( debuggerKey );
	}
	else
		debuggerCombo.selectEntry(arraySize( debuggerList )-1);

	openResult = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
								"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", 0,
								KEY_READ, &explorerKey );
	if( openResult == ERROR_SUCCESS )
	{
		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Common Desktop", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_DESKTOP, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Common Favorites", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_FAVORITES, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Common Start Menu", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_COMMON_START, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Common Programs", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_COMMON_PROG, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Common Startup", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_COMMON_AUTO, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Common Administrative Tools", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_ADMIN_MENU, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Common Documents", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_DOCUMENTS, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Common Templates", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_TEMPLATES, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Common AppData", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_APP_FILES, valueBuff );
		}

		RegCloseKey( explorerKey );
	}

	openResult = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
								"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0,
								KEY_READ, &internetKey );
	if( openResult == ERROR_SUCCESS )
	{
		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( internetKey, "ActiveXCache", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_DOWNLOAD, valueBuff );
		}

		RegCloseKey( internetKey );
	}
	return psDO_DEFAULT;
}

ProcessStatus GlobalConfig::handleOk( void )
{
	long		openResult;
	HKEY		desktopKey,
				backgroundKey,
				explorerKey1,
				explorerKey2,
				debuggerKey,
				internetKey;
	int			selectedDebugger;
	DWORD		valueSize;
	const char	*valuePtr;
	char		valueBuff[1024];
	int			timeOut;

	STRING		tmpBuff;

	openResult = RegOpenKeyEx(	HKEY_USERS,
								".default\\Control Panel\\Desktop", 0,
								KEY_SET_VALUE, &desktopKey );
	if( openResult == ERROR_SUCCESS )
	{
		getEditText( IDE_WALLPAPER, valueBuff, sizeof( valueBuff ) );
		valueSize = DWORD(strlen( valueBuff ) + 1);
		RegSetValueEx( desktopKey, "Wallpaper", 0,
						REG_SZ, (LPBYTE)valueBuff, valueSize );

		if( isButtonChecked( IDC_TILE_WALL ) )
			strcpy( valueBuff, "1" );
		else
			strcpy( valueBuff, "0" );
		valueSize = DWORD(strlen( valueBuff ) + 1);
		RegSetValueEx( desktopKey, "TileWallpaper", 0,
						REG_SZ, (LPBYTE)valueBuff, valueSize );

		getEditText( IDE_SCREEN_SAVER, valueBuff, sizeof( valueBuff ) );
		valueSize = DWORD(strlen( valueBuff ) + 1);
		RegSetValueEx( desktopKey, "SCRNSAVE.EXE", 0,
						REG_SZ, (LPBYTE)valueBuff, valueSize );

		getEditText( IDE_SAVER_TIMEOUT, valueBuff, sizeof( valueBuff ) );
		timeOut = atoi( valueBuff ) * 60;
		itoa( timeOut, valueBuff, 10 );
		valueSize = DWORD(strlen( valueBuff ) + 1);
		RegSetValueEx( desktopKey, "ScreenSaveTimeOut", 0,
						REG_SZ, (LPBYTE)valueBuff, valueSize );

		RegCloseKey( desktopKey );
	}


	openResult = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Authentication\\LogonUI\\Background",
		0, KEY_SET_VALUE, &backgroundKey
	);
	if( openResult == ERROR_SUCCESS )
	{
		DWORD dwValue = 0;
		getEditText( IDE_WALLPAPER, valueBuff, sizeof( valueBuff ) );

		if( *valueBuff )
		{
			STRING	newPath = getenv( "windir" );
			newPath += "\\system32\\oobe";
			mkdir( newPath );
			newPath += "\\info";
			mkdir( newPath );
			newPath += "\\backgrounds";
			mkdir( newPath );
			newPath += "\\backgroundDefault.jpg";

			fcopy( valueBuff, (const char *)newPath );

			dwValue = 1;
		}

		openResult = RegSetValueEx( backgroundKey, "OEMBackground", 0,
					REG_DWORD, (LPBYTE)&dwValue, sizeof( dwValue ) );

		doLogValue( openResult );

		RegCloseKey( backgroundKey );
	}


	openResult = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
								"Software\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug", 0,
								KEY_SET_VALUE, &debuggerKey );
	if( openResult == ERROR_SUCCESS )
	{
		selectedDebugger = debuggerCombo.getSelection();

		if( selectedDebugger < arraySize( debuggerList ) )
			valuePtr = debuggerList[selectedDebugger].command;
		else
			valuePtr = "";

		RegSetValueEx( 
			debuggerKey, "Debugger", 0,
			REG_SZ, LPBYTE(valuePtr), DWORD(strlen(valuePtr)+1) 
		);

		if( askCheckBox.isActive() )
			valuePtr = "0";
		else
			valuePtr = "1";

		RegSetValueEx( 
			debuggerKey, "Auto", 0,
			REG_SZ, LPBYTE(valuePtr), DWORD(strlen(valuePtr)+1) 
		);


		RegCloseKey( debuggerKey );
	}

	openResult = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
								"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", 0,
								KEY_SET_VALUE, &explorerKey1 );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
								"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders", 0,
								KEY_SET_VALUE, &explorerKey2 );
		if( openResult == ERROR_SUCCESS )
		{
			getEditText( IDC_DESKTOP, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Common Desktop", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			replaceEnvironment( valueBuff, "ALLUSERSPROFILE" );
			replaceEnvironment( valueBuff, "PUBLIC" );
			valueSize = DWORD( replaceEnvironment( valueBuff, "ProgramData" ) );

			RegSetValueEx( explorerKey2, "Common Desktop", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_FAVORITES, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Common Favorites", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			replaceEnvironment( valueBuff, "ALLUSERSPROFILE" );
			replaceEnvironment( valueBuff, "PUBLIC" );
			valueSize = DWORD( replaceEnvironment( valueBuff, "ProgramData" ) );

			RegSetValueEx( explorerKey2, "Common Favorites", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_COMMON_START, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Common Start Menu", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			replaceEnvironment( valueBuff, "ALLUSERSPROFILE" );
			replaceEnvironment( valueBuff, "PUBLIC" );
			valueSize = DWORD( replaceEnvironment( valueBuff, "ProgramData" ) );

			RegSetValueEx( explorerKey2, "Common Start Menu", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_COMMON_PROG, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Common Programs", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			replaceEnvironment( valueBuff, "ALLUSERSPROFILE" );
			replaceEnvironment( valueBuff, "PUBLIC" );
			valueSize = DWORD( replaceEnvironment( valueBuff, "ProgramData" ) );

			RegSetValueEx( explorerKey2, "Common Programs", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_COMMON_AUTO, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Common Startup", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			replaceEnvironment( valueBuff, "ALLUSERSPROFILE" );
			replaceEnvironment( valueBuff, "PUBLIC" );
			valueSize = DWORD( replaceEnvironment( valueBuff, "ProgramData" ) );

			RegSetValueEx( explorerKey2, "Common Startup", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_ADMIN_MENU, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Common Administrative Tools", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			replaceEnvironment( valueBuff, "ALLUSERSPROFILE" );
			replaceEnvironment( valueBuff, "PUBLIC" );
			valueSize = DWORD( replaceEnvironment( valueBuff, "ProgramData" ) );

			RegSetValueEx( explorerKey2, "Common Administrative Tools", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_DOCUMENTS, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Common Documents", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			replaceEnvironment( valueBuff, "ALLUSERSPROFILE" );
			replaceEnvironment( valueBuff, "PUBLIC" );
			valueSize = DWORD( replaceEnvironment( valueBuff, "ProgramData" ) );

			RegSetValueEx( explorerKey2, "Common Documents", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_TEMPLATES, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Common Templates", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			replaceEnvironment( valueBuff, "ALLUSERSPROFILE" );
			replaceEnvironment( valueBuff, "PUBLIC" );
			valueSize = DWORD( replaceEnvironment( valueBuff, "ProgramData" ) );

			RegSetValueEx( explorerKey2, "Common Templates", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );


			getEditText( IDC_APP_FILES, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Common AppData", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			replaceEnvironment( valueBuff, "ALLUSERSPROFILE" );
			replaceEnvironment( valueBuff, "PUBLIC" );
			valueSize = DWORD( replaceEnvironment( valueBuff, "ProgramData" ) );

			RegSetValueEx( explorerKey2, "Common AppData", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			RegCloseKey( explorerKey2 );
		}
		RegCloseKey( explorerKey1 );
	}

	openResult = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
								"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0,
								KEY_SET_VALUE, &internetKey );
	if( openResult == ERROR_SUCCESS )
	{
		getEditText( IDC_DOWNLOAD, valueBuff, sizeof( valueBuff ) );
		valueSize = DWORD(strlen( valueBuff ) + 1);
		RegSetValueEx( internetKey, "ActiveXCache", 0,
						REG_SZ, (LPBYTE)valueBuff, valueSize );

		RegCloseKey( internetKey );
	}

	openResult = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
								"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ActiveX Cache", 0,
								KEY_SET_VALUE, &internetKey );
	if( openResult == ERROR_SUCCESS )
	{
		getEditText( IDC_DOWNLOAD, valueBuff, sizeof( valueBuff ) );
		valueSize = DWORD(strlen( valueBuff ) + 1);
		RegSetValueEx( internetKey, "0", 0,
						REG_SZ, (LPBYTE)valueBuff, valueSize );
		RegDeleteValue( internetKey, "1" );
		RegDeleteValue( internetKey, "2" );
		RegDeleteValue( internetKey, "3" );
		RegDeleteValue( internetKey, "4" );

		RegCloseKey( internetKey );
	}

	return psDO_DEFAULT;
}

ProcessStatus GlobalConfig::handleCommand( int cmd )
{
	char valueBuff[1024];

	switch( cmd )
	{
		case AUTORUN_DIALOG:
		{
			AutorunDialog	dialog( HKEY_LOCAL_MACHINE );

			dialog.create( this );
/*v*/		break;
		}
		case IDPB_BROWSE_WALL:
		{
			OpenFileDialog	dialog;
			STRING			fileName;

			fileName = "";
			if( dialog.create( this, BITMAP_TITLE, BITMAP_FILTER ) )
			{
				fileName = dialog.getFilename();
				setEditText( IDE_WALLPAPER, fileName );
			}

/*v*/		break;
		}
		case IDPB_BROWSE_SAVER:
		{
			OpenFileDialog	dialog;
			char			directory[1024];
			STRING			fileName;
			const char		*lastBackSlash;

			GetWindowsDirectory( directory, sizeof( directory ) );
			setcwd( directory );

			fileName = "";
			if( dialog.create( this, SAVER_TITLE, SAVER_FILTER ) )
			{
				fileName = dialog.getFilename();

				lastBackSlash = strrchr( (const char*)fileName, '\\' );
				if( lastBackSlash )
					lastBackSlash++;
				else
					lastBackSlash = fileName;

				setEditText( IDE_SCREEN_SAVER, lastBackSlash );
			}

/*v*/		break;
		}
		case IDC_DESK_BROWSE:
		{
			getEditText( IDC_DESKTOP, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_DESKTOP, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_FAV_BROWSE:
		{
			getEditText( IDC_FAVORITES, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_FAVORITES, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_COM_START_BROWSE:
		{
			getEditText( IDC_COMMON_START, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_COMMON_START, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_COM_PROG_BROWSE:
		{
			getEditText( IDC_COMMON_PROG, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_COMMON_PROG, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_COM_AUTO_BROWSE:
		{
			getEditText( IDC_COMMON_AUTO, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_COMMON_AUTO, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_ADMIN_BROWSE:
		{
			getEditText( IDC_ADMIN_MENU, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_ADMIN_MENU, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_DOC_BROWSE:
		{
			getEditText( IDC_DOCUMENTS, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_DOCUMENTS, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_TMPL_BROWSE:
		{
			getEditText( IDC_TEMPLATES, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_TEMPLATES, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_APP_BROWSE:
		{
			getEditText( IDC_APP_FILES, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_APP_FILES, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_DOWNLOAD_BROWSE:
		{
			char valueBuff[1024];

			getEditText( IDC_DOWNLOAD, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_DOWNLOAD, dialog.getDirectory() );

/*v*/		break;
		}
		case ASSOCIATION_DIALOG:
		{
			AssociationsConfig	dialog;

			dialog.create( this );
/*v*/		break;
		}
		default:
			return ModalDialog::handleCommand( cmd );
	}
	return psPROCESSED;
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

