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
#include <gak/fmtNumber.h>
#include <gak/numericString.h>

#include <winlib/stddlg.h>
#include <winlib/regedit.h>
#include <winlib/registry.h>

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

ProcessStatus GlobalConfig::handleCreate()
{
	long		openResult;
	Registry	desktopKey;
	Registry	explorerKey;
	Registry	debuggerKey;
	Registry	internetKey;
	STRING		valueBuff;

	openResult = desktopKey.openUsers( ".default\\Control Panel\\Desktop" );
	if( openResult == ERROR_SUCCESS )
	{
		if( desktopKey.readValue( "Wallpaper", &valueBuff ) == rsOK )
			setEditText( IDE_WALLPAPER, valueBuff );

		if( desktopKey.readValue( "TileWallpaper", &valueBuff ) == rsOK )
		{
			if( valueBuff[0U] == '0' )
				clrButtonCheck( IDC_TILE_WALL );
			else
				setButtonCheck( IDC_TILE_WALL );
		}

		if( desktopKey.readValue( "SCRNSAVE.EXE", &valueBuff ) == rsOK )
			setEditText( IDE_SCREEN_SAVER, valueBuff );

		if( desktopKey.readValue( "ScreenSaveTimeOut", &valueBuff ) == rsOK && !valueBuff.isEmpty())
		{
			int		timeOut = valueBuff.getValueN<int>() / 60;
			valueBuff = gak::formatNumber(timeOut);
			setEditText( IDE_SAVER_TIMEOUT, valueBuff );
		}
	}

	askCheckBox.initHandle( this, IDC_ASK_CHECK );
	debuggerCombo.initHandle( this, IDC_DEBUG_COMBO );

	for( size_t i=0; i<arraySize( debuggerList ); ++i )
		debuggerCombo.addEntry( debuggerList[i].title );

	openResult = debuggerKey.openPublic( "Software\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug" );
	if( openResult == ERROR_SUCCESS )
	{
		if( debuggerKey.readValue( "Debugger", &valueBuff ) == rsOK )
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

		if( debuggerKey.readValue( "Auto", &valueBuff ) == rsOK )
		{
			if( valueBuff[0U] == '0' )
				askCheckBox.setActive();
			else
				askCheckBox.clrActive();
		}
	}
	else
		debuggerCombo.selectEntry(arraySize( debuggerList )-1);

	openResult = explorerKey.openPublic( "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders" );
	if( openResult == ERROR_SUCCESS )
	{
		if( explorerKey.readValue( "Common Desktop", &valueBuff ) == rsOK )
		{
			setEditText( IDC_DESKTOP, valueBuff );
		}

		if( explorerKey.readValue( "Common Favorites", &valueBuff ) == rsOK )
		{
			setEditText( IDC_FAVORITES, valueBuff );
		}

		if( explorerKey.readValue( "Common Start Menu", &valueBuff ) == rsOK )
		{
			setEditText( IDC_COMMON_START, valueBuff );
		}

		if( explorerKey.readValue( "Common Programs", &valueBuff ) == rsOK )
		{
			setEditText( IDC_COMMON_PROG, valueBuff );
		}

		if( explorerKey.readValue( "Common Startup", &valueBuff ) == rsOK )
		{
			setEditText( IDC_COMMON_AUTO, valueBuff );
		}

		if( explorerKey.readValue( "Common Administrative", &valueBuff ) == rsOK )
		{
			setEditText( IDC_ADMIN_MENU, valueBuff );
		}

		if( explorerKey.readValue( "Common Documents", &valueBuff ) == rsOK )
		{
			setEditText( IDC_DOCUMENTS, valueBuff );
		}

		if( explorerKey.readValue( "Common Templates", &valueBuff ) == rsOK )
		{
			setEditText( IDC_TEMPLATES, valueBuff );
		}

		if( explorerKey.readValue( "Common AppData", &valueBuff ) == rsOK )
		{
			setEditText( IDC_APP_FILES, valueBuff );
		}
	}

	openResult = debuggerKey.openPublic( "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings" );
	if( openResult == ERROR_SUCCESS )
	{
		if( internetKey.readValue( "ActiveXCache", &valueBuff ) == rsOK )
		{
			setEditText( IDC_DOWNLOAD, valueBuff );
		}

	}
	return psDO_DEFAULT;
}

ProcessStatus GlobalConfig::handleOk()
{
#if 0											// actually disabled
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

		_RegCloseKey( desktopKey );
	}


	openResult = RegOpenKeyEx(
		_HKEY_LOCAL_MACHINE,
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

		_RegCloseKey( backgroundKey );
	}


	openResult = RegOpenKeyEx(	_HKEY_LOCAL_MACHINE,
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


		_RegCloseKey( debuggerKey );
	}

	openResult = RegOpenKeyEx(	_HKEY_LOCAL_MACHINE,
								"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", 0,
								KEY_SET_VALUE, &explorerKey1 );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = RegOpenKeyEx(	_HKEY_LOCAL_MACHINE,
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

			_RegCloseKey( explorerKey2 );
		}
		_RegCloseKey( explorerKey1 );
	}

	openResult = RegOpenKeyEx(	_HKEY_LOCAL_MACHINE,
								"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0,
								KEY_SET_VALUE, &internetKey );
	if( openResult == ERROR_SUCCESS )
	{
		getEditText( IDC_DOWNLOAD, valueBuff, sizeof( valueBuff ) );
		valueSize = DWORD(strlen( valueBuff ) + 1);
		RegSetValueEx( internetKey, "ActiveXCache", 0,
						REG_SZ, (LPBYTE)valueBuff, valueSize );

		_RegCloseKey( internetKey );
	}

	openResult = RegOpenKeyEx(	_HKEY_LOCAL_MACHINE,
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

		_RegCloseKey( internetKey );
	}
#endif
	return psDO_DEFAULT;
}

ProcessStatus GlobalConfig::handleCommand( int cmd )
{
	char valueBuff[1024];

	switch( cmd )
	{
		case AUTORUN_DIALOG:
		{
			AutorunDialog	dialog( true );

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
			AssociationsConfig	dialog(true);

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

