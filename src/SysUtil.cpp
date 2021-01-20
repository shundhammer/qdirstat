/*
 *   File name: SysUtil.cpp
 *   Summary:	System utility functions for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#define DONT_DEPRECATE_STRERROR
#include <unistd.h>	// access(), getuid(), geteduid(), readlink()
#include <errno.h>
#include <pwd.h>	// getpwuid()
#include <limits.h>     // PATH_MAX
#include <sys/stat.h>   // lstat()
#include <sys/types.h>

#include "SysUtil.h"
#include "Process.h"
#include "DirSaver.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


bool SysUtil::tryRunCommand( const QString & commandLine,
			     const QRegExp & expectedResult,
			     bool	     logCommand,
			     bool	     logOutput	)
{
    int exitCode = -1;
    QString output = runCommand( commandLine, &exitCode,
				 COMMAND_TIMEOUT_SEC, logCommand, logOutput,
				 true ); // ignoreErrCode

    if ( exitCode != 0 )
    {
	// logDebug() << "Exit code: " << exitCode << " command line: \"" << commandLine << "\"" << endl;
	return false;
    }

    bool expected = expectedResult.exactMatch( output );
    // logDebug() << "Expected: " << expected << endl;

    return expected;
}


QString SysUtil::runCommand( const QString & commandLine,
			     int *	     exitCode_ret,
			     int	     timeout_sec,
			     bool	     logCommand,
			     bool	     logOutput,
			     bool	     ignoreErrCode )
{
    if ( exitCode_ret )
	*exitCode_ret = -1;

    QStringList args = commandLine.split( QRegExp( "\\s+" ) );

    if ( args.size() < 1 )
    {
	logError() << "Bad command line: \"" << commandLine << "\"" << endl;
	return "ERROR: Bad command line";
    }

    QString command = args.takeFirst();

    return runCommand( command, args, exitCode_ret,
		       timeout_sec, logCommand, logOutput, ignoreErrCode );
}


QString SysUtil::runCommand( const QString &	 command,
			     const QStringList & args,
			     int *		 exitCode_ret,
			     int		 timeout_sec,
			     bool		 logCommand,
			     bool		 logOutput,
			     bool		 ignoreErrCode )
{
    if ( exitCode_ret )
	*exitCode_ret = -1;

    if ( ! haveCommand( command ) )
    {
	logInfo() << "Command not found: " << command << endl;
	return "ERROR: Command not found";
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert( "LANG", "C" ); // Prevent output in translated languages

    Process process;
    process.setProgram( command );
    process.setArguments( args );
    process.setProcessEnvironment( env );
    process.setProcessChannelMode( QProcess::MergedChannels ); // combine stdout and stderr

    if ( logCommand )
	logDebug() << command << " " << args.join( " " ) << endl;

    process.start();
    bool success = process.waitForFinished( timeout_sec * 1000 );
    QString output = QString::fromUtf8( process.readAll() );

    if ( success )
    {
	if ( process.exitStatus() == QProcess::NormalExit )
	{
	    if ( exitCode_ret )
		*exitCode_ret = process.exitCode();

	    if ( ! ignoreErrCode && process.exitCode() )
	    {
		logError() << "Command exited with exit code "
			   << process.exitCode() << ": "
			   << command << "\" args: " << args
			   << endl;
	    }
	}
	else
	{
	    logError() << "Command crashed: \"" << command << "\" args: " << args << endl;
	    output = "ERROR: Command crashed\n\n" + output;
	}
    }
    else
    {
	logError() << "Timeout or crash: \"" << command << "\" args: " << args << endl;
	output = "ERROR: Timeout or crash\n\n" + output;
    }

    if ( logOutput || ( process.exitCode() != 0 && ! ignoreErrCode ) )
    {
        QString logOutput = output.trimmed();

        if ( logOutput.contains( '\n' ) )
            logDebug() << "Output: \n" << output << endl;
        else
            logDebug() << "Output: \"" << logOutput << "\"" << endl;
    }

    return output;
}


void SysUtil::openInBrowser( const QString & url )
{
    logDebug() << "Opening URL " << url << endl;

    Process::startDetached( "/usr/bin/xdg-open", QStringList() << url );
}


bool SysUtil::haveCommand( const QString & command )
{
    return access( command.toUtf8(), X_OK ) == 0;
}


bool SysUtil::runningAsRoot()
{
    return geteuid() == 0;
}


bool SysUtil::runningWithSudo()
{
    return ! qgetenv( "SUDO_USER" ).isEmpty();
}


bool SysUtil::runningAsTrueRoot()
{
    return runningAsRoot() && ! runningWithSudo();
}


QString SysUtil::homeDir( uid_t uid )
{
    struct passwd * pw = getpwuid( uid );

    return pw ? QString::fromUtf8( pw->pw_dir ) : QString();
}


QString SysUtil::symLinkTarget( const QString & path )
{
    return QString::fromUtf8( readLink( path ) );
}


bool SysUtil::isBrokenSymLink( const QString & path )
{
    QByteArray target = readLink( path );

    if ( target.size() == 0 )   // path is not a symlink
        return false;           // so it's also not a broken symlink


    // Start from the symlink's parent directory

    QStringList pathSegments = path.split( '/', QString::SkipEmptyParts );
    pathSegments.removeLast(); // We already know it's a symlink, not a directory
    QString parentPath = QString( path.startsWith( "/" ) ? "/" : "" ) + pathSegments.join( "/" );
    DirSaver dir( parentPath );

    // We can't use access() here since that would follow symlinks.
    // Let's use lstat() instead.

    struct stat statBuf;
    int statResult = lstat( target, &statBuf );

    if ( statResult == 0 )      // lstat() successful?
    {
        return false;           // -> the symlink is not broken.
    }
    else                        // lstat() failed
    {
        if ( errno == EACCES )  // permission denied for one of the dirs in target
        {
            logWarning() << "Permission denied for one of the directories"
                         << " in symlink target " << QString::fromUtf8( target )
                         << " of symlink " << path
                         << endl;

            return false;       // We don't know if the symlink is broken
        }
        else
        {
            logWarning() << "Broken symlink " << path
                         << " errno: " << strerror( errno )
                         << endl;
            return true;
        }
    }
}


QByteArray SysUtil::readLink( const QString & path )
{
    return readLink( path.toUtf8() );
}


QByteArray SysUtil::readLink( const QByteArray & path )
{
    QByteArray targetBuf( PATH_MAX, 0 );
    ssize_t len = ::readlink( path, targetBuf.data(), targetBuf.size() );

    if ( len == 0 )
    {
        logWarning() << QString::fromUtf8( path ) << " is not a symlink" << endl;
    }
    else if ( len == targetBuf.size() )
    {
        // Buffer overflow. Yes, this can actually happen:
        //   http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
        //
        // Since this is a very pathological case, we won't attempty any crazy
        // workarounds and simply fail with an error in the log.

        logError() << "Symlink target of " << QString::fromUtf8( path )
                   << " is longer than " << PATH_MAX << " bytes" << endl;
        targetBuf.clear();
    }
    else
    {
        targetBuf.resize( len );
    }

    return targetBuf;
}
