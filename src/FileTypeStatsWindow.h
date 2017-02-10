/*
 *   File name: FileTypeStatsWindow.h
 *   Summary:	QDirStat file type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileTypeStatsWindow_h
#define FileTypeStatsWindow_h

#include <QDialog>
#include <QMap>

#include "ui_file-type-stats-window.h"
#include "DirInfo.h"


namespace QDirStat
{
    class DirTree;
    class MimeCategorizer;
    class MimeCategory;


    /**
     * Modeless dialog to display file type statistics, such as how much disk
     * space is used for each kind of filename extension (*.jpg, *.mp4 etc.).
     **/
    class FileTypeStatsWindow: public QDialog
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 *
	 * Notice that this widget will destroy itself upon window close.
	 *
	 * It is advised to use a QPointer for storing a pointer to an instance
	 * of this class. The QPointer will keep track of this window
	 * auto-deleting itself when closed.
	 **/
	FileTypeStatsWindow( DirTree * tree, QWidget * parent );

	/**
	 * Destructor.
	 **/
	virtual ~FileTypeStatsWindow();

    public:

	/**
	 * Return the corresponding DirTree.
	 **/
	DirTree * tree() const { return _tree; }

    public slots:

	/**
	 * Calculate the statistics from the tree.
	 **/
	void calc();

	/**
	 * Reject the dialog contents, i.e. the user clicked the "Cancel"
	 * or WM_CLOSE button.
	 *
	 * Reimplemented from QDialog.
	 **/
	virtual void reject() Q_DECL_OVERRIDE;

    protected:

	/**
	 * Clear all data and widget contents.
	 **/
	void clear();

	/**
	 * Collect information from the associated widget tree:
	 *
	 * Recursively go through the tree and collect sizes for each file type
	 * (filename extension).
	 **/
	void collect( FileInfo * dir );

	/**
	 * Remove useless content from the maps. On a Linux system, there tend
	 * to be a lot of files that have a '.' in the name, but it's not a
	 * meaningful suffix but a general-purpose separator for dates, SHAs,
	 * version numbers or whatever. All that stuff accumulates in the maps,
	 * and it's typically just a single file with that non-suffix. This
	 * function tries a best effort to get rid of that stuff.
	 **/
	void removeCruft();

	/**
	 * Check if a suffix is cruft, i.e. a nonstandard suffix that is not
	 * useful for display.
	 *
	 * Notice that this is a highly heuristical algorithm that might give
	 * false positives.
	 **/
	bool isCruft( const QString & suffix );

	/**
	 * Populate the widgets from the collected information.
	 **/
	void populate();


	//
	// Data members
	//

	Ui::FileTypeStatsWindow * _ui;
	DirTree *		  _tree;
	MimeCategorizer *	  _mimeCategorizer;

	QMap<QString, FileSize>	       _suffixSum;
	QMap<QString, int>	       _suffixCount;
	QMap<MimeCategory *, FileSize> _categorySum;
	QMap<MimeCategory *, int>      _categoryCount;
    };
}



#endif // FileTypeStatsWindow_h
