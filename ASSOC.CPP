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
#include "filetype.h"

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

class FILE_TYPE : public ListObject
{
	STRING	identifier;
	STRING	commandLine;
	STRING	printCommand;

	void loadFileType( void );

	public:
	FILE_TYPE( const STRING &value )
	{
		identifier = value;
		loadFileType();
	}

	bool isValidAssociation( void ) const
	{
		return commandLine[0U] ? TRUE : FALSE;
	}
	STRING getCommandLine( void ) const
	{
		return commandLine;
	}
	STRING getPrintCommand( void ) const
	{
		return printCommand;
	}
	STRING getIdentifier( void ) const
	{
		return identifier;
	}
};

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void FILE_TYPE::loadFileType( void )
{
	long	size;
	char	tmpBuff[1024];

	STRING	value;
	STRING	key;

	size = sizeof( tmpBuff );
	*tmpBuff = 0;
	RegQueryValue( HKEY_CLASSES_ROOT, identifier, tmpBuff, &size );
	if( size > 0 && *tmpBuff )
		value = tmpBuff;
	else
		value = identifier;

	setValue( value );

	key = identifier;
	key += "\\shell\\open\\command";
	size = sizeof( tmpBuff );
	*tmpBuff = 0;
	RegQueryValue( HKEY_CLASSES_ROOT, key, tmpBuff, &size );
	if( size > 0 && *tmpBuff )
		commandLine = tmpBuff;

	key = identifier;
	key += "\\shell\\print\\command";
	size = sizeof( tmpBuff );
	*tmpBuff = 0;
	RegQueryValue( HKEY_CLASSES_ROOT, key, tmpBuff, &size );
	if( size > 0 && *tmpBuff )
		printCommand = tmpBuff;
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus AssociationsConfig::handleCreate( void )
{
	char	typeBuffer[1024];
	STRING	fileType;
	DWORD	i;

	fileTypeBox.initHandle( this, IDL_FILETYPES );
	data.setControl( &fileTypeBox );

	i = 0;
	while( RegEnumKey( HKEY_CLASSES_ROOT, i++, typeBuffer, sizeof( typeBuffer ) ) == ERROR_SUCCESS )
	{
		if( *typeBuffer && *typeBuffer != '.' )
		{
			FILE_TYPE	*tmp;

			fileType = typeBuffer;
			tmp = new FILE_TYPE( fileType );
			if( tmp->isValidAssociation() )
				data.addEntry( tmp );
			else
				delete tmp;
		}
	}

	return psDO_DEFAULT;
}

ProcessStatus AssociationsConfig::handleCommand( int cmd )
{
	if( cmd == IDL_FILETYPES )
	{
		FILE_TYPE	*tmp;

		tmp = (FILE_TYPE*)data.getSelectedEntry();
		if( tmp )
		{
			STRING	commandLine = tmp->getCommandLine();
			STRING	commandLabel = STRING("Open with: ") + commandLine;

			setStaticText( IDS_OPEN_WITH, commandLabel );

			commandLine = tmp->getPrintCommand();
			commandLabel = STRING("Print with: ") + commandLine;

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
		FILE_TYPE	*tmp;
		tmp = (FILE_TYPE*)data.getSelectedEntry();
		if( tmp )
		{
			STRING	identifier = tmp->getIdentifier();
			FileTypeDialog	dialog( identifier );
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

