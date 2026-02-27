/*
		Project:		CONFIG
		Module:			COMMAND.H
		Description:	CommandDialog
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

#include <winlib\dialogwi.h>

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
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class FILE_COMMAND : public ListObject
{
	STRING	fileType;
	STRING	identifier;
	STRING	menuText;
	STRING	commandLine;
	STRING	ddeCommand;
	STRING	ddeIfRunning;
	STRING	application;
	STRING	topic;

	void loadCommand( void );

	public:
	void saveCommand( void );
	FILE_COMMAND( const STRING &type, const STRING &command )
	{
		fileType = type;
		identifier = command;
		loadCommand();
	}
	FILE_COMMAND( const STRING &type )
	{
		fileType = type;
	}
	STRING getIdentifier( void ) const
	{
		return identifier;
	}
	STRING getMenuText( void ) const
	{
		return menuText;
	}
	STRING getCommandLine( void ) const
	{
		return commandLine;
	}
	STRING getDDEcommand( void ) const
	{
		return ddeCommand;
	}
	STRING getDDErunCommand( void ) const
	{
		return ddeIfRunning;
	}
	STRING getApplication( void ) const
	{
		return application;
	}
	STRING getTopic( void ) const
	{
		return topic;
	}

	void setIdentifier( const STRING &newValue )
	{
		identifier = newValue;
	}
	void setMenuText( const STRING &newValue )
	{
		menuText = newValue;
	}
	void setCommandLine( const STRING &newValue )
	{
		commandLine = newValue;
	}
	void setDDEcommand( const STRING &newValue )
	{
		ddeCommand = newValue;
	}
	void setDDErunCommand( const STRING &newValue )
	{
		ddeIfRunning = newValue;
	}
	void setApplication( const STRING &newValue )
	{
		application = newValue;
	}
	void setTopic( const STRING &newValue )
	{
		topic = newValue;
	}
	void setValue( void )
	{
		if( menuText[0U] )
			ListObject::setValue( menuText );
		else
			ListObject::setValue( identifier );
	}
};

class CommandDialog : public ModalDialog
{
	FILE_COMMAND	*command;

	virtual ProcessStatus handleCreate( void );
	virtual ProcessStatus handleOk( void );
	virtual ProcessStatus handleCommand( int cmd );

	public:
	CommandDialog( FILE_COMMAND *newCommand )
	{
		command = newCommand;
	}
	int create( const BasicWindow *parent )
	{
		return ModalDialog::create( COMMANDLINE_DIALOG, parent );
	}
};

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

