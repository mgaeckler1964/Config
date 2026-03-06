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

#include <memory>

#include <winlib/registry.h>

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
using namespace gak;

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

class FileExtension : public ListObject
{
	STRING	m_extension;
	STRING	m_newFileName;

	void loadExtension();

	public:
	FileExtension()	{}
	FileExtension( const STRING &extension ) : ListObject( extension ), m_extension(extension)
	{
		loadExtension();
	}
	void setExtension( const STRING &extension )
	{
		m_extension = extension;
		setValue( extension );
	}
};

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void FileExtension::loadExtension()
{
	m_newFileName.release();
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

/*
---------------------------------------------------------------------------
		FileTypeDialog
---------------------------------------------------------------------------
*/
ProcessStatus FileTypeDialog::handleCreate()
{
	RegistryClassesRoot	rootKey(m_publicKey);
	Registry			handle;
	
	STRING	commandType;
	STRING	extension, type;

	commandList.initHandle( this, IDL_COMMANDS );
	commandData.setControl( &commandList );

	extensionList.initHandle( this, IDL_EXTENSIONS );
	extensionData.setControl( &extensionList );

	setEditText( IDE_FILE_IDENTIFIER, m_identifier );
	if( !m_identifier.isEmpty() )
		disableText( IDE_FILE_IDENTIFIER );

	/*
		find the commands (open, print, ...) that belong to the current file file
	*/
	STRING key = m_identifier + "\\shell";

	if( handle.openSubkey( rootKey, key ) == ERROR_SUCCESS )
	{
		for(
			Registry::key_iterator it = handle.kbegin(), endIT = handle.kend();
			it != endIT;
			++it
		)
		{
			commandType = *it;
			commandData.addEntry( new FileCommand( m_identifier, commandType, m_publicKey ) );
		}
	}

	/*
		find the extensions that are associated with current file file
	*/
	for(
		Registry::key_iterator it = rootKey.kbegin(), endIT = rootKey.kend();
		it != endIT;
		++it
	)
	{
		extension = *it;
		if( extension[0U] == '.' )		// the keys under classes root that begin with a dot are file extensions
										// their values contains the name of the file type
		{
			rootKey.readValue( extension, &type );
			if( type == m_identifier )
			{
				extensionData.addEntry( new FileExtension( extension ) );
			}
		}
	}

	return psDO_DEFAULT;
}

ProcessStatus FileTypeDialog::handleCommand( int cmd )
{
	if( cmd == COMMANDLINE_DIALOG )
	{
		FileCommand	*command = (FileCommand*)commandData.getSelectedEntry();
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
		std::auto_ptr<FileCommand>  command( new FileCommand( m_identifier, m_publicKey ) );
		if( command.get() )
		{
			CommandDialog	dialog( command .get());
			if( dialog.create( this ) == IDOK )
			{
				command->setValue();
				commandData.addEntry( command.get() );
				command.release()->saveCommand();
			}
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

