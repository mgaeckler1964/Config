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

#include <WINLIB/registry.h>

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

struct AutorunCommand : public ListObject
{
	public:
	STRING	commandLine;
	bool	enabled;

	AutorunCommand( const STRING &title, const STRING &commandLine, bool enabled ) : 
		ListObject( title ), commandLine(commandLine), enabled(enabled)
	{
	}

};

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void AutorunDialog::loadAutoRunData( bool enabled, ListboxContainer &data )
{
	Registry		runKey;

	STRING	theKeyPath = "Software\\Microsoft\\Windows\\CurrentVersion\\";

	theKeyPath += enabled ? "Run" : "DisabledRun";

	long result = runKey.openKey( m_publicKey, theKeyPath );
	if( result == ERROR_SUCCESS )
	{
		for( 
			Registry::value_iterator it = runKey.vbegin(), endIT = runKey.vend();
			it != endIT;
			++it
		)
		{
			if( it->type == rtSTRING )
			{
				AutorunCommand	*cmd = new AutorunCommand( it->valueName, it->valueBuffer, enabled );
				data.addEntry( cmd );
			}
		}
	}
}


// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus AutorunDialog::handleCreate()
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

ProcessStatus AutorunDialog::handleOk()
{
	Registry		enabledRun;
	Registry		disabledRun;

	long			result;

	result = enabledRun.createKey( m_publicKey, "Software\\Microsoft\\Windows\\CurrentVersion\\Run" );
	if( result == ERROR_SUCCESS )
	{
		result = disabledRun.createKey( m_publicKey, "Software\\Microsoft\\Windows\\CurrentVersion\\DisabledRun" );
		if( result == ERROR_SUCCESS )
		{
			size_t i = disabledData.size();
			while( i>0 )
			{
				i--;
				AutorunCommand	*cmd = static_cast<AutorunCommand *>(disabledData[i]);
				if( cmd->enabled )
				{
					STRING	valueName = *cmd;

					// delete from enabled list
					enabledRun.deleteValue( valueName );

					// add to disabled list
					disabledRun.writeValue( valueName, cmd->commandLine );
				}
			}

			i = enabledData.size();
			while( i>0 )
			{
				i--;
				AutorunCommand *cmd = static_cast<AutorunCommand *>(enabledData[i]);
				if( !cmd->enabled )
				{
					STRING	valueName = *cmd;

					// delete from disabled list
					disabledRun.deleteValue( valueName );

					// add to enabled list
					enabledRun.writeValue( valueName, cmd->commandLine );
				}
			}
		}
	}

	return psDO_DEFAULT;
}

ProcessStatus AutorunDialog::handleSelectionChange( int cmd )
{
	AutorunCommand	*runCommand;

	switch( cmd )
	{
		case IDC_ENABLED_LIST:
			runCommand = static_cast<AutorunCommand *>(enabledData.getSelectedEntry());
			if( runCommand )
			{
				commandLine.setText( runCommand->commandLine );
			}
			break;

		case IDC_DISABLED_LIST:
			runCommand = static_cast<AutorunCommand *>(disabledData.getSelectedEntry());
			if( runCommand )
			{
				commandLine.setText( runCommand->commandLine );
			}
			break;

		default:
			return ModalDialog::handleSelectionChange( cmd );
	}

	return psPROCESSED;
}

ProcessStatus AutorunDialog::handleCommand( int cmd )
{
	AutorunCommand		*runCommand;

	switch( cmd )
	{
		case IDC_ENABLE:
			runCommand = static_cast<AutorunCommand *>(disabledData.getSelectedEntry());
			if( runCommand )
			{
				disabledData.removeEntry(runCommand);
				enabledData.addEntry(runCommand);
			}
			break;

		case IDC_DISABLE:
			runCommand = static_cast<AutorunCommand *>(enabledData.getSelectedEntry());
			if( runCommand )
			{
				enabledData.removeEntry(runCommand);
				disabledData.addEntry(runCommand);
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

