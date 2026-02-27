/*
		Project:		CONFIG
		Module:			AUTORUN.CPP
		Description:	Auto run settings
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

#include "autorun.h"

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

class AUTORUN_COMMAND : public ListObject
{
	public:
	STRING	commandLine;
	BOOL	enabled;

	AUTORUN_COMMAND( const STRING &title, const STRING &commandLine, BOOL enabled ) : ListObject( title )
	{
		this->commandLine = commandLine;
		this->enabled = enabled;
	}

};

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void AutorunDialog::loadAutoRunData( BOOL enabled, ListboxContainer &data )
{
	HKEY			runKey;
	LONG			result;
	DWORD			valueSize, dataSize, type;

	DWORD			i;
	char			valueName[1024];
	char			regData[1024];
	AUTORUN_COMMAND	*cmd;

	STRING	theKeyPath = "Software\\Microsoft\\Windows\\CurrentVersion\\";

	theKeyPath += enabled ? "Run" : "DisabledRun";

	result = RegOpenKey( rootKey, theKeyPath, &runKey );
	if( result == ERROR_SUCCESS )
	{
		i = 0;
		while( 1 )
		{
			valueSize = sizeof( valueName );
			dataSize  = sizeof( regData );

			if( RegEnumValue( runKey, i++, valueName, &valueSize, NULL, &type, (LPBYTE)regData, &dataSize ) != ERROR_SUCCESS )
				break;

			cmd = new AUTORUN_COMMAND( valueName, regData, enabled );
			data.addEntry( cmd );
		}

		RegCloseKey( runKey );
	}
}


// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus AutorunDialog::handleCreate( void )
{
	enabledBox.initHandle( this, IDC_ENABLED_LIST );
	enabledData.setControl( &enabledBox );

	disabledBox.initHandle( this, IDC_DISABLED_LIST );
	disabledData.setControl( &disabledBox );

	commandLine.initHandle( this, IDC_COMMANDLINE );
	commandLine.setText("");

	loadAutoRunData( TRUE, enabledData );
	loadAutoRunData( FALSE, disabledData );

	return psDO_DEFAULT;
}

ProcessStatus AutorunDialog::handleOk( void )
{
	HKEY			enabledRun;
	HKEY			disabledRun;

	LONG			result;

	size_t			i;
	AUTORUN_COMMAND	*cmd;

	result = RegCreateKey( rootKey,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &enabledRun );
	if( result == ERROR_SUCCESS )
	{
		result = RegCreateKey( rootKey,
			"Software\\Microsoft\\Windows\\CurrentVersion\\DisabledRun", &disabledRun );
		if( result == ERROR_SUCCESS )
		{
			i = disabledData.size();
			while( i>0 )
			{
				i--;
				cmd = static_cast<AUTORUN_COMMAND *>(disabledData[i]);
				if( cmd->enabled )
				{
					STRING	valueName = *cmd;

					// delete from enabled list
					RegDeleteValue( enabledRun, valueName );

					// add to disabled list
					RegSetValueEx(
						disabledRun,
						valueName,
						0,
						REG_SZ,
						(LPBYTE)((const char *)cmd->commandLine),
						DWORD(cmd->commandLine.strlen())
					);
				}
			}

			i = enabledData.size();
			while( i>0 )
			{
				i--;
				cmd = static_cast<AUTORUN_COMMAND *>(enabledData[i]);
				if( !cmd->enabled )
				{
					STRING	valueName = *cmd;

					// delete from disabled list
					RegDeleteValue( disabledRun, valueName );

					// add to enabled list
					RegSetValueEx(
						enabledRun,
						valueName,
						0,
						REG_SZ,
						(LPBYTE)((const char *)cmd->commandLine),
						DWORD(cmd->commandLine.strlen())
					);
				}
			}


			RegCloseKey( disabledRun );
		}

		RegCloseKey( enabledRun );
	}

	return psDO_DEFAULT;
}

ProcessStatus AutorunDialog::handleCommand( int cmd )
{
	AUTORUN_COMMAND		*runCommand;

	switch( cmd )
	{
		case IDC_ENABLE:
			runCommand = (AUTORUN_COMMAND *)disabledData.getSelectedEntry();
			if( runCommand )
			{
				disabledData.removeEntry(runCommand);
				enabledData.addEntry(runCommand);
			}
			break;

		case IDC_DISABLE:
			runCommand = (AUTORUN_COMMAND *)enabledData.getSelectedEntry();
			if( runCommand )
			{
				enabledData.removeEntry(runCommand);
				disabledData.addEntry(runCommand);
			}
			break;

		case IDC_ENABLED_LIST:
			runCommand = (AUTORUN_COMMAND *)enabledData.getSelectedEntry();
			if( runCommand )
			{
				commandLine.setText( runCommand->commandLine );
			}
			break;

		case IDC_DISABLED_LIST:
			runCommand = (AUTORUN_COMMAND *)disabledData.getSelectedEntry();
			if( runCommand )
			{
				commandLine.setText( runCommand->commandLine );
			}
			break;

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

