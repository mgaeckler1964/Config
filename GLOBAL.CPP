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

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

static const char Wallpaper[] = "Wallpaper";
static const char TileWallpaper[] = "TileWallpaper";
static const char SCRNSAVE_EXE[] = "SCRNSAVE.EXE";
static const char ScreenSaveTimeOut[] = "ScreenSaveTimeOut";
static const char DebuggerValueName[] = "Debugger";
static const char Auto[] = "Auto";
static const char CommonDesktop[] = "Common Desktop";
static const char CommonFavorites[] = "Common Favorites";
static const char CommonStartMenu[] = "Common Start Menu";
static const char CommonPrograms[] = "Common Programs";
static const char CommonStartup[] = "Common Startup";
static const char CommonAdministrativeTools[] = "Common Administrative Tools";
static const char CommonDocuments[] = "Common Documents";
static const char CommonTemplates[] = "Common Templates";
static const char CommonAppData[] = "Common AppData";
static const char ActiveXCache[] = "ActiveXCache";

static const char ShellFoldersKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
static const char UserShellFoldersKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders";
static const char AeDebugKey[] = "Software\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug";
static const char InternetSettingsKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";
static const char ActiveXCacheKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ActiveX Cache";

#define NO_DEBUG_CHANGE	"*"
inline bool canChangeDebugger( const char *val )
{
	return strcmp( val, NO_DEBUG_CHANGE );
}

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
	{ "Unknown", NO_DEBUG_CHANGE },
	{ "NONE", "" }
};


// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

static bool replaceEnvironment( STRING &valueStr, const char *env )
{
	char	*envVar = getenv( env );
	size_t	envLen = envVar ? strlen( envVar ) : 0;

	if( envLen && !valueStr.comparenI( envVar, envLen ) )
	{
		valueStr.replaceText( 0, envLen, STRING("%") + env + "%" );
		return true;
	}
	return false;
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
		if( desktopKey.readValue( Wallpaper, &valueBuff ) == rsOK )
			setEditText( IDE_WALLPAPER, valueBuff );

		if( desktopKey.readValue( TileWallpaper, &valueBuff ) == rsOK )
		{
			if( valueBuff[0U] == '0' )
				clrButtonCheck( IDC_TILE_WALL );
			else
				setButtonCheck( IDC_TILE_WALL );
		}

		if( desktopKey.readValue( SCRNSAVE_EXE, &valueBuff ) == rsOK )
			setEditText( IDE_SCREEN_SAVER, valueBuff );

		if( desktopKey.readValue( ScreenSaveTimeOut, &valueBuff ) == rsOK && !valueBuff.isEmpty())
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

	openResult = debuggerKey.openPublic( AeDebugKey );
	if( openResult == ERROR_SUCCESS )
	{
		if( debuggerKey.readValue( DebuggerValueName, &valueBuff ) == rsOK )
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
				debuggerCombo.selectEntry(arraySize( debuggerList )-2);
		}
		else
			debuggerCombo.selectEntry(arraySize( debuggerList )-1);

		if( debuggerKey.readValue( Auto, &valueBuff ) == rsOK )
		{
			if( valueBuff[0U] == '0' )
				askCheckBox.setActive();
			else
				askCheckBox.clrActive();
		}
	}
	else
		debuggerCombo.selectEntry(arraySize( debuggerList )-1);

	openResult = explorerKey.openPublic( ShellFoldersKey );
	if( openResult == ERROR_SUCCESS )
	{
		if( explorerKey.readValue( CommonDesktop, &valueBuff ) == rsOK )
		{
			setEditText( IDC_DESKTOP, valueBuff );
		}

		if( explorerKey.readValue( CommonFavorites, &valueBuff ) == rsOK )
		{
			setEditText( IDC_FAVORITES, valueBuff );
		}

		if( explorerKey.readValue( CommonStartMenu, &valueBuff ) == rsOK )
		{
			setEditText( IDC_COMMON_START, valueBuff );
		}

		if( explorerKey.readValue( CommonPrograms, &valueBuff ) == rsOK )
		{
			setEditText( IDC_COMMON_PROG, valueBuff );
		}

		if( explorerKey.readValue( CommonStartup, &valueBuff ) == rsOK )
		{
			setEditText( IDC_COMMON_AUTO, valueBuff );
		}

		if( explorerKey.readValue( CommonAdministrativeTools, &valueBuff ) == rsOK )
		{
			setEditText( IDC_ADMIN_MENU, valueBuff );
		}

		if( explorerKey.readValue( CommonDocuments, &valueBuff ) == rsOK )
		{
			setEditText( IDC_DOCUMENTS, valueBuff );
		}

		if( explorerKey.readValue( CommonTemplates, &valueBuff ) == rsOK )
		{
			setEditText( IDC_TEMPLATES, valueBuff );
		}

		if( explorerKey.readValue( CommonAppData, &valueBuff ) == rsOK )
		{
			setEditText( IDC_APP_FILES, valueBuff );
		}
	}

	openResult = internetKey.openPublic( InternetSettingsKey );
	if( openResult == ERROR_SUCCESS )
	{
		if( internetKey.readValue( ActiveXCache, &valueBuff ) == rsOK )
		{
			setEditText( IDC_DOWNLOAD, valueBuff );
		}

	}
	return psDO_DEFAULT;
}

