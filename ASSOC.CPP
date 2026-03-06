/*
		Project:		CONFIG
		Module:			ASSOC.CPP
		Description:	AssociationsConfig Dialog
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

#include "assoc.h"

#include <memory>

#include <winlib/registry.h>

#include "filetype.h"

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

class FileType : public ListObject
{
	STRING	m_identifier;	// key name under .../Classes/Root
	STRING	m_commandLine;	// the value of the key <ident>/shell/open/coomand
	STRING	m_printCommand;	// the value of the key <ident>/shell/print/coomand
	bool	m_publicKey;

	void loadFileType();

	public:
	FileType( const STRING &value, bool publicKey ) : m_identifier( value ), m_publicKey(publicKey)
	{
		loadFileType();
	}

	bool isValidAssociation() const
	{
		return m_commandLine.isEmpty() ? false : true;
	}
	const STRING &getCommandLine() const
	{
		return m_commandLine;
	}
	const STRING &getPrintCommand() const
	{
		return m_printCommand;
	}
	const STRING &getIdentifier() const
	{
		return m_identifier;
	}
};

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void FileType::loadFileType()
{
	RegistryClassesRoot	rootKey(m_publicKey);
	STRING				value;

	rootKey.readValue( m_identifier, &value );
	if( value.isEmpty() )
		value = m_identifier;
	setValue( value );

	STRING key = m_identifier;
	key += "\\shell\\open\\command";
	if( rootKey.readValue( key, &value ) == rsOK && !value.isEmpty() )
		m_commandLine = value;

	key = m_identifier;
	key += "\\shell\\print\\command";
	if( rootKey.readValue( key, &value ) == rsOK && !value.isEmpty() )
		m_printCommand = value;
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus AssociationsConfig::handleCreate()
{
	RegistryClassesRoot	rootKey(m_publicKey);

	fileTypeBox.initHandle( this, IDL_FILETYPES );
	m_data.setControl( &fileTypeBox );

	for( 
		Registry::key_iterator it = rootKey.kbegin(), endIT = rootKey.kend();
		it != endIT;
		++it
	)
	{
		const STRING &key = *it;
		if( !key.isEmpty() && *key.c_str() != '.' )
		{
			std::auto_ptr<FileType>	theFileType( new FileType( key, m_publicKey ) );
			if( theFileType->isValidAssociation() )
				m_data.addEntry( theFileType.release() );
		}
	}

	return psDO_DEFAULT;
}

ProcessStatus AssociationsConfig::handleCommand( int cmd )
{
	if( cmd == IDL_FILETYPES )
	{
		FileType	*tmp = static_cast<FileType*>(m_data.getSelectedEntry());
		if( tmp )
		{
			const STRING	&commandLine = tmp->getCommandLine();
			STRING			commandLabel = STRING("Open with: ") + commandLine;
			setStaticText( IDS_OPEN_WITH, commandLabel );

			const STRING	&printCommand = tmp->getPrintCommand();
			commandLabel = STRING("Print with: ") + printCommand;

			setStaticText( IDS_PRINT_WITH, commandLabel );
		}
		else
		{
			setStaticText( IDS_OPEN_WITH, "No open" );
			setStaticText( IDS_PRINT_WITH, "No print" );
		}
	}
	else if( cmd == FILETYPE_DIALOG )
	{
		FileType	*tmp = static_cast<FileType*>(m_data.getSelectedEntry());
		if( tmp )
		{
			FileTypeDialog	dialog( tmp->getIdentifier(), m_publicKey );
			dialog.create( this );
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

