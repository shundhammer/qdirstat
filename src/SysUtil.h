/*
 *   File name: SysUtil.h
 *   Summary:	System utility functions for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef SysUtil_h
#define SysUtil_h

#include <sys/types.h> // uid_t

#include <QString>
#include <QRegExp>


// Override these before #include

#ifndef LOG_COMMANDS
#  define LOG_COMMANDS	true
#endif

#ifndef LOG_OUTPUT
#  define LOG_OUTPUT	false
#endif

#ifndef COMMAND_TIMEOUT_SEC
#  define COMMAND_TIMEOUT_SEC	15
#endif


namespace QDirStat
{
    /**
     * System utility functions
     **/
    namespace SysUtil
    {
	/**
	 * Try running a command and compare it against an expected result.
	 * Return 'true' if ok, 'false' if not.
	 *
	 * Log the command that is executed if 'logCommand' is 'true',
	 * log the command's output if 'logOutput' is 'true'.
	 **/
	bool tryRunCommand( const QString & commandLine,
			    const QRegExp & expectedResult,
			    bool	    logCommand	= LOG_COMMANDS,
			    bool	    logOutput	= LOG_OUTPUT   );

	/**
	 * Run a command line and return its output. If exitCode_ret is
	 * non-null, return the command's exit code in exitCode_ret.
	 *
	 * Log the command that is executed if 'logCommand' is 'true',
	 * log the command's output if 'logOutput' is 'true'.
	 *
	 * If the command exits with a non-zero exit code, both the command and
	 * the output are logged anyway unless 'ignoreErrCode' is 'true'.
	 *
	 * NOTICE 1: This uses a very basic command line parser; it simply
	 * splits the command up wherever whitespace might occur. If any of
	 * the arguments (no matter how sophisticated they might be quoted)
	 * possibly contains any whitespace, this is unsafe; in that case, use
	 * the overloaded version instead that accepts a QStringList as
	 * arguments.
	 *
	 * NOTICE 2: This does not start a shell with that command, it runs the
	 * command directly, so only binaries can be executed, no shell scripts
	 * or scripts of other interpreted languages. If that is desired, wrap
	 * the command into "/bin/sh -c".
	 **/
	QString runCommand( const QString & commandLine,
			    int *	    exitCode_ret  = 0,
			    int		    timeout_sec	  = COMMAND_TIMEOUT_SEC,
			    bool	    logCommand	  = LOG_COMMANDS,
			    bool	    logOutput	  = LOG_OUTPUT,
			    bool	    ignoreErrCode = false );

	/**
	 * Run a command with arguments 'args' and return its output. If
	 * exitCode_ret is non-null, return the command's exit code in
	 * exitCode_ret.
	 *
	 * Use this version to avoid any side effects due to command line
	 * parsing.
	 *
	 * Log the command that is executed if 'logCommand' is 'true',
	 * log the command's output if 'logOutput' is 'true'.
	 *
	 * If the command exits with a non-zero exit code, both the command and
	 * the output are logged anyway unless 'ignoreErrCode' is 'true'.
	 *
	 * NOTICE: This does not start a shell with that command, it runs the
	 * command directly, so only binaries can be executed, no shell scripts
	 * or scripts of other interpreted languages. If that is desired, use
	 * "/bin/sh" as the command, "-c" as the first argument and the command
	 * line to be executed as the second. Beware of shell quoting quirks!
	 **/
	QString runCommand( const QString &	command,
			    const QStringList & args,
			    int *		exitCode_ret  = 0,
			    int			timeout_sec   = COMMAND_TIMEOUT_SEC,
			    bool		logCommand    = LOG_COMMANDS,
			    bool		logOutput     = LOG_OUTPUT,
			    bool		ignoreErrCode = false );

	/**
	 * Return 'true' if the specified command is available and executable.
	 **/
	bool haveCommand( const QString & command );

	/**
	 * Open a URL in the desktop's default browser (using the
	 * /usr/bin/xdg-open command).
	 **/
	void openInBrowser( const QString & url );

	/**
	 * Check if this program runs with root privileges, i.e. with effective
	 * user ID 0.
	 **/
	bool runningAsRoot();

	/**
	 * Check if this program runs with 'sudo'.
	 **/
	bool runningWithSudo();

	/**
	 * Check if this program runs as the real root user, with root
	 * permissions, but not with 'sudo'.
	 **/
	bool runningAsTrueRoot();

	/**
	 * Return the home directory of the user with the specified user ID.
	 **/
	QString homeDir( uid_t uid );

        /**
         * Return the (first level) target of a symbolic link, i.e. the path
         * that the link points to. That target may again be a symlink;
         * this function does not follow multiple levels of symlinks.
         *
         * If 'path' is not a symlink, this returns an empty string.
         *
         * This function assumes UTF-8 encoding of names in the filesystem.
         **/
        QString symLinkTarget( const QString & path );

        /**
         * Return 'true' if a symbolic link is broken, i.e. the (first level)
         * target of the symlink does not exist in the filesystem.
         **/
        bool isBrokenSymLink( const QString & path );

        /**
         * Read the (first level) target of a symbolic link, assuming UTF-8
         * encoding of names in the filesystem.
         * This is a more user-friendly version of readlink(2).
         *
         * This returns an empty QByteArray if 'path' is not a symlink.
         **/
        QByteArray readLink( const QString & path );

        /**
         * Read the (first level) target of a symbolic link.
         * Unlike readLink( const QString & ) above, this does not make any
         * assumptions of name encoding in the filessytem; it just uses bytes.
         *
         * This is a more user-friendly version of readlink(2).
         *
         * This returns an empty QByteArray if 'path' is not a symlink.
         **/
        QByteArray readLink( const QByteArray & path );

    }	// namespace SysUtil
}	// namespace QDirStat


#endif // SysUtil_h
