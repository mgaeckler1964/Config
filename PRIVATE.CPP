/*
		Project:		CONFIG
		Module:			PRIVATE.H
		Description:	ConfigDialog
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

#include <memory>

#include <winlib/regedit.h>
#include <winlib/registry.h>

#include "private.h"

#include "global.h"
#include "autorun.h"
#include "assoc.h"
#include "directry.h"

#include "config.h"

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
#endif

using namespace winlib;
using namespace gak;

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

static const char ShellFoldersKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
static const char UserShellFoldersKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders";
static const char WinlogonKey[] = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon";
static const char AdvancedKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
static const char StreamsKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Streams";
static const char StreamMRUKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StreamMRU";
static const char LoadKey[] = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\Load";
static const char RunKey[] = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\Run";

static const char Desktop[] = "Desktop";
static const char Favorites[] = "Favorites";
static const char StartMenu[] = "Start Menu";
static const char Programs[] = "Programs";
static const char Startup[] = "Startup";
static const char AdministrativeTools[] = "Administrative Tools";
static const char Recent[] = "Recent";
static const char SendTo[] = "SendTo";
static const char Personal[] = "Personal";
static const char MyPictures[] = "My Pictures";
static const char MyVideo[] = "My Video";
static const char MyMusic[] = "My Music";
static const char Templates[] = "Templates";
static const char AppData[] = "AppData";
static const char LocalSettings[] = "Local Settings";
static const char LocalAppData[] = "Local AppData";
static const char Cache[] = "Cache";
static const char Cookies[] = "Cookies";
static const char History[] = "History";

static const char ParseAutoexec[] = "ParseAutoexec";
static const char RunLogonScriptSync[] = "RunLogonScriptSync";
static const char PersistBrowsers[] = "PersistBrowsers";

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus ConfigDialog::handleCreate()
{
	long		openResult;
	Registry	logonKey;
	Registry	explorerKey;
	STRING		valueBuff;
	long		valueWord;

	openResult = logonKey.openPrivate( WinlogonKey );
	if( openResult == ERROR_SUCCESS )
	{
		if( logonKey.readValue( ParseAutoexec, &valueBuff ) == rsOK )
		{
			if( valueBuff[0U] == '0' )
				clrButtonCheck( IDC_PARSE_AUTO );
			else
				setButtonCheck( IDC_PARSE_AUTO );
		}

		if( logonKey.readValue( RunLogonScriptSync, &valueWord ) == rsOK )
		{
			if( valueWord )
				setButtonCheck( IDC_WAIT_AUTO );
			else
				clrButtonCheck( IDC_WAIT_AUTO );
		}
	}


	openResult = explorerKey.openPrivate( AdvancedKey );
	if( openResult == ERROR_SUCCESS )
	{
		if( explorerKey.readValue( PersistBrowsers, &valueWord ) == rsOK )
		{
			if( valueWord )
				setButtonCheck( IDC_PERSIST_BROWSER );
			else
				clrButtonCheck( IDC_PERSIST_BROWSER );
		}
		explorerKey.close();
	}


	openResult = explorerKey.openPrivate( ShellFoldersKey );
	if( openResult == ERROR_SUCCESS )
	{
		if( explorerKey.readValue( Desktop, &valueBuff ) == rsOK )
		{
			setEditText( IDC_DESKTOP, valueBuff );
		}

		if( explorerKey.readValue( Favorites, &valueBuff ) == rsOK )
		{
			setEditText( IDC_FAVORITES, valueBuff );
		}

		if( explorerKey.readValue( StartMenu, &valueBuff ) == rsOK )
		{
			setEditText( IDC_START_MENU, valueBuff );
		}

		if( explorerKey.readValue( Programs, &valueBuff ) == rsOK )
		{
			setEditText( IDC_PROGRAM_MENU, valueBuff );
		}

		if( explorerKey.readValue( Startup, &valueBuff ) == rsOK )
		{
			setEditText( IDC_AUTO_START, valueBuff );
		}

		if( explorerKey.readValue( AdministrativeTools, &valueBuff ) == rsOK )
		{
			setEditText( IDC_ADMIN_MENU, valueBuff );
		}

		if( explorerKey.readValue( Recent, &valueBuff ) == rsOK )
		{
			setEditText( IDC_RECENT, valueBuff );
		}

		if( explorerKey.readValue( SendTo, &valueBuff ) == rsOK )
		{
			setEditText( IDC_SEND_TO, valueBuff );
		}

		if( explorerKey.readValue( Personal, &valueBuff ) == rsOK )
		{
			setEditText( IDC_DOCUMENTS, valueBuff );
		}

		if( explorerKey.readValue( MyPictures, &valueBuff ) == rsOK )
		{
			setEditText( IDC_PICTURES, valueBuff );
		}

		if( explorerKey.readValue( MyVideo, &valueBuff ) == rsOK )
		{
			setEditText( IDC_VIDEOS, valueBuff );
		}

		if( explorerKey.readValue( MyMusic, &valueBuff ) == rsOK )
		{
			setEditText( IDC_MUSIC, valueBuff );
		}

		if( explorerKey.readValue( Templates, &valueBuff ) == rsOK )
		{
			setEditText( IDC_TEMPLATES, valueBuff );
		}

		if( explorerKey.readValue( AppData, &valueBuff ) == rsOK )
		{
			setEditText( IDC_APP_FILES, valueBuff );
		}

		if( explorerKey.readValue( LocalSettings, &valueBuff ) == rsOK )
		{
			setEditText( IDC_LOCAL_SETTINGS, valueBuff );
		}

		if( explorerKey.readValue( LocalAppData, &valueBuff ) == rsOK )
		{
			setEditText( IDC_LAPP_FILES, valueBuff );
		}

		if( explorerKey.readValue( Cache, &valueBuff ) == rsOK )
		{
			setEditText( IDC_IE_CACHE, valueBuff );
		}

		if( explorerKey.readValue( Cookies, &valueBuff ) == rsOK )
		{
			setEditText( IDC_IE_COOKIES, valueBuff );
		}


		if( explorerKey.readValue( History, &valueBuff ) == rsOK )
		{
			setEditText( IDC_IE_HISTORY, valueBuff );
		}
	}

	return psDO_DEFAULT;
}

#define WriteFolderSetting( editID, regKey )						\
{																	\
	valueStr = getEditText( editID );								\
	explorerKey1.writeValue( regKey, valueStr );					\
	if( userLen && !valueStr.comparenI( userProfile, userLen ) )	\
	{																\
		valueStr.replaceText(0, userLen, "%USERPROFILE%");			\
	}																\
	valueEnv = valueStr;											\
	explorerKey2.writeValue( regKey, valueEnv );					\
}

ProcessStatus ConfigDialog::handleOk()
{
	long		openResult;
	Registry	logonKey;
	Registry	advKey;
	STRING		valueStr;
	EnvSTRING	valueEnv;
	long		valueWord;
	Registry	explorerKey1;		// user readable
	Registry	explorerKey2;		// environment strings (REG_EXPAND_SZ)

	/*
		I'm not sure whether this environment variable is standard or set by my own scripts
	*/
	const char	*userProfile = getenv( "USERPROFILE" );
	size_t		userLen = userProfile ? strlen( userProfile ) : 0;

	openResult = logonKey.openPrivate( WinlogonKey, KEY_SET_VALUE|KEY_WOW64_64KEY );
	if( openResult == ERROR_SUCCESS )
	{
		valueStr = isButtonChecked( IDC_PARSE_AUTO ) ? '1' : '0';
		logonKey.writeValue( ParseAutoexec, valueStr );

		valueWord = isButtonChecked( IDC_WAIT_AUTO ) ? 1 : 0;
		logonKey.writeValue( RunLogonScriptSync, valueWord );

		logonKey.close();
	}

	openResult = advKey.openPrivate( AdvancedKey, KEY_SET_VALUE|KEY_WOW64_64KEY );
	if( openResult == ERROR_SUCCESS )
	{
		valueWord = isButtonChecked( IDC_PERSIST_BROWSER ) ? 1 : 0;
		advKey.writeValue( PersistBrowsers, valueWord );
	}

	gak::int8 ver = getWindowsMajorVersion();
	if( ver <= 5 )	// windows XP or earlier
	{
		openResult = explorerKey1.openPrivate( ShellFoldersKey, KEY_SET_VALUE|KEY_WOW64_64KEY );
		if( openResult == ERROR_SUCCESS )
		{
			openResult = explorerKey2.openPrivate( UserShellFoldersKey, KEY_SET_VALUE|KEY_WOW64_64KEY );
			if( openResult == ERROR_SUCCESS )
			{
				WriteFolderSetting(IDC_DESKTOP, Desktop);
				WriteFolderSetting(IDC_FAVORITES, Favorites);
				WriteFolderSetting(IDC_START_MENU, StartMenu);
				WriteFolderSetting(IDC_PROGRAM_MENU, Programs);
				WriteFolderSetting(IDC_AUTO_START, Startup);
				WriteFolderSetting(IDC_ADMIN_MENU, AdministrativeTools);
				WriteFolderSetting(IDC_RECENT, Recent);
				WriteFolderSetting(IDC_SEND_TO, SendTo);
				WriteFolderSetting(IDC_DOCUMENTS, Personal);
				WriteFolderSetting(IDC_PICTURES, MyPictures);
				WriteFolderSetting(IDC_VIDEOS, MyVideo);
				WriteFolderSetting(IDC_MUSIC, MyMusic);
				WriteFolderSetting(IDC_TEMPLATES, Templates);
				WriteFolderSetting(IDC_APP_FILES, AppData);
				WriteFolderSetting(IDC_LOCAL_SETTINGS, LocalSettings);
				WriteFolderSetting(IDC_LAPP_FILES, LocalAppData);
				WriteFolderSetting(IDC_IE_CACHE, Cache);
				WriteFolderSetting(IDC_IE_COOKIES, Cookies);
				WriteFolderSetting(IDC_IE_HISTORY, History);
			}
		}
	}

	return psDO_DEFAULT;
}

