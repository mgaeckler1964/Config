/*
		Project:		CONFIG
		Module:			DIRECTRY.CPP
		Description:	DirectoryDialog
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

#include "directry.h"
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

static size_t const DIR_BUFFER_SIZE = 10240;

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus DirectoryDialog::handleCreate()
{
	char	tmpDir[DIR_BUFFER_SIZE];
	strcpy( tmpDir, m_directory );
	DlgDirList(	handle(), tmpDir,
				IDC_DIRECTORY_LIST, IDC_CUR_DIRECTORY,
				DDL_DRIVES|DDL_DIRECTORY );

	return psDO_DEFAULT;
}

ProcessStatus DirectoryDialog::handleSelectionChange( int cmd )
{
	char	dirBuffer[DIR_BUFFER_SIZE];

	if( cmd == IDC_DIRECTORY_LIST )
	{
		*dirBuffer = 0;
		DlgDirSelectEx( handle(), dirBuffer, sizeof( dirBuffer ), IDC_DIRECTORY_LIST );

		if( dirBuffer[1] == ':' )	// drive
		{
			m_directory = dirBuffer;
			m_directory.condAppend(DIRECTORY_DELIMITER);
		}
		else if( !strcmp(dirBuffer, ".." DIRECTORY_DELIMITER_STRING ) )	// up one level
		{
			size_t	slashPos = m_directory.searchRChar( DIRECTORY_DELIMITER );
			if( slashPos != m_directory.no_index )
			{
				if( slashPos <= 3 )
					slashPos++;
				m_directory.cut(slashPos);
			}
		}
		else if( *dirBuffer )
		{
			char	*cp = strrchr( dirBuffer, '\\' );
			if( cp )
				*cp = 0;

			if( !m_directory.isEmpty() )
				m_directory.condAppend( DIRECTORY_DELIMITER );
			m_directory += dirBuffer;
		}
		if( *dirBuffer )
		{
			char	tmpDir[DIR_BUFFER_SIZE];
			strcpy( tmpDir, m_directory );
			DlgDirList(	handle(), tmpDir,
						IDC_DIRECTORY_LIST, IDC_CUR_DIRECTORY,
						DDL_DRIVES|DDL_DIRECTORY|DDL_EXCLUSIVE );
		}
		return psPROCESSED;
	}
	else
		return ModalDialog::handleSelectionChange( cmd );
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