#define WriteFolderSetting( editID, regKey )						\
{																	\
	valueStr = getEditText( editID );								\
	explorerKey1.writeValue( regKey, valueStr );					\
	replaceEnvironment( valueStr, "ALLUSERSPROFILE" )	||			\
	replaceEnvironment( valueStr, "PUBLIC" )			||			\
	replaceEnvironment( valueStr, "ProgramData" );					\
	valueEnv = valueStr;											\
	explorerKey2.writeValue( regKey, valueEnv );					\
}

ProcessStatus GlobalConfig::handleOk()
{
	long		openResult;
	STRING		valueStr;

	gak::uint8 major, minor;
	gak::uint16 build;
	getWindowsVersion(&major, &minor, &build );
	if( major <= 5 )	// windows XP or earlier
	{
		Registry	desktopKey;
		openResult = desktopKey.openUsers(	".default\\Control Panel\\Desktop", KEY_SET_VALUE|KEY_WOW64_64KEY );
		if( openResult == ERROR_SUCCESS )
		{
			valueStr = getEditText( IDE_WALLPAPER );
			desktopKey.writeValue( Wallpaper, valueStr );

			valueStr = isButtonChecked( IDC_TILE_WALL ) ? '1' : '0';
			desktopKey.writeValue( TileWallpaper, valueStr );

			valueStr = isButtonChecked( IDE_SCREEN_SAVER );
			desktopKey.writeValue( SCRNSAVE_EXE, valueStr );

			int timeOut = getEditText( IDE_SAVER_TIMEOUT ).getValueN<int>() * 60;
			valueStr = formatNumber( timeOut );
			desktopKey.writeValue( ScreenSaveTimeOut, valueStr );
		}
	}

	if( major == 6 && minor == 1 )	// check for win 7
	{
		Registry	backgroundKey;

		openResult = backgroundKey.openPublic(
			"Software\\Microsoft\\Windows\\CurrentVersion\\Authentication\\LogonUI\\Background",
			KEY_SET_VALUE|KEY_WOW64_64KEY
		);
		if( openResult == ERROR_SUCCESS )
		{
			long longValue = 0;
			valueStr = getEditText( IDE_WALLPAPER );

			if( !valueStr.isEmpty() )
			{
				STRING	newPath = getenv( "windir" );
				newPath += "\\system32\\oobe";
				mkdir( newPath );
				newPath += "\\info";
				mkdir( newPath );
				newPath += "\\backgrounds";
				mkdir( newPath );
				newPath += "\\backgroundDefault.jpg";

				fcopy( valueStr, newPath );

				longValue = 1;
			}

			openResult = backgroundKey.writeValue( "OEMBackground", longValue );
			doLogValueEx( gakLogging::llDetail, openResult );
		}
	}

	Registry	debuggerKey;
	openResult = debuggerKey.openPublic( AeDebugKey, KEY_SET_VALUE|KEY_WOW64_64KEY );
	if( openResult == ERROR_SUCCESS )
	{
		int selectedDebugger = debuggerCombo.getSelection();

		const char	*valuePtr = selectedDebugger < arraySize( debuggerList )
			? debuggerList[selectedDebugger].command
			: NO_DEBUG_CHANGE;

		if( canChangeDebugger( valuePtr ) )
		{
			debuggerKey.writeValue( DebuggerValueName, valuePtr );

			valuePtr = askCheckBox.isActive() ? "0" : "1";

			debuggerKey.writeValue(Auto, valuePtr);
		}
	}

	if( major <= 5 )	// windows XP or earlier
	{
		Registry	explorerKey1,
					explorerKey2;

		openResult = explorerKey1.openPublic( ShellFoldersKey, KEY_SET_VALUE|KEY_WOW64_64KEY );
		if( openResult == ERROR_SUCCESS )
		{
			openResult = explorerKey2.openPublic( UserShellFoldersKey, KEY_SET_VALUE|KEY_WOW64_64KEY );
			if( openResult == ERROR_SUCCESS )
			{
				EnvSTRING	valueEnv;

				WriteFolderSetting( IDC_DESKTOP, CommonDesktop );
				WriteFolderSetting( IDC_FAVORITES, CommonFavorites );
				WriteFolderSetting( IDC_COMMON_START, CommonStartMenu );
				WriteFolderSetting( IDC_COMMON_PROG, CommonPrograms );
				WriteFolderSetting( IDC_COMMON_AUTO, CommonStartup );
				WriteFolderSetting( IDC_ADMIN_MENU, CommonAdministrativeTools );
				WriteFolderSetting( IDC_DOCUMENTS, CommonDocuments );
				WriteFolderSetting( IDC_TEMPLATES, CommonTemplates );
				WriteFolderSetting( IDC_APP_FILES, CommonAppData );
			}
		}

		Registry	internetKey;

		openResult = internetKey.openPublic( InternetSettingsKey, KEY_SET_VALUE|KEY_WOW64_64KEY );
		if( openResult == ERROR_SUCCESS )
		{
			valueStr = getEditText( IDC_DOWNLOAD );
			internetKey.writeValue( ActiveXCache, valueStr );
		}

		openResult = internetKey.openPublic( ActiveXCacheKey, KEY_SET_VALUE|KEY_WOW64_64KEY );
		if( openResult == ERROR_SUCCESS )
		{
			valueStr = getEditText( IDC_DOWNLOAD );
			internetKey.writeValue( "0", valueStr );
			internetKey.deleteValue( "1" );
			internetKey.deleteValue( "2" );
			internetKey.deleteValue( "3" );
			internetKey.deleteValue( "4" );
		}
	}

	return psDO_DEFAULT;
}

ProcessStatus GlobalConfig::handleCommand( int cmd )
{
	char valueBuff[10240];

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
			char			directory[10240];
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

