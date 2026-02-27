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

#include <winlib\regedit.h>

#include "private.h"

#include "global.h"
#include "autorun.h"

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

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus ConfigDialog::handleCreate( void )
{
	long		openResult;
	HKEY		logonKey;
	HKEY		explorerKey;
	DWORD		valueType;
	DWORD		valueSize;
	char		valueBuff[1024];
	DWORD		valueWord;

	openResult = RegOpenKeyEx(	HKEY_CURRENT_USER,
								"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon", 0,
								KEY_READ, &logonKey );
	if( openResult == ERROR_SUCCESS )
	{
		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( logonKey, "ParseAutoexec", NULL,
										&valueType, (LPBYTE)valueBuff, &valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			if( *valueBuff == '0' )
				clrButtonCheck( IDC_PARSE_AUTO );
			else
				setButtonCheck( IDC_PARSE_AUTO );
		}

		valueSize = sizeof( valueWord );
		valueType = REG_DWORD;
		openResult = RegQueryValueEx( logonKey, "RunLogonScriptSync", NULL,
										&valueType, (LPBYTE)&valueWord,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_DWORD )
		{
			if( valueWord )
				setButtonCheck( IDC_WAIT_AUTO );
			else
				clrButtonCheck( IDC_WAIT_AUTO );
		}

		RegCloseKey( logonKey );
	}


	openResult = RegOpenKeyEx(	HKEY_CURRENT_USER,
								"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", 0,
								KEY_READ, &explorerKey );
	if( openResult == ERROR_SUCCESS )
	{
		valueSize = sizeof( valueWord );
		valueType = REG_DWORD;
		openResult = RegQueryValueEx( explorerKey, "PersistBrowsers", NULL,
										&valueType, (LPBYTE)&valueWord,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_DWORD )
		{
			if( valueWord )
				setButtonCheck( IDC_PERSIST_BROWSER );
			else
				clrButtonCheck( IDC_PERSIST_BROWSER );
		}

		RegCloseKey( explorerKey );
	}


	openResult = RegOpenKeyEx(	HKEY_CURRENT_USER,
								"Software\\Microsoft\\Windows\\CurrentVersion\\"
									"Explorer\\Shell Folders", 0,
								KEY_READ, &explorerKey );
	if( openResult == ERROR_SUCCESS )
	{
		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Desktop", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_DESKTOP, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Favorites", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_FAVORITES, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Start Menu", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_START_MENU, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Programs", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_PROGRAM_MENU, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Startup", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_AUTO_START, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Administrative Tools", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_ADMIN_MENU, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Recent", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_RECENT, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "SendTo", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_SEND_TO, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Personal", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_DOCUMENTS, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "My Pictures", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_PICTURES, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "My Video", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_VIDEOS, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "My Music", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_MUSIC, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Templates", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_TEMPLATES, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "AppData", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_APP_FILES, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "Local Settings", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_LOCAL_SETTINGS, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( 
			explorerKey, "Local AppData", NULL,
			&valueType, (LPBYTE)valueBuff,
			&valueSize 
		);
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_LAPP_FILES, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( 
			explorerKey, "Cache", NULL,
			&valueType, (LPBYTE)valueBuff,
			&valueSize 
		);
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_IE_CACHE, valueBuff );
		}

		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( 
			explorerKey, "Cookies", NULL,
			&valueType, (LPBYTE)valueBuff,
			&valueSize 
		);
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_IE_COOKIES, valueBuff );
		}


		valueSize = sizeof( valueBuff );
		valueType = REG_SZ;
		openResult = RegQueryValueEx( explorerKey, "History", NULL,
										&valueType, (LPBYTE)valueBuff,
										&valueSize );
		if( openResult == ERROR_SUCCESS
		&&  valueSize > 0
		&&  valueType == REG_SZ )
		{
			setEditText( IDC_IE_HISTORY, valueBuff );
		}

		RegCloseKey( explorerKey );
	}

	return psDO_DEFAULT;
}

ProcessStatus ConfigDialog::handleOk( void )
{
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

	openResult = RegOpenKeyEx(	HKEY_CURRENT_USER,
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

		RegCloseKey( logonKey );
	}

	openResult = RegOpenKeyEx(	HKEY_CURRENT_USER,
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

		RegCloseKey( logonKey );
	}

	openResult = RegOpenKeyEx(	HKEY_CURRENT_USER,
								"Software\\Microsoft\\Windows\\CurrentVersion\\"
									"Explorer\\Shell Folders", 0,
								KEY_SET_VALUE, &explorerKey1 );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = RegOpenKeyEx(	HKEY_CURRENT_USER,
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

			RegCloseKey( explorerKey2 );
		}
		RegCloseKey( explorerKey1 );
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
			AutorunDialog	dialog(HKEY_CURRENT_USER);

			dialog.create( this );
/*v*/		break;
		}
		case NT_AUTO_LOAD:
		{
			RegistryEditor	*theDialog = new RegistryEditor;

			theDialog->create( this, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\Load" );

			delete theDialog;

/*v*/		break;
		}
		case NT_AUTO_RUN:
		{
			RegistryEditor	*theDialog = new RegistryEditor;

			theDialog->create( this, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows\\Run" );

			delete theDialog;

/*v*/		break;
		}

		case IDC_EXPLORER_RESET:
		{
			long	errorCode;
			unsigned long	dummy;
			HKEY	streams;
			char	buffer[1024];
			int		i;

			errorCode = RegOpenKey( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Streams", &streams );
			i = 0;
			while( errorCode == ERROR_SUCCESS )
			{
				errorCode = RegEnumKey( streams, i, buffer, sizeof( buffer ) );
				if( errorCode == ERROR_SUCCESS && isdigit( *buffer ) )
					errorCode = RegDeleteKey( streams, buffer );
				else
					i++;
			}
			RegCloseKey( streams );
			if( errorCode != ERROR_SUCCESS && errorCode != 259 )
			{
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, buffer, sizeof( buffer ), NULL );
				MessageBox( handle(), buffer, "Fehler", MB_ICONSTOP );
			}

			errorCode = RegOpenKey( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StreamMRU", &streams );
			i = 0;
			while( errorCode == ERROR_SUCCESS )
			{
				dummy = sizeof( buffer );
				errorCode = RegEnumValue( streams, i, buffer, &dummy, NULL, NULL, NULL, NULL );
				if( errorCode == ERROR_SUCCESS )
					errorCode = RegDeleteValue( streams, buffer );
			}
			RegCloseKey( streams );
			if( errorCode != ERROR_SUCCESS && errorCode != 259 )
			{
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, buffer, sizeof( buffer ), NULL );
				MessageBox( handle(), buffer, "Fehler", MB_ICONSTOP );
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

