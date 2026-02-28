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
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus ConfigDialog::handleCreate()
{
	long		openResult;
	Registry	logonKey;
	Registry	explorerKey;
	STRING		valueBuff;
	long		valueWord;

	openResult = logonKey.openPrivate( "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon" );
	if( openResult == ERROR_SUCCESS )
	{
		if( logonKey.readValue( "ParseAutoexec", &valueBuff ) == rsOK )
		{
			if( valueBuff[0U] == '0' )
				clrButtonCheck( IDC_PARSE_AUTO );
			else
				setButtonCheck( IDC_PARSE_AUTO );
		}

		if( logonKey.readValue( "RunLogonScriptSync", &valueWord ) == rsOK )
		{
			if( valueWord )
				setButtonCheck( IDC_WAIT_AUTO );
			else
				clrButtonCheck( IDC_WAIT_AUTO );
		}
	}


	openResult = explorerKey.openPrivate( "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced" );
	if( openResult == ERROR_SUCCESS )
	{
		if( explorerKey.readValue( "PersistBrowsers", &valueWord ) == rsOK )
		{
			if( valueWord )
				setButtonCheck( IDC_PERSIST_BROWSER );
			else
				clrButtonCheck( IDC_PERSIST_BROWSER );
		}
		explorerKey.close();
	}


	openResult = explorerKey.openPrivate( 
			"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
	);
	if( openResult == ERROR_SUCCESS )
	{
		if( explorerKey.readValue( "Desktop", &valueBuff ) == rsOK )
		{
			setEditText( IDC_DESKTOP, valueBuff );
		}

		if( explorerKey.readValue( "Favorites", &valueBuff ) == rsOK )
		{
			setEditText( IDC_FAVORITES, valueBuff );
		}

		if( explorerKey.readValue( "Start Menu", &valueBuff ) == rsOK )
		{
			setEditText( IDC_START_MENU, valueBuff );
		}

		if( explorerKey.readValue( "Programs", &valueBuff ) == rsOK )
		{
			setEditText( IDC_PROGRAM_MENU, valueBuff );
		}

		if( explorerKey.readValue( "Startup", &valueBuff ) == rsOK )
		{
			setEditText( IDC_AUTO_START, valueBuff );
		}

		if( explorerKey.readValue( "Administrative Tools", &valueBuff ) == rsOK )
		{
			setEditText( IDC_ADMIN_MENU, valueBuff );
		}

		if( explorerKey.readValue( "Recent", &valueBuff ) == rsOK )
		{
			setEditText( IDC_RECENT, valueBuff );
		}

		if( explorerKey.readValue( "SendTo", &valueBuff ) == rsOK )
		{
			setEditText( IDC_SEND_TO, valueBuff );
		}

		if( explorerKey.readValue( "Personal", &valueBuff ) == rsOK )
		{
			setEditText( IDC_DOCUMENTS, valueBuff );
		}

		if( explorerKey.readValue( "My Pictures", &valueBuff ) == rsOK )
		{
			setEditText( IDC_PICTURES, valueBuff );
		}

		if( explorerKey.readValue( "My Video", &valueBuff ) == rsOK )
		{
			setEditText( IDC_VIDEOS, valueBuff );
		}

		if( explorerKey.readValue( "My Music", &valueBuff ) == rsOK )
		{
			setEditText( IDC_MUSIC, valueBuff );
		}

		if( explorerKey.readValue( "Templates", &valueBuff ) == rsOK )
		{
			setEditText( IDC_TEMPLATES, valueBuff );
		}

		if( explorerKey.readValue( "AppData", &valueBuff ) == rsOK )
		{
			setEditText( IDC_APP_FILES, valueBuff );
		}

		if( explorerKey.readValue( "Local Settings", &valueBuff ) == rsOK )
		{
			setEditText( IDC_LOCAL_SETTINGS, valueBuff );
		}

		if( explorerKey.readValue( "Local AppData", &valueBuff ) == rsOK )
		{
			setEditText( IDC_LAPP_FILES, valueBuff );
		}

		if( explorerKey.readValue( "Cache", &valueBuff ) == rsOK )
		{
			setEditText( IDC_IE_CACHE, valueBuff );
		}

		if( explorerKey.readValue( "Cookies", &valueBuff ) == rsOK )
		{
			setEditText( IDC_IE_COOKIES, valueBuff );
		}


		if( explorerKey.readValue( "History", &valueBuff ) == rsOK )
		{
			setEditText( IDC_IE_HISTORY, valueBuff );
		}
	}

	return psDO_DEFAULT;
}

