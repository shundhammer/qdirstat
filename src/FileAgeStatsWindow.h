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
#include "PercentBar.h"


namespace QDirStat
{
    class YearListItem;

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

	/**
	 * Read settings from the config file
	 **/
	void readSettings();

	/**
	 * Write settings to the config file
	 **/
	void writeSettings();


    signals:

        /**
         * Emitted when the user clicks the "Locate" button (which is only
         * enabled when there are 1..1000 files from that year).
         **/
        void locateFilesFromYear( short year );


    protected slots:

        /**
         * Emit the locateFilesFromYear() signal for the currently selected
         * item's year. Do nothing if nothing is selected.
         **/
        void locateFiles();

        /**
         * Enable or disable actions and buttons depending on the internal
         * state, e.g. if any item is selected and the number of files for the
         * selected year are in the specified range (1..1000).
         **/
        void enableActions();


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

        /**
         * Return the current year.
         *
         * Not named currentYear() to avoid confusion with the year that is
         * currently selected in the tree widget.
         **/
        short thisYear() const;

        /**
         * Return the year that is currently selected in the tree widget
         * or -1 if nothing is selected.
         **/
        short selectedYear() const;

        /**
         * Return the currently selected item in the tree widget or 0
         * if there is none or if it is the wrong type.
         **/
        YearListItem * selectedItem() const;


	//
	// Data members
	//

	Ui::FileAgeStatsWindow * _ui;
	FileAgeStats *		 _stats;
        PercentBarDelegate *     _filesPercentBarDelegate;
        PercentBarDelegate *     _sizePercentBarDelegate;
	Subtree			 _subtree;
        bool                     _startGapsWithCurrentYear;

    };	// class FileAgeStatsWindow


    /**
     * Column numbers for the years tree widget
     **/
    enum YearListColumns
    {
	YearListYearCol,
	YearListFilesCountCol,
	YearListFilesPercentBarCol,
	YearListFilesPercentCol,
	YearListSizeCol,
	YearListSizePercentBarCol,
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
         *
         * Reimplemented from QTreeWidgetItem.
	 **/
	virtual bool operator<( const QTreeWidgetItem & other ) const Q_DECL_OVERRIDE;

        /**
         * Generic data method for different roles. Here used for more exotic
         * settings like the vertical alignment where there is no simpler way.
         *
         * Reimplemented from QTreeWidgetItem.
         **/
        virtual QVariant data( int column, int role ) const Q_DECL_OVERRIDE;

    protected:

	YearStats _stats;

    };	// class YearListItem

}	// namespace QDirStat


#endif // FileAgeStatsWindow_h
