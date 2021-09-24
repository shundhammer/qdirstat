/*
 *   File name: FileSizeStatsWindow.h
 *   Summary:	QDirStat file size statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileSizeStatsWindow_h
#define FileSizeStatsWindow_h

#include <QDialog>
#include <QPointer>

#include "ui_file-size-stats-window.h"
#include "FileInfo.h"

class QTableWidget;


namespace QDirStat
{
    class DirTree;
    class FileSizeStats;
    class BucketsTableModel;


    /**
     * Modeless dialog to display file size statistics:
     * median, min, max, quartiles; histogram; percentiles table.
     **/
    class FileSizeStatsWindow: public QDialog
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
	FileSizeStatsWindow( QWidget  * parent );

	/**
	 * Destructor.
	 **/
	virtual ~FileSizeStatsWindow();

        /**
         * Populate with new content.
         **/
	void populate( FileInfo * subtree, const QString & suffix = "" );

	/**
	 * Return the corresponding subtree.
	 **/
	FileInfo * subtree() const { return _subtree; }

        /**
         * Return the filename suffix to filter the collected information.
         **/
        QString suffix() const { return _suffix; }

        /**
         * Static method for using one shared instance of this class between
         * multiple parts of the application. This will create a new instance
         * if there is none yet (or anymore).
         *
         * Do not hold on to this pointer; the instance destroys itself when
         * the user closes the window, and then the pointer becomes invalid.
         *
         * After getting this shared instance, call populate() and show().
         **/
        static FileSizeStatsWindow * sharedInstance();

        /**
         * Convenience function for creating, populating and showing the shared
         * instance.
         **/
        static void populateSharedInstance( FileInfo *      subtree,
                                            const QString & suffix = "" );


    public slots:

	/**
	 * Reject the dialog contents, i.e. the user clicked the "Cancel"
	 * or WM_CLOSE button.
	 *
	 * Reimplemented from QDialog.
	 **/
	virtual void reject() Q_DECL_OVERRIDE;


    protected slots:

        /**
         * Fill the percentiles table depending on the content of the filter
         * combo box in the same tab.
         **/
        void fillPercentileTable();

        /**
         * Make the histogram options visible or invisible.
         **/
        void toggleOptions();

        /**
         * Check the options widgets for any new values, apply them and rebuild
         * the histogram.
         **/
        void applyOptions();

        /**
         * Calculate automatic values for the start and end percentiles, apply
         * them and rebuild the histogram.
         **/
        void autoPercentiles();

        /**
         * Show help for a topic determined by the sender of this signal.
         **/
        void showHelp();


    protected:

	/**
	 * Clear all data and widget contents.
	 **/
	void clear();

	/**
	 * Calculate the statistics from the tree.
	 **/
	void calc();

	/**
	 * One-time initialization of the widgets in this window.
	 **/
	void initWidgets();

        /**
         * Update the values for the option widgets from the current ones from
         * the histogram.
         **/
        void updateOptions();

        /**
         * Return text for all quantiles of 'order' named 'name'.
         **/
        QStringList quantile( int order, const QString & name );

        /**
         * Fill a quantile table for 'order' quantiles with content.
         *
         * 'sums' (if non-empty) is a list of accumulated sums between one
         * quantile and its previous one.
         *
         * 'step' is the step width; 'extremesMargin' specifies how far from
         * the extremes (min, max) the step width should be 1 instead.
         **/
        void fillQuantileTable( QTableWidget *    table,
                                int               order,
                                const QString &   namePrefix = "",
                                const QRealList & sums = QRealList(),
                                int               step = 1,
                                int               extremesMargin = 1 );

        /**
         * Add an item to a table.
         **/
        QTableWidgetItem * addItem( QTableWidget *  table,
                                    int             row,
                                    int             col,
                                    const QString & text );

        /**
         * Set the font to bold for all items in a table row.
         **/
        void setRowBold( QTableWidget * table, int row );

        /**
         * Set the foreground (the text color) for all items in a table row.
         **/
        void setRowForeground( QTableWidget * table, int row, const QBrush & brush );

        /**
         * Set the background for all items in a table row.
         **/
        void setRowBackground( QTableWidget * table, int row, const QBrush & brush );

        /**
         * Set the text alignment for all items in a table column.
         **/
        void setColAlignment( QTableWidget * table, int col, int alignment );

        /**
         * Fill the histogram with content.
         **/
        void fillHistogram();

        /**
         * Fill the buckets table with content.
         **/
        void fillBucketsTable();

        /**
         * Provide data for the histogram.
         **/
        void fillBuckets();


	//
	// Data members
	//

	Ui::FileSizeStatsWindow *   _ui;
        FileInfo *                  _subtree;
        QString                     _suffix;
	FileSizeStats *		    _stats;
        BucketsTableModel *         _bucketsTableModel;

        static QPointer<FileSizeStatsWindow> _sharedInstance;
    };

} // namespace QDirStat


#endif // FileSizeStatsWindow_h