ProcessStatus ConfigDialog::handleOk()
{
#if 0											// actually disabled

	long		openResult;
	HKEY		logonKey;
	HKEY		explorerKey1;
	HKEY		explorerKey2;
	DWORD		valueSize;
	char		valueBuff[1024];
	DWORD		valueWord;

	STRING		tmpBuff;

	char	*userProfile = getenv( "USERPROFILE" );
	size_t	userLen = userProfile ? strlen( userProfile ) : 0;

	openResult = RegOpenKeyEx(	_HKEY_CURRENT_USER,
								"Software\\Microsoft\\Windows NT\\"
									"CurrentVersion\\Winlogon", 0,
								KEY_SET_VALUE, &logonKey );
	if( openResult == ERROR_SUCCESS )
	{
		if( isButtonChecked( IDC_PARSE_AUTO ) )
			strcpy( valueBuff, "1" );
		else
			strcpy( valueBuff, "0" );
		valueSize = DWORD(strlen( valueBuff ) + 1);

		RegSetValueEx( logonKey, "ParseAutoexec", 0,
						REG_SZ, (LPBYTE)valueBuff, valueSize );

		if( isButtonChecked( IDC_WAIT_AUTO ) )
			valueWord = 1;
		else
			valueWord = 0;
		valueSize = sizeof( valueWord );

		RegSetValueEx( logonKey, "RunLogonScriptSync", 0,
						REG_DWORD, (LPBYTE)&valueWord, valueSize );

		_RegCloseKey( logonKey );
	}

	openResult = RegOpenKeyEx(	_HKEY_CURRENT_USER,
								"Software\\Microsoft\\Windows\\"
									"CurrentVersion\\Explorer\\Advanced", 0,
								KEY_SET_VALUE, &logonKey );
	if( openResult == ERROR_SUCCESS )
	{
		if( isButtonChecked( IDC_PERSIST_BROWSER ) )
			valueWord = 1;
		else
			valueWord = 0;
		valueSize = sizeof( valueWord );

		RegSetValueEx( logonKey, "PersistBrowsers", 0,
						REG_DWORD, (LPBYTE)&valueWord, valueSize );

		_RegCloseKey( logonKey );
	}

	openResult = RegOpenKeyEx(	_HKEY_CURRENT_USER,
								"Software\\Microsoft\\Windows\\CurrentVersion\\"
									"Explorer\\Shell Folders", 0,
								KEY_SET_VALUE, &explorerKey1 );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = RegOpenKeyEx(	_HKEY_CURRENT_USER,
								"Software\\Microsoft\\Windows\\CurrentVersion\\"
									"Explorer\\User Shell Folders", 0,
								KEY_SET_VALUE, &explorerKey2 );
		if( openResult == ERROR_SUCCESS )
		{
			getEditText( IDC_DESKTOP, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Desktop", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Desktop", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_FAVORITES, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Favorites", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Favorites", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_START_MENU, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Start Menu", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Start Menu", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_PROGRAM_MENU, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Programs", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Programs", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_AUTO_START, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Startup", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Startup", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_ADMIN_MENU, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Administrative Tools", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Administrative Tools", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_RECENT, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Recent", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Recent", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_SEND_TO, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "SendTo", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "SendTo", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_DOCUMENTS, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Personal", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Personal", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_PICTURES, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "My Pictures", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "My Pictures", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_VIDEOS, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "My Video", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "My Video", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_MUSIC, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "My Music", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "My Music", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_TEMPLATES, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Templates", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Templates", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_APP_FILES, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "AppData", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "AppData", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_LOCAL_SETTINGS, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Local Settings", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Local Settings", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_LAPP_FILES, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Local AppData", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Local AppData", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_IE_CACHE, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Cache", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Cache", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_IE_COOKIES, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "Cookies", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "Cookies", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			getEditText( IDC_IE_HISTORY, valueBuff, sizeof( valueBuff ) );
			valueSize = DWORD(strlen( valueBuff ) + 1);
			RegSetValueEx( explorerKey1, "History", 0,
							REG_SZ, (LPBYTE)valueBuff, valueSize );

			if( userLen && !strncmpi( valueBuff, userProfile, userLen ) )
			{
				tmpBuff = "%USERPROFILE%";
				tmpBuff += valueBuff + userLen;
				strcpy( valueBuff, tmpBuff );
				valueSize = DWORD(strlen( valueBuff ) + 1);
			}
			RegSetValueEx( explorerKey2, "History", 0,
							REG_EXPAND_SZ, (LPBYTE)valueBuff, valueSize );

			_RegCloseKey( explorerKey2 );
		}
		_RegCloseKey( explorerKey1 );
	}

#endif
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

			theDialog->create( this, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\Load" );

/*v*/		break;
		}
		case NT_AUTO_RUN:
		{
			std::auto_ptr<RegistryEditor>	theDialog(new RegistryEditor);

			theDialog->create( this, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\Run" );

/*v*/		break;
		}

		case IDC_EXPLORER_RESET:
		{
			long			errorCode;
			Registry		streams;
			char			buffer[1024];

			errorCode = streams.openPrivate( "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Streams" );
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

			errorCode = streams.openKey( false, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StreamMRU" );
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

