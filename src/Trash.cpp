/*
 *   File name: Trash.h
 *   Summary:	Implementation of the XDG Trash spec for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <errno.h>
#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "Trash.h"
#include "Logger.h"
#include "Exception.h"


Trash * Trash::_instance = 0;


Trash * Trash::instance()
{
    if ( ! _instance )
    {
	_instance = new Trash();
	CHECK_NEW( _instance );
    }

    return _instance;
}


Trash::Trash():
    _homeTrashDir(0)
{
    _homeDevice = device( QDir::homePath() );

    QByteArray xdg_data_home = qgetenv( "XDG_DATA_HOME" );
    QString homeTrash = xdg_data_home.isEmpty() ?
	QDir::homePath() + "/.local/share" :
	QString::fromUtf8( xdg_data_home );
    homeTrash += "/Trash";

    _homeTrashDir = new TrashDir( homeTrash, _homeDevice );
    CHECK_NEW( _homeTrashDir );

    _trashDirs[ _homeDevice ] = _homeTrashDir;
}


Trash::~Trash()
{
    // NOP
}


dev_t Trash::device( const QString & path )
{
    dev_t dev = 0;
    struct stat statBuf;
    int result = stat( path.toUtf8(), &statBuf );
    dev = statBuf.st_dev;

    if ( result < 0 )
    {
	logError() << "stat( " << path << " ) failed: "
		   << formatErrno() << endl;

	dev = static_cast<dev_t>( -1 );
    }

    return dev;
}


QString Trash::toplevel( const QString & rawPath )
{
    dev_t dev = device( rawPath );
    QFileInfo fileInfo( rawPath );
    QString path = fileInfo.canonicalPath();
    QStringList components = path.split( "/", QString::SkipEmptyParts );
    QString lastPath;

    // Go one directory level up as long as we are on the same device

    while ( ! components.isEmpty() && device( path ) == dev )
    {
	lastPath = path;
	components.removeLast();
	path = "/" + components.join( "/" );
    }

    if ( components.isEmpty() && device( "/" ) == dev )
	lastPath = "/";

    return lastPath;
}


TrashDir * Trash::trashDir( const QString & path )
{
    dev_t dev = device( path );

    if ( _trashDirs.contains( dev ) )
	return _trashDirs[ dev ];

    QString topDir = toplevel( path );

    try
    {
	// Check if there is $TOPDIR/.Trash

	QString trashPath = topDir + "/.Trash";

	struct stat statBuf;
	int result = stat( trashPath.toUtf8(), &statBuf );

	if ( result < 0 && errno == ENOENT ) // No such file or directory
	{
	    // No $TOPDIR/.Trash: Use $TOPDIR/.Trash-$UID

	    logInfo() << "No " << trashPath << endl;
	    trashPath = topDir + QString( "/.Trash-%1" ).arg( getuid() );
	    logInfo() << "Using " << trashPath << endl;
	}
	else if ( result < 0 )
	{
	    // stat() failed for some other reason (not "no such file or directory")

	    THROW( FileException( trashPath, "stat() failed for " + trashPath
				  + ": " + formatErrno() ) );
	}
	else // stat() was successful
	{
	    mode_t mode = statBuf.st_mode;

	    if ( S_ISDIR( mode ) &&
		 ( mode & S_ISVTX	) ) // Check sticky bit
	    {
		// Use $TOPDIR/.Trash/$UID

		trashPath += QString( "/%1" ).arg( getuid() );
		logInfo() << "Using " << trashPath << endl;
	    }
	    else // Not a directory or sticky bit not set
	    {
		if ( ! S_ISDIR( mode ) )
		    THROW( FileException( trashPath, trashPath + " is not a directory" ) );
		else
		    THROW( FileException( trashPath, "Sticky bit required on " + trashPath ) );
	    }
	}

	TrashDir * trashDir = new TrashDir( trashPath, dev );
	CHECK_NEW( trashDir );
	_trashDirs[ dev ] = trashDir;

	return trashDir;
    }
    catch ( const FileException &ex )
    {
	CAUGHT( ex );
	logWarning() << "Falling back to home trash dir: "
		     << _homeTrashDir->path() << endl;

	return _homeTrashDir;
    }
}


bool Trash::trash( const QString & path )
{
    try
    {
	TrashDir * trashDir = instance()->trashDir( path );

	if ( ! trashDir )
	    return false;

	QString targetName = trashDir->uniqueName( path );
	trashDir->createTrashInfo( path, targetName );
	trashDir->move( path, targetName );
    }
    catch ( const FileException & ex )
    {
	CAUGHT( ex );
	logError() << "Move to trash failed for " << path << endl;

	return false;
    }

    logInfo() << "Successfully moved to trash: " << path << endl;

    return true;
}


bool Trash::restore( const QString & path )
{
    Q_UNUSED( path )

    // TO DO
    // TO DO
    // TO DO

    return true;
}


void Trash::empty()
{
    // TO DO
    // TO DO
    // TO DO
}





TrashDir::TrashDir( const QString & path, dev_t device ):
    _path( path ),
    _device( device )
{
    // logDebug() << "Created TrashDir " << path << endl;

    ensureDirExists( path,	  0700, true );
    ensureDirExists( filesPath(), 0700, true );
    ensureDirExists( infoPath(),  0700, true );
}


QString TrashDir::uniqueName( const QString & path )
{
    QFileInfo file( path );
    QDir filesDir( filesPath() );

    QString baseName  = file.baseName();
    QString extension = file.completeSuffix();
    int	    count     = 0;
    QString name      = baseName;

    if ( ! extension.isEmpty() )
	name += "." + extension;

    while ( filesDir.exists( name ) )
    {
	name = QString( "%1_%2" ).arg( baseName ).arg( ++count );

	if ( ! extension.isEmpty() )
	    name += "." + extension;
    }

    // We don't care if a .trashinfo file with that name already exists in the
    // Trash/info directory: Without a corresponding file or directory in the
    // Trash/files directory, that .trashinfo file is worthless anyway and can
    // safely be overwritten.

    return name;
}


bool TrashDir::ensureDirExists( const QString & path,
				mode_t		mode,
				bool		doThrow )
{
    QDir dir( path );

    if ( dir.exists() )
	return true;

    logInfo() << "mkdir " << path << endl;
    int result = mkdir( path.toUtf8(), mode );

    if ( result < 0 && doThrow )
    {
	THROW( FileException( path,
			      QString( "Could not create directory %1: %2" )
			      .arg( path ).arg( formatErrno() ) ) );
    }

    return result >= 0;
}


void TrashDir::createTrashInfo( const QString & path,
				const QString & targetName )
{
    QFile trashInfo( infoPath() + "/" + targetName + ".trashinfo" );

    if ( ! trashInfo.open( QIODevice::WriteOnly | QIODevice::Text ) )
	THROW( FileException( trashInfo.fileName(), "Can't open " + trashInfo.fileName() ) );

    QTextStream str( &trashInfo );
    str << "[Trash Info]" << endl;
    str << "Path=" << path << endl;
    str << "DeletionDate=" << QDateTime::currentDateTime().toString( Qt::ISODate ) << endl;
}


void TrashDir::move( const QString & path,
		     const QString & targetName )
{
    QFile file( path );
    QString targetPath = filesPath() + "/" + targetName;

    bool success = file.rename( targetPath );

    if ( ! success )
	THROW( FileException( path, "Could not move " + path + " to " + targetPath ) );
}