ProcessStatus ConfigDialog::handleCommand( int cmd )
{
	char valueBuff[1024];

	switch( cmd )
	{
		case GLOBAL_CONFIG:
		{
			GlobalConfig dialog;

			dialog.create( this );
/*v*/		break;
		}
		case AUTORUN_DIALOG:
		{
			AutorunDialog	dialog(false);

			dialog.create( this );
/*v*/		break;
		}
		case NT_AUTO_LOAD:
		{
			std::auto_ptr<RegistryEditor>	theDialog(new RegistryEditor);

			theDialog->create( this, LoadKey );

/*v*/		break;
		}
		case NT_AUTO_RUN:
		{
			std::auto_ptr<RegistryEditor>	theDialog(new RegistryEditor);

			theDialog->create( this, RunKey );

/*v*/		break;
		}

		case IDC_EXPLORER_RESET:
		{
			long			errorCode;
			Registry		streams;
			char			buffer[1024];

			errorCode = streams.openPrivate( StreamsKey, KEY_ALL_ACCESS|KEY_WOW64_64KEY );
			if( errorCode == ERROR_SUCCESS )
			{
				for(
					Registry::key_iterator it = streams.kbegin(), endIT = streams.kend();
					it != endIT && errorCode == ERROR_SUCCESS;
					++it
				)
				{
					if( isdigit( (*it)[0U] ) )
						errorCode = streams.deleteSubkey( *it );
				}
				streams.close();
			}

			if( errorCode != ERROR_SUCCESS )
			{
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, buffer, sizeof( buffer ), NULL );
				messageBox( buffer, "Fehler", MB_ICONSTOP );
			}

			errorCode = streams.openPrivate( StreamMRUKey, KEY_ALL_ACCESS|KEY_WOW64_64KEY );
			if( errorCode == ERROR_SUCCESS )
			{
				for(
					Registry::key_iterator it = streams.kbegin(), endIT = streams.kend();
					it != endIT && errorCode == ERROR_SUCCESS;
					++it
				)
				{
					errorCode = streams.deleteSubkey( *it );
				}
			}
			if( errorCode != ERROR_SUCCESS )
			{
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, buffer, sizeof( buffer ), NULL );
				messageBox( buffer, "Fehler", MB_ICONSTOP );
			}
/*v*/		break;
		}

		case ASSOCIATION_DIALOG:
		{
			AssociationsConfig	dialog(false);

			dialog.create( this );
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
		case IDC_START_BROWSE:
		{
			getEditText( IDC_START_MENU, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_START_MENU, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_PRG_BROWSE:
		{
			getEditText( IDC_PROGRAM_MENU, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_PROGRAM_MENU, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_AUTO_BROWSE:
		{
			getEditText( IDC_AUTO_START, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_AUTO_START, dialog.getDirectory() );

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
		case IDC_RECENT_BROWSE:
		{
			getEditText( IDC_RECENT, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_RECENT, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_SEND_BROWSE:
		{
			getEditText( IDC_SEND_TO, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_SEND_TO, dialog.getDirectory() );

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
		case IDC_PICTURES_BROWSE:
		{
			getEditText( IDC_PICTURES, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_PICTURES, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_VIDEOS_BROWSE:
		{
			getEditText( IDC_VIDEOS, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_VIDEOS, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_MUSIC_BROWSE:
		{
			getEditText( IDC_MUSIC, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_MUSIC, dialog.getDirectory() );

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
		case IDC_LOCSET_BROWSE:
		{
			getEditText( IDC_LOCAL_SETTINGS, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_LOCAL_SETTINGS, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_LAPP_BROWSE:
		{
			getEditText( IDC_LAPP_FILES, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_LAPP_FILES, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_IE_CACHE_BROWSE:
		{
			getEditText( IDC_IE_CACHE, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_IE_CACHE, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_IE_COOKIES_BROWSE:
		{
			getEditText( IDC_IE_COOKIES, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_IE_COOKIES, dialog.getDirectory() );

/*v*/		break;
		}
		case IDC_IE_HISTORY_BROWSE:
		{
			getEditText( IDC_IE_HISTORY, valueBuff, sizeof( valueBuff ) );
			DirectoryDialog	dialog( valueBuff );
			if( dialog.create( this ) == IDOK )
				setEditText( IDC_IE_HISTORY, dialog.getDirectory() );

/*v*/		break;
		}
		default:
			return ApplicationDialog::handleCommand( cmd );
	}

	return psPROCESSED;
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

