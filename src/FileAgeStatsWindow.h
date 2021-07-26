/*
 *   File name: FileAgeStatsWindow.h
 *   Summary:	QDirStat "File Age Statistics" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileAgeStatsWindow_h
#define FileAgeStatsWindow_h

#include <QDialog>
#include <QTreeWidgetItem>

#include "ui_file-age-stats-window.h"
#include "FileAgeStats.h"
#include "FileInfo.h"
#include "Subtree.h"


namespace QDirStat
{
    /**
     * Modeless dialog to display file age statistics, i.e. statistics about
     * the years of last modification times of files in a subtree.
     **/
    class FileAgeStatsWindow: public QDialog
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 *
	 * Notice that this widget will destroy itself upon window close.
	 **/
	FileAgeStatsWindow( QWidget * parent );

	/**
	 * Destructor.
	 **/
	virtual ~FileAgeStatsWindow();


	/**
	 * Obtain the subtree from the last used URL or 0 if none was found.
	 **/
	const Subtree & subtree() const { return _subtree; }


    public slots:

	/**
	 * Populate the window.
	 **/
	void populate( FileInfo * subtree = 0 );

        /**
         * Populate the window if the "sync" check box is checked.
         **/
        void syncedPopulate( FileInfo * subtree );

	/**
	 * Refresh (reload) all data.
	 **/
	void refresh();

	/**
	 * Reject the dialog contents, i.e. the user clicked the "Cancel" or
	 * WM_CLOSE button. This not only closes the dialog, it also deletes
	 * it.
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
	 * One-time initialization of the widgets in this window.
	 **/
	void initWidgets();

	/**
	 * Create an item in the years tree / list widget for each year
	 **/
	void populateListWidget();

        /**
         * Fill the gaps between years.
         **/
        void fillGaps();

        /**
         * Find the gaps between years.
         **/
        YearsList findGaps();


	//
	// Data members
	//

	Ui::FileAgeStatsWindow * _ui;
	FileAgeStats *		 _stats;
	Subtree			 _subtree;

    };	// class FileAgeStatsWindow


    /**
     * Column numbers for the years tree widget
     **/
    enum YearListColumns
    {
	YearListYearCol,
	YearListFilesCountCol,
	YearListFilesPercentCol,
	YearListSizeCol,
	YearListSizePercentCol,
	YearListColumnCount
    };


    /**
     * Item class for the years list (which is really a tree widget),
     * representing one year with accumulated values.
     **/
    class YearListItem: public QTreeWidgetItem
    {
    public:

	/**
	 * Constructor.
	 **/
	YearListItem( const YearStats & yearStats );

	/**
	 * Return the statistics values for comparison.
	 **/
	const YearStats & stats() const { return _stats; }

	/**
	 * Less-than operator for sorting.
	 **/
	virtual bool operator<( const QTreeWidgetItem & other ) const Q_DECL_OVERRIDE;

    protected:

	Ui::FileAgeStatsWindow * _ui;
	YearStats		 _stats;
	Subtree			 _subtree;

    };	// class YearListItem

}	// namespace QDirStat


#endif // FileAgeStatsWindow_h
