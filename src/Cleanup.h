/*
 *   File name: Cleanup.h
 *   Summary:	QDirStat classes to reclaim disk space
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef Cleanup_h
#define Cleanup_h


#include <QAction>
#include <QList>
#include <QTextStream>

class OutputWindow;


namespace QDirStat
{
    class FileInfo;


    /**
     * Cleanup action to be performed for DirTree items.
     **/
    class Cleanup: public QAction
    {
	Q_OBJECT

    public:

	enum RefreshPolicy
	{
	    NoRefresh,
	    RefreshThis,
	    RefreshParent,
	    AssumeDeleted
	};

	enum OutputWindowPolicy
	{
	    ShowAlways,
	    ShowIfErrorOutput,
	    ShowAfterTimeout,  // This includes ShowIfErrorOutput
	    ShowNever
	};

	/**
	 * Constructor.
	 *
	 * 'title' is the human readable menu title.
	 * 'command' is the shell command to execute.
	 **/
	Cleanup( QString   command = "",
		 QString   title   = "",
		 QObject * parent  = 0 );

	/**
	 * Return the command line that will be executed upon calling
	 * Cleanup::execute(). This command line may contain %p for the
	 * complete path of the directory or file concerned or %n for the pure
	 * file or directory name without path.
	 **/
	const QString & command() const { return _command; }

	/**
	 * Return the user title of this command as displayed in menus.
	 * This may include '&' characters for keyboard shortcuts.
	 * See also cleanTitle() .
	 **/
	const QString & title() const { return _title; }

	/**
	 * Return the cleanup action's title without '&' keyboard shortcuts.
	 * Uses the ID as fallback if the name is empty.
	 **/
	QString cleanTitle() const;

	/**
	 * Return the icon name of this cleanup action.
	 **/
	QString iconName() const { return _iconName; }

	/**
	 * Return whether or not this cleanup action is generally active.
	 **/
	bool active() const { return _active; }

	/**
	 * Return whether or not this cleanup action works for this particular
	 * FileInfo. Checks all the other conditions (active(),
	 * worksForDir(), worksForFile(), ...) accordingly.
	 **/
	bool worksFor( FileInfo * item ) const;

	/**
	 * Return whether or not this cleanup action works for directories,
	 * i.e. whether or not Cleanup::execute() will be successful if the
	 * object passed is a directory.
	 **/
	bool worksForDir() const { return _worksForDir; }

	/**
	 * Return whether or not this cleanup action works for plain files.
	 **/
	bool worksForFile() const { return _worksForFile; }

	/**
	 * Return whether or not this cleanup action works for QDirStat's
	 * special '<Files>' items, i.e. the pseudo nodes created in most
	 * directories that hold the plain files.
	 **/
	bool worksForDotEntry() const { return _worksForDotEntry; }

	/**
	 * Return whether or not the cleanup action should be performed
	 * recursively in subdirectories of the initial FileInfo.
	 **/
	bool recurse()			const { return _recurse; }

	/**
	 * Return whether or not this cleanup should ask the user for
	 * confirmation when it is executed.
	 *
	 * The default is 'false'. Use with caution - not only can this become
	 * very annoying, people also tend to automatically click on 'OK' when
	 * too many confirmation dialogs pop up!
	 **/
	bool askForConfirmation() const { return _askForConfirmation; }

	/**
	 * Return the shell to use to invoke the command of this cleanup.
	 * If this is is empty, use defaultShells().first().
	 *
	 * Regardless of which shell is used, the command is always started
	 * with the shell and the "-c" option.
	 **/
	QString shell() const { return _shell; }

	/**
	 * Return the full path name to the user's login shell.
	 * The $SHELL environment variable is used to obtain this value.
	 * If this is empty, this defaults to defaultShells().first().
	 **/
	static QString loginShell();

	/**
	 * Return the full paths to the available (and executable) shells:
	 *     loginShell()	($SHELL)
	 *     /bin/bash
	 *     /bin/sh
	 **/
	static const QStringList & defaultShells();

	/**
	 * Return the first default shell or an empty string if there is no
	 * usable shell at all.
	 **/
	static QString defaultShell();

	/**
	 * Return 'true' if programName is non-empty and executable.
	 **/
	static bool isExecutable( const QString & programName );

	/**
	 * Return the refresh policy of this cleanup action - i.e. the action
	 * to perform after each call to Cleanup::execute(). This is supposed
	 * to bring the corresponding DirTree back into sync after the cleanup
	 * action - the underlying file tree might have changed due to that
	 * cleanup action.
	 *
	 * NoRefresh: Don't refresh anything. Assume nothing has changed.
	 * This is the default.
	 *
	 * RefreshThis: Refresh the DirTree from the item on that was passed
	 * to Cleanup::execute().
	 *
	 * RefreshParent: Refresh the DirTree from the parent of the item on
	 * that was passed to Cleanup::execute(). If there is no such parent,
	 * refresh the entire tree.
	 *
	 * AssumeDeleted: Do not actually refresh the DirTree.	Instead,
	 * blindly assume the cleanup action has deleted the item that was
	 * passed to Cleanup::execute() and delete the corresponding subtree
	 * in the DirTree accordingly. This will work well for most deleting
	 * actions as long as they can be performed without problems. If there
	 * are any problems, however, the DirTree might easily run out of sync
	 * with the directory tree: The DirTree will show the subtree as
	 * deleted (i.e. it will not show it any more), but it still exists on
	 * disk. This is the tradeoff to a very quick response. On the other
	 * hand, the user can easily at any time hit one of the explicit
	 * refresh buttons and everything will be back into sync again.
	 **/
	enum RefreshPolicy refreshPolicy() const { return _refreshPolicy; }

	/**
	 * Return the policy when an output window (see also OutputWindow) for
	 * this clean action is shown. Since cleanup actions start shell
	 * commands, the output of those shell commands might be important,
	 * especially if they report an error. In addition to that, if a
	 * cleanup action takes a while, it might be a good idea to show the
	 * user what is going on. Notice that there will always be only one
	 * output window for all cleanup tasks that are to be started in one
	 * user action; if multiple items are selected, the corresponding
	 * command will be started for each of the selected items individually
	 * one after another, but the output window will remain open and
	 * collect the output of each one. Likewise, if a command is recursive,
	 * it is started for each directory level, and the output is also
	 * collected in the same output window.
	 *
	 * Possible values:
	 *
	 * ShowAlways: Always open an output window. This makes sense for
	 * cleanup actions that take a while, like compressing files, recoding
	 * videos, recompressing JPG images.
	 *
	 * ShowIfErrorOutput: Leave the output window hidden, but open it if
	 * there is any error output (i.e. output on its stderr channel). This
	 * is useful for most cleanup tasks that are typically quick, but that
	 * might also go wrong - for example, due to insufficient permissions
	 * in certain directories.
	 *
	 * ShowAfterTimeout: (This includes ShowIfErrorOutput) Leave the output
	 * window hidden for a certain timeout (3 seconds by default), but open
	 * it if it takes any longer than that. If there is error output, it is
	 * opened immediately. This is the default and recommended setting. The
	 * output can be configured with setOutputWindowTimeout().
	 *
	 * ShowNever: Never show the output window, no matter how long the
	 * cleanup task takes or if there is any amount of error output, or
	 * even if the cleanup process crashes or could not be started.
	 **/
	enum OutputWindowPolicy outputWindowPolicy() const
	    { return _outputWindowPolicy; }

	/**
	 * Return the timeout (in milliseconds) for the ShowAfterTimeout output
	 * window policy. The default is 0 which means to use the
	 * OutputWindow dialog class default.
	 **/
	int outputWindowTimeout() const { return _outputWindowTimeout; }

	/**
	 * Return 'true' if the output window is closed automatically when the
	 * cleanup task is done and there was no error.
	 **/
	bool outputWindowAutoClose() const { return _outputWindowAutoClose; }

	/**
	 * Return a mapping from RefreshPolicy to string.
	 **/
	static QMap<int, QString> refreshPolicyMapping();

	/**
	 * Return a mapping from OutputWindowPolicy to string.
	 **/
	static QMap<int, QString> outputWindowPolicyMapping();

	/**
	 * Return a mapping from macros to applications that may be specific
	 * for different desktops (KDE, GNOME, Xfce, Unity, LXDE).
	 * Incomplete list:
	 *
	 *   %terminal
	 *	KDE:	"konsole --workdir %d"
	 *	GNOME:	"gnome-terminal"
	 *	Unity:	"gnome-terminal"
	 *	Xfce:	"xfce4-terminal"
	 *	LXDE:	"lxterminal"
	 *
	 *   %filemanager
	 *	KDE:	"konqueror --profile filemanagement" // not that dumbed-down Dolphin
	 *	GNOME:	"nautilus"
	 *	Unity:	"nautilus"
	 *	Xfcd:	"thunar"
	 *	LXDE:	"pcmanfm"
	 *
	 * What desktop is currently used is guessed from $XDG_CURRENT_DESKTOP.
	 **/
	static const QMap<QString, QString> & desktopSpecificApps();

	/**
	 * Return a mapping from macros to fallback applications in case the
	 * current desktop cannot be determined:
	 *
	 *   %terminal	   "xterm"
	 *   %filemanager  "xdg-open"
	 **/
	static const QMap<QString, QString> & fallbackApps();


	//
	// Setters (see the corresponding getter for documentation)
	//

	void setTitle		     ( const QString & title  );
	void setCommand		     ( const QString & command)	   { _command		    = command;	 }
	void setIcon		     ( const QString & iconName );
	void setActive		     ( bool active   )		   { _active		    = active;	 }
	void setWorksForDir	     ( bool canDo    )		   { _worksForDir	    = canDo;	 }
	void setWorksForFile	     ( bool canDo    )		   { _worksForFile	    = canDo;	 }
	void setWorksForDotEntry     ( bool canDo    )		   { _worksForDotEntry	    = canDo;	 }
	void setRecurse		     ( bool recurse  )		   { _recurse		    = recurse;	 }
	void setAskForConfirmation   ( bool ask	     )		   { _askForConfirmation    = ask;	 }
	void setShell		     ( const QString &	  sh	 ) { _shell		    = sh;	 }
	void setRefreshPolicy	     ( RefreshPolicy	  policy ) { _refreshPolicy	    = policy;	 }
	void setOutputWindowPolicy   ( OutputWindowPolicy policy ) { _outputWindowPolicy    = policy;	 }
	void setOutputWindowTimeout  ( int timeoutMillisec )	   { _outputWindowTimeout   = timeoutMillisec; }
	void setOutputWindowAutoClose( bool autoClose )		   { _outputWindowAutoClose = autoClose; }


    public slots:

	/**
	 * The heart of the matter: Perform the cleanup with the FileInfo
	 * specified.
	 *
	 * 'outputWindow' is the optional dialog to watch the commands and
	 * their stdout and stderr output as they are executed.
	 **/
	void execute( FileInfo * item, OutputWindow * outputWindow );


    protected:

	/**
	 * Recursively perform the cleanup.
	 **/
	void executeRecursive( FileInfo *item, OutputWindow * outputWindow );

	/**
	 * Retrieve the directory part of a FileInfo's path.
	 **/
	const QString itemDir( const FileInfo * item ) const;

	/**
	 * Choose a suitable shell. Try this->shell() and fall back to
	 * defaultShell(). Return an empty string if no usable shell is found.
	 **/
	QString chooseShell( OutputWindow * outputWindow ) const;

	/**
	 * Expand some variables in string 'unexpanded' to information from
	 * within 'item'. Multiple expansion is performed as needed, i.e. the
	 * string may contain more than one variable to expand. The resulting
	 * string is returned.
	 *
	 *   %p expands to item->path() (in single quotes), i.e. the item's
	 *   full path name.
	 *
	 *     '/usr/local/bin'	      for that directory
	 *     '/usr/local/bin/doit'  for a file within it
	 *
	 *   %n expands to item->name() (in single quotes), i.e. the last
	 *   component of the pathname. The examples above would expand to:
	 *
	 *     'bin'
	 *     'doit'
	 *
	 *   %d expands to the directory name with full path. For directories,
	 *   this is the same as %p. For files or dot entries, this is the same
	 *   as their parent's %p:
	 *
	 *    '/usr/local/bin'	for a file /usr/local/bin/doit
	 *    '/usr/local/bin'	for directory /usr/local/bin.
	 *
	 *
	 *   %terminal	  "konsole" or "gnome-terminal" or "xfce4-terminal" ...
	 *
	 *   %filemanager "konqueror" or "nautilus" or "thunar" ...
	 *
	 *
	 * For commands that are to be executed from within the 'Clean up'
	 * menu, you might specify something like:
	 *
	 *     "xdg-open %p"
	 *     "tar cjvf %n.tar.bz2 && rm -rf %n"
	 **/
	QString expandVariables ( const FileInfo * item,
				  const QString	 & unexpanded ) const;

	/**
	 * Expand some variables in string 'unexpanded' to application that are
	 * typically different from one desktop (KDE, Gnome, Xfce) to the next:
	 *
	 *   %terminal	  "konsole" or "gnome-terminal" or "xfce4-terminal" ...
	 *   %filemanager "konqueror" or "nautilus" or "thunar" ...
	 **/
	QString expandDesktopSpecificApps( const QString & unexpanded ) const;

	/**
	 * Return a string with all occurrences of a single quote escaped with
	 * backslash.
	 **/
	QString escaped( const QString & unescaped ) const;

	/**
	 * Return a string in single quotes.
	 **/
	QString quoted( const QString & unquoted ) const;

	/**
	 * Run a command with 'item' as base to expand variables.
	 **/
	void runCommand( const FileInfo * item,
			 const QString	& command,
			 OutputWindow	* outputWindow) const;


	//
	// Data members
	//

	QString		   _command;
	QString		   _title;
	QString		   _iconName;
	bool		   _active;
	bool		   _worksForDir;
	bool		   _worksForFile;
	bool		   _worksForDotEntry;
	bool		   _recurse;
	bool		   _askForConfirmation;
	QString		   _shell;
	RefreshPolicy	   _refreshPolicy;
	OutputWindowPolicy _outputWindowPolicy;
	int		   _outputWindowTimeout;
	bool		   _outputWindowAutoClose;
    };


    typedef QList<Cleanup *>		CleanupList;
    typedef CleanupList::const_iterator CleanupListIterator;


    inline QTextStream & operator<< ( QTextStream & stream, const Cleanup * cleanup )
    {
	if ( cleanup )
	    stream << cleanup->cleanTitle();
	else
	    stream << "<NULL Cleanup *>";

	return stream;
    }
}	// namespace QDirStat


#endif // ifndef Cleanup_h

