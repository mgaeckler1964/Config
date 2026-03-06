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
#	pragma option -a4
#	pragma option -pc
#endif

using namespace winlib;

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class FileCommand : public ListObject
{
	STRING	m_fileType;			// the file type ...\Software\classes\<type>
	STRING	m_identifier;		// the key name ...\Software\classes\<type>\shell\<key>
	STRING	m_menuText;			// the value of the key item or if empty the key name itself
	STRING	m_commandLine;
	STRING	m_ddeCommand;
	STRING	m_ddeIfRunning;
	STRING	m_application;
	STRING	m_topic;
	bool	m_publicKey;

	void loadCommand();

	public:
	void saveCommand();
	FileCommand( const STRING &type, const STRING &command, bool publicKey ) 
		: m_fileType(type), m_identifier(command), m_publicKey(publicKey)
	{
		loadCommand();
	}
	FileCommand( const STRING &type, bool publicKey ) : m_fileType(type), m_publicKey(publicKey)
	{
	}
	const STRING &getIdentifier() const
	{
		return m_identifier;
	}
	const STRING &getMenuText() const
	{
		return m_menuText;
	}
	const STRING &getCommandLine() const
	{
		return m_commandLine;
	}
	const STRING &getDDEcommand() const
	{
		return m_ddeCommand;
	}
	const STRING &getDDErunCommand() const
	{
		return m_ddeIfRunning;
	}
	const STRING &getApplication() const
	{
		return m_application;
	}
	const STRING &getTopic() const
	{
		return m_topic;
	}

	void setIdentifier( const STRING &newValue )
	{
		m_identifier = newValue;
	}
	void setMenuText( const STRING &newValue )
	{
		m_menuText = newValue;
	}
	void setCommandLine( const STRING &newValue )
	{
		m_commandLine = newValue;
	}
	void setDDEcommand( const STRING &newValue )
	{
		m_ddeCommand = newValue;
	}
	void setDDErunCommand( const STRING &newValue )
	{
		m_ddeIfRunning = newValue;
	}
	void setApplication( const STRING &newValue )
	{
		m_application = newValue;
	}
	void setTopic( const STRING &newValue )
	{
		m_topic = newValue;
	}
	void setValue()
	{
		if( !m_menuText.isEmpty() )
			ListObject::setValue( m_menuText );
		else
			ListObject::setValue( m_identifier );
	}
};

class CommandDialog : public ModalDialog
{
	FileCommand	*m_command;		// open, print ...

	virtual ProcessStatus handleCreate();
	virtual ProcessStatus handleOk();
	virtual ProcessStatus handleCommand( int cmd );

	public:
	CommandDialog( FileCommand *newCommand )
	{
		m_command = newCommand;
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

