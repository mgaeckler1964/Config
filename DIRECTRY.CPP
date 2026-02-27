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

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus DirectoryDialog::handleCreate( void )
{
	strcpy( tmpDir, directory );
	DlgDirList(	handle(), tmpDir,
				IDC_DIRECTORY_LIST, IDC_CUR_DIRECTORY,
				DDL_DRIVES|DDL_DIRECTORY|DDL_HIDDEN|DDL_SYSTEM );

	return psDO_DEFAULT;
}

ProcessStatus DirectoryDialog::handleCommand( int cmd )
{
	char	dirBuffer[1024];

	if( cmd == IDC_DIRECTORY_LIST )
	{
		*dirBuffer = 0;
		DlgDirSelectEx( handle(), dirBuffer, sizeof( dirBuffer ), IDC_DIRECTORY_LIST );

		if( dirBuffer[1] == ':' )	// drive
		{
			strcpy( directory, dirBuffer );
			strcat( directory, "\\" );
		}
		else if( dirBuffer[0] == '.' )	// back
		{
			char	*cp = strrchr( directory, '\\' );
			if( cp )
			{
				if( cp <= directory + 3 )
					cp++;
				*cp = 0;
			}
		}
		else if( *dirBuffer )
		{
			char	*cp = strrchr( dirBuffer, '\\' );
			if( cp )
				*cp = 0;

			if( strlen( directory ) > 3 )
				strcat( directory, "\\" );
			strcat( directory, dirBuffer );
		}
		if( *dirBuffer )
		{
			strcpy( tmpDir, directory );
			DlgDirList(	handle(), tmpDir,
						IDC_DIRECTORY_LIST, IDC_CUR_DIRECTORY,
						DDL_DRIVES|DDL_DIRECTORY|DDL_HIDDEN|DDL_SYSTEM|DDL_EXCLUSIVE );
		}
		return psPROCESSED;
	}
	else
		return ModalDialog::handleCommand( cmd );
}

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

