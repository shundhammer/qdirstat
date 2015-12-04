/*
 *   File name:	Cleanup.h
 *   Summary:	Support classes for QDirStat
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef Cleanup_h
#define Cleanup_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <qdict.h>
#include <qintdict.h>
#include <kaction.h>
#include "Logger.h"
#include "kdirtree.h"


namespace QDirStat
{
    /**
     * Cleanup action to be performed for @ref DirTree items.
     *
     * @short QDirStat cleanup action
     **/

    class Cleanup: public KAction
    {
	Q_OBJECT

    public:

	enum RefreshPolicy { noRefresh, refreshThis, refreshParent, assumeDeleted };

	/**
	 * Constructor.
	 *
	 * 'id' is the name of this cleanup action as used in the XML UI file
	 * and config files, 'title' is the human readable menu title.
	 * 'command' is the shell command to execute.
	 *
	 * Most applications will want to pass KMainWindow::actionCollection()
	 * for 'parent' so the menus and toolbars can be created using the XML
	 * UI description ('kdirstatui.rc' for QDirStat).
	 **/
	Cleanup( QString		id		= "",
		  QString		command 	= "",
		  QString		title 		= "",
		  KActionCollection *	parent		= 0 );

	/**
	 * Copy Constructor.
	 *
	 * Notice that this is a not quite complete copy constructor: Since
	 * there is no KAction copy constructor, the inherited KAction members
	 * will be constructed with the KAction default constructor. Thus, an
	 * object created with this copy constructor can rely only on its
	 * Cleanup members. This is intended for save/restore operations only,
	 * not for general use. In particular, DO NOT connect an object thus
	 * constructed with signals. The results will be undefined (at best).
	 **/
	Cleanup( const KCleanup &src );

	/**
	 * Assignment operator.
	 *
	 * This will not modify the KAction members, just the Cleanup
	 * members. Just like the copy constructor, this is intended for
	 * save/restore operations, not for general use.
	 **/
	Cleanup &	operator= ( const KCleanup &src );

	/**
	 * Return the ID (name) of this cleanup action as used for setup files
	 * and the XML UI description. This ID should be unique within the
	 * application.
	 **/
	const QString &	id()		const { return _id; }
   
	/**
	 * Return the command line that will be executed upon calling @ref
	 * Cleanup::execute(). This command line may contain %p for the
	 * complete path of the directory or file concerned or %n for the pure
	 * file or directory name without path.
	 **/
	const QString &	command()	const { return _command; }

	/**
	 * Return the user title of this command as displayed in menus.
	 * This may include '&' characters for keyboard shortcuts.
	 * See also @ref cleanTitle() .
	 **/
	const QString &	title()		const { return _title; }

	/**
	 * Returns the cleanup action's title without '&' keyboard shortcuts.
	 * Uses the ID as fallback if the name is empty.
	 **/
	QString cleanTitle() const;

	/**
	 * Return whether or not this cleanup action is generally enabled.
	 **/
	bool enabled()			const { return _enabled; }

	/**
	 * Return this cleanup's internally stored @ref DirTree
	 * selection. Important only for copy constructor etc.
	 **/
	FileInfo * selection()		const { return _selection; }

	/**
	 * Return whether or not this cleanup action works for this particular
	 * FileInfo. Checks all the other conditions (enabled(),
	 * worksForDir(), worksForFile(), ...) accordingly.
	 **/
	bool worksFor( FileInfo *item ) const;

	/**
	 * Return whether or not this cleanup action works for directories,
	 * i.e. whether or not @ref Cleanup::execute() will be successful if
	 * the object passed is a directory.
	 **/
	bool worksForDir()		const { return _worksForDir; }

	/**
	 * Return whether or not this cleanup action works for plain files.
	 **/
	bool worksForFile()		const { return _worksForFile; }

	/**
	 * Return whether or not this cleanup action works for QDirStat's
	 * special 'Dot Entry' items, i.e. the pseudo nodes created in most
	 * directories that hold the plain files.
	 **/
	bool worksForDotEntry()		const { return _worksForDotEntry; }

	/**
	 * Return whether or not this cleanup action works for simple local
	 * files and directories only ('file:/' protocol) or network
	 * transparent, i.e. all protocols KDE supports ('ftp', 'smb' - but
	 * even 'tar', even though it is - strictly spoken - local).
	 **/
	bool worksLocalOnly()		const { return _worksLocalOnly; }

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
	bool askForConfirmation()	const { return _askForConfirmation; }

	/**
	 * Return the refresh policy of this cleanup action - i.e. the action
	 * to perform after each call to Cleanup::execute(). This is supposed
	 * to bring the corresponding DirTree back into sync after the cleanup
	 * action - the underlying file tree might have changed due to that
	 * cleanup action.
	 *
	 * noRefresh: Don't refresh anything. Assume nothing has changed.
	 * This is the default.
	 *
	 * refreshThis: Refresh the DirTree from the item on that was passed
	 * to Cleanup::execute().
	 *
	 * refreshParent: Refresh the DirTree from the parent of the item on
	 * that was passed to Cleanup::execute(). If there is no such parent,
	 * refresh the entire tree.
	 *
	 * assumeDeleted: Do not actually refresh the DirTree.  Instead,
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


	void setTitle			( const QString &title	);
	void setId			( const QString &id	) 	{ _id			= id;		}
	void setCommand			( const QString &command) 	{ _command 		= command;	}
	void setEnabled			( bool enabled	)		{ _enabled		= enabled;	}
	void setWorksForDir		( bool canDo 	)		{ _worksForDir		= canDo; 	}
	void setWorksForFile		( bool canDo 	)		{ _worksForFile		= canDo; 	}
	void setWorksForDotEntry	( bool canDo 	)		{ _worksForDotEntry	= canDo; 	}
	void setWorksLocalOnly		( bool canDo 	)		{ _worksLocalOnly	= canDo; 	}
	void setRecurse			( bool recurse	)		{ _recurse		= recurse; 	}
	void setAskForConfirmation	( bool ask	)		{ _askForConfirmation	= ask;		}
	void setRefreshPolicy		( enum RefreshPolicy refreshPolicy ) { _refreshPolicy = refreshPolicy; 	}


    public slots:

	/**
	 * The heart of the matter: Perform the cleanup with the FileInfo
	 * specified.
	 **/
        void execute( FileInfo *item );

	/**
	 * Perform the cleanup with the current DirTree selection if there is
	 * any.
	 **/
	void executeWithSelection();

	/**
	 * Set enabled/disabled status according to 'selection' and internally
	 * store 'selection' - this will also be used upon calling
	 * @ref executeWithSelection() . '0' means "nothing selected".
	 **/
	void selectionChanged( FileInfo *selection );

        /**
	 * Read configuration.
	 **/
        void readConfig();

	/**
	 * Save configuration.
	 **/
	void saveConfig() const;


    signals:

	/**
	 * Emitted after the action is executed.
	 *
	 * Please note that there intentionally is no reference as to which
	 * object the action was executed upon since this object very likely
	 * doesn't exist any more.
	 **/
	void executed();

	
    protected slots:

        /**
	 * Inherited from @ref KAction : Perform the action.
	 * In this case, execute the cleanup with the current selection.
	 **/
        virtual void slotActivated() { executeWithSelection(); }

    
    protected:

	/**
	 * Recursively perform the cleanup.
	 **/
	void executeRecursive( FileInfo *item );

	/**
	 * Ask user for confirmation to execute this cleanup action for
	 * 'item'. Returns 'true' if user accepts, 'false' otherwise.
	 **/
	bool confirmation( FileInfo *item );
	
	/**
	 * Retrieve the directory part of a FileInfo's path.
	 **/
	const QString itemDir( const FileInfo *item ) const;

	/**
	 * Expand some variables in string 'unexpanded' to information from
	 * within 'item'. Multiple expansion is performed as needed, i.e. the
	 * string may contain more than one variable to expand.  The resulting
	 * string is returned.
	 *
	 * %p expands to item->path(), i.e. the item's full path name.
	 *
	 * /usr/local/bin		for that directory
	 * /usr/local/bin/doit	for a file within it
	 *
	 * %n expands to item->name(), i.e. the last component of the pathname.
	 * The examples above would expand to:
	 *
	 * bin
	 * doit
	 *
	 * For commands that are to be executed from within the 'Clean up'
	 * menu, you might specify something like:
	 *
	 * "kfmclient openURL %p"
	 * "tar czvf %{name}.tgz && rm -rf %{name}"
	 **/
	QString	expandVariables ( const FileInfo *	item,
				  const QString &	unexpanded ) const;

	/**
	 * Run a command with 'item' as base to expand variables.
	 **/
	void	runCommand	( const FileInfo *	item,
				  const QString &	command ) const;

	/**
	 * Internal implementation of the copy constructor and assignment
	 * operator: Copy all data members from 'src'.
	 **/
	void	copy		( const Cleanup &src );


	//
	// Data members
	//
	
	FileInfo *		_selection;
	QString			_id;
	QString			_command;
	QString			_title;
	bool			_enabled;
	bool			_worksForDir;
	bool			_worksForFile;
	bool			_worksForDotEntry;
	bool			_worksLocalOnly;
	bool			_recurse;
	bool			_askForConfirmation;
	enum RefreshPolicy	_refreshPolicy;
    };


    inline kdbgstream & operator<< ( kdbgstream & stream, const Cleanup * cleanup )
    {
	if ( cleanup )
	    stream << cleanup->id();
	else
	    stream << "<NULL>";
	
	return stream;
    }
}	// namespace QDirStat


#endif // ifndef Cleanup_h


// EOF
