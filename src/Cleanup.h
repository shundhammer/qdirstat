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

	/**
	 * Constructor.
	 *
	 * 'id' is the unique name of this cleanup action as used in the
	 * settings file, 'title' is the human readable menu title. 'command'
	 * is the shell command to execute.
	 **/
	Cleanup( QString   id	   = "",
		 QString   command = "",
		 QString   title   = "",
		 QObject * parent  = 0 );

	/**
	 * Return the ID (name) of this cleanup action as used for setup files
	 * and the XML UI description. This ID should be unique within the
	 * application.
	 **/
	const QString & id() const { return _id; }

	/**
	 * Return the command line that will be executed upon calling @ref
	 * Cleanup::execute(). This command line may contain %p for the
	 * complete path of the directory or file concerned or %n for the pure
	 * file or directory name without path.
	 **/
	const QString & command() const { return _command; }

	/**
	 * Return the user title of this command as displayed in menus.
	 * This may include '&' characters for keyboard shortcuts.
	 * See also @ref cleanTitle() .
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
	 * i.e. whether or not @ref Cleanup::execute() will be successful if
	 * the object passed is a directory.
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
	enum RefreshPolicy refreshPolicy()	const { return _refreshPolicy; }


	void setTitle		   ( const QString & title  );
	void setId		   ( const QString & id	    )	   { _id		   = id;	   }
	void setCommand		   ( const QString & command)	   { _command		   = command;	   }
	void setIcon		   ( const QString & iconName );
	void setActive		   ( bool active   )		   { _active		  = active;	   }
	void setWorksForDir	   ( bool canDo	   )		   { _worksForDir	   = canDo;	   }
	void setWorksForFile	   ( bool canDo	   )		   { _worksForFile	   = canDo;	   }
	void setWorksForDotEntry   ( bool canDo	   )		   { _worksForDotEntry	   = canDo;	   }
	void setRecurse		   ( bool recurse  )		   { _recurse		   = recurse;	   }
	void setAskForConfirmation ( bool ask	   )		   { _askForConfirmation   = ask;	   }
	void setRefreshPolicy	   ( RefreshPolicy refreshPolicy ) { _refreshPolicy = refreshPolicy;  }

	/**
	 * Return a mapping from RefreshPolicy to string.
	 **/
	static QMap<int, QString> refreshPolicyMapping();

    public slots:

	/**
	 * The heart of the matter: Perform the cleanup with the FileInfo
	 * specified.
	 **/
	void execute( FileInfo * item );


    protected:

	/**
	 * Recursively perform the cleanup.
	 **/
	void executeRecursive( FileInfo *item );

	/**
	 * Ask user for confirmation to execute this cleanup action for
	 * 'item'. Returns 'true' if user accepts, 'false' otherwise.
	 **/
	bool confirmation( FileInfo * item );

	/**
	 * Retrieve the directory part of a FileInfo's path.
	 **/
	const QString itemDir( const FileInfo * item ) const;

	/**
	 * Expand some variables in string 'unexpanded' to information from
	 * within 'item'. Multiple expansion is performed as needed, i.e. the
	 * string may contain more than one variable to expand. The resulting
	 * string is returned.
	 *
	 * %p expands to item->path() (in single quotes), i.e. the item's full
	 * path name.
	 *
	 *     '/usr/local/bin'	      for that directory
	 *     '/usr/local/bin/doit'  for a file within it
	 *
	 * %n expands to item->name() (in single quotes), i.e. the last
	 * component of the pathname. The examples above would expand to:
	 *
	 *     'bin'
	 *     'doit'
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
	 * Escape all occurences of a single quote with backslash and surround
	 * the resulting string in single quotes.
	 **/
	QString escapeAndQuote( const QString & unescaped ) const;

	/**
	 * Run a command with 'item' as base to expand variables.
	 **/
	void runCommand( const FileInfo * item,
			 const QString	& command ) const;


	//
	// Data members
	//

	QString	      _id;
	QString	      _command;
	QString	      _title;
	QString	      _iconName;
	bool	      _active;
	bool	      _worksForDir;
	bool	      _worksForFile;
	bool	      _worksForDotEntry;
	bool	      _recurse;
	bool	      _askForConfirmation;
	RefreshPolicy _refreshPolicy;
    };


    typedef QList<Cleanup *>		CleanupList;
    typedef CleanupList::const_iterator CleanupListIterator;


    inline QTextStream & operator<< ( QTextStream & stream, const Cleanup * cleanup )
    {
	if ( cleanup )
	    stream << cleanup->id();
	else
	    stream << "<NULL Cleanup *>";

	return stream;
    }
}	// namespace QDirStat


#endif // ifndef Cleanup_h

