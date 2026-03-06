/*
		Project:		CONFIG
		Module:			COMMAND.CPP
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


#include <winlib\stddlg.h>
#include <winlib\ListCont.h>

#include "command.h"

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
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void FileCommand::loadCommand()
{
	STRING				tmpStr;

	STRING				value;
	STRING				key;
	STRING				tmpKey;
	RegistryClassesRoot	rootKey(m_publicKey);

	key = m_fileType;
	key += "\\shell\\";
	key += m_identifier;

	rootKey.readValue( key, &tmpStr );
	if( !tmpStr.isEmpty() )
	{
		value = tmpStr;
		m_menuText = value;
	}
	else
	{
		value = m_identifier;
		m_menuText.release();
	}

	ListObject::setValue( value );

	tmpKey = key;
	tmpKey += "\\command";
	rootKey.readValue( tmpKey, &tmpStr );
	if( !tmpStr.isEmpty() )
		m_commandLine = tmpStr;

	tmpKey = key;
	tmpKey += "\\ddeexec";
	rootKey.readValue( tmpKey, &tmpStr );
	if( !tmpStr.isEmpty() )
		m_ddeCommand = tmpStr;

	key = tmpKey;
	tmpKey += "\\application";
	rootKey.readValue( tmpKey, &tmpStr );
	if( !tmpStr.isEmpty() )
		m_application = tmpStr;

	tmpKey = key;
	tmpKey += "\\ifexec";
	rootKey.readValue( tmpKey, &tmpStr );
	if( !tmpStr.isEmpty() )
		m_ddeIfRunning = tmpStr;

	tmpKey = key;
	tmpKey += "\\topic";
	rootKey.readValue( tmpKey, &tmpStr );
	if( !tmpStr.isEmpty() )
		m_topic = tmpStr;
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus CommandDialog::handleCreate()
{
	setEditText( IDE_COMMANDLINE, m_command->getCommandLine() );
	setEditText( IDE_DDE_COMMAND, m_command->getDDEcommand() );
	setEditText( IDE_DDE_IFEXEC, m_command->getDDErunCommand() );
	setEditText( IDE_APPLICATION, m_command->getApplication() );
	setEditText( IDE_TOPIC, m_command->getTopic() );
	setEditText( IDC_CMD_MENUTEXT, m_command->getMenuText() );

	STRING identifier = m_command->getIdentifier();
	setEditText( IDE_CMD_IDENTIFIER, identifier );
	if( !identifier.isEmpty() )
		disableText( IDE_CMD_IDENTIFIER );

	return psDO_DEFAULT;
}

ProcessStatus CommandDialog::handleOk()
{
	m_command->setCommandLine( getEditText( IDE_COMMANDLINE ) );
	m_command->setDDEcommand( getEditText( IDE_DDE_COMMAND) );
	m_command->setDDErunCommand( getEditText( IDE_DDE_IFEXEC) );
	m_command->setApplication( getEditText( IDE_APPLICATION) );
	m_command->setTopic( getEditText( IDE_TOPIC ) );
	m_command->setIdentifier( getEditText( IDE_CMD_IDENTIFIER ) );
	m_command->setMenuText( getEditText( IDC_CMD_MENUTEXT ) );

	return psDO_DEFAULT;
}

ProcessStatus CommandDialog::handleCommand( int cmd )
{
	if( cmd == IDPB_BROWSE_COMMAND )
	{
		OpenFileDialog	dialog;
		STRING			fileName;

		fileName = "";
		if( dialog.create( this, PROGRAM_TITLE, PROGRAM_FILTER ) )
		{
			fileName = dialog.getFilename();
			setEditText( IDE_COMMANDLINE, fileName );
		}

		return psPROCESSED;
	}
	else
		return ModalDialog::handleCommand( cmd );
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

void FileCommand::saveCommand()
{
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

