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
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void FILE_COMMAND::loadCommand( void )
{
	long	size;
	char	tmpBuff[1024];

	STRING	value;
	STRING	key;
	STRING	tmpKey;

	size = sizeof( tmpBuff );
	key = fileType;
	key += "\\shell\\";
	key += identifier;

	*tmpBuff = 0;
	RegQueryValue( HKEY_CLASSES_ROOT, key, tmpBuff, &size );
	if( size > 0 && *tmpBuff )
	{
		value = tmpBuff;
		menuText = value;
	}
	else
	{
		value = identifier;
		menuText = "";
	}

	ListObject::setValue( value );

	tmpKey = key;
	tmpKey += "\\command";
	size = sizeof( tmpBuff );
	*tmpBuff = 0;
	RegQueryValue( HKEY_CLASSES_ROOT, tmpKey, tmpBuff, &size );
	if( size > 0 && *tmpBuff )
		commandLine = tmpBuff;

	tmpKey = key;
	tmpKey += "\\ddeexec";
	size = sizeof( tmpBuff );
	*tmpBuff = 0;
	RegQueryValue( HKEY_CLASSES_ROOT, tmpKey, tmpBuff, &size );
	if( size > 0 && *tmpBuff )
		ddeCommand = tmpBuff;

	key = tmpKey;
	tmpKey += "\\application";
	*tmpBuff = 0;
	RegQueryValue( HKEY_CLASSES_ROOT, tmpKey, tmpBuff, &size );
	if( size > 0 && *tmpBuff )
		application = tmpBuff;

	tmpKey = key;
	tmpKey += "\\ifexec";
	*tmpBuff = 0;
	RegQueryValue( HKEY_CLASSES_ROOT, tmpKey, tmpBuff, &size );
	if( size > 0 && *tmpBuff )
		ddeIfRunning = tmpBuff;

	tmpKey = key;
	tmpKey += "\\topic";
	*tmpBuff = 0;
	RegQueryValue( HKEY_CLASSES_ROOT, tmpKey, tmpBuff, &size );
	if( size > 0 && *tmpBuff )
		topic = tmpBuff;
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus CommandDialog::handleCreate( void )
{
	setEditText( IDE_COMMANDLINE, command->getCommandLine() );
	setEditText( IDE_DDE_COMMAND, command->getDDEcommand() );
	setEditText( IDE_DDE_IFEXEC, command->getDDErunCommand() );
	setEditText( IDE_APPLICATION, command->getApplication() );
	setEditText( IDE_TOPIC, command->getTopic() );
	setEditText( IDC_CMD_MENUTEXT, command->getMenuText() );

	STRING identifier = command->getIdentifier();
	setEditText( IDE_CMD_IDENTIFIER, identifier );
	if( identifier[0U] )
		disableText( IDE_CMD_IDENTIFIER );

	return psDO_DEFAULT;
}

ProcessStatus CommandDialog::handleOk( void )
{
	command->setCommandLine( getEditText( IDE_COMMANDLINE ) );
	command->setDDEcommand( getEditText( IDE_DDE_COMMAND) );
	command->setDDErunCommand( getEditText( IDE_DDE_IFEXEC) );
	command->setApplication( getEditText( IDE_APPLICATION) );
	command->setTopic( getEditText( IDE_TOPIC ) );
	command->setIdentifier( getEditText( IDE_CMD_IDENTIFIER ) );
	command->setMenuText( getEditText( IDC_CMD_MENUTEXT ) );

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

void FILE_COMMAND::saveCommand( void )
{
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

