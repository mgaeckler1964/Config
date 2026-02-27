/*
		Project:		CONFIG
		Module:			FILETYPE.CPP
		Description:	File Type Dialog
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

#include "filetype.h"
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

class FILE_EXTENSION : public ListObject
{
	STRING	extension;
	STRING	newFileName;

	void loadExtension( void );

	public:
	FILE_EXTENSION()
	{
		extension = "";
	}
	FILE_EXTENSION( const STRING &extension ) : ListObject( extension )
	{
		this->extension = extension;
		loadExtension();
	}
	void setExtension( const STRING &extension )
	{
		this->extension = extension;
		setValue( extension );
	}
};

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void FILE_EXTENSION::loadExtension( void )
{
	newFileName = "";
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

/*
---------------------------------------------------------------------------
		FileTypeDialog
---------------------------------------------------------------------------
*/
ProcessStatus FileTypeDialog::handleCreate( void )
{
	HKEY	handle;
	char	commandBuffer[1024];
	char	extensionBuffer[32];
	char	typeBuffer[1024];
	STRING	commandType;
	STRING	extension;
	DWORD	i;
	long	size;
	STRING	key;

	commandList.initHandle( this, IDL_COMMANDS );
	commandData.setControl( &commandList );

	extensionList.initHandle( this, IDL_EXTENSIONS );
	extensionData.setControl( &extensionList );

	key = identifier;
	key += "\\shell";

	setEditText( IDE_FILE_IDENTIFIER, identifier );
	if( identifier[0U] )
		disableText( IDE_FILE_IDENTIFIER );

	if( RegOpenKey( HKEY_CLASSES_ROOT, key, &handle ) == ERROR_SUCCESS )
	{
		i = 0;
		while( RegEnumKey( handle, i++, commandBuffer, sizeof( commandBuffer ) ) == ERROR_SUCCESS )
		{
			commandType = commandBuffer;
			commandData.addEntry( new FILE_COMMAND( identifier, commandType ) );
		}
		RegCloseKey( handle );
	}

	i = 0;
	while( RegEnumKey( HKEY_CLASSES_ROOT, i++, extensionBuffer, sizeof( extensionBuffer ) ) == ERROR_SUCCESS )
	{
		if( *extensionBuffer == '.' )
		{
			*typeBuffer = 0;
			size = sizeof( typeBuffer );
			RegQueryValue( HKEY_CLASSES_ROOT, extensionBuffer, typeBuffer, &size );
			if( size > 0
			&&	*typeBuffer
			&&	!strcmp( typeBuffer, identifier ) )
			{
				FILE_EXTENSION	*tmp;

				extension = extensionBuffer;
				tmp = new FILE_EXTENSION( extension );
				extensionData.addEntry( tmp );
			}
		}
	}

	return psDO_DEFAULT;
}

ProcessStatus FileTypeDialog::handleCommand( int cmd )
{
	if( cmd == COMMANDLINE_DIALOG )
	{
		FILE_COMMAND	*command = (FILE_COMMAND*)commandData.getSelectedEntry();
		if( command )
		{
			CommandDialog	dialog( command );

			if( dialog.create( this ) )
			{
				command->setValue();
				command->saveCommand();
			}
		}
	}
	else if( cmd == IDPB_CREATE_COMMAND )
	{
		FILE_COMMAND *command = new FILE_COMMAND( identifier );
		if( command )
		{
			CommandDialog	dialog( command );
			if( dialog.create( this ) == IDOK )
			{
				command->setValue();
				commandData.addEntry( command );
				command->saveCommand();
			}
			else
				delete command;
		}
	}
	else
		return ModalDialog::handleCommand( cmd );

	return psPROCESSED;
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

