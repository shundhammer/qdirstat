/*
 *   File name: FileSizeStatsWindow.h
 *   Summary:	QDirStat file type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileSizeStatsWindow_h
#define FileSizeStatsWindow_h

#include <QDialog>

#include "ui_file-size-stats-window.h"
#include "FileInfo.h"

class QTableWidget;


namespace QDirStat
{
    class DirTree;
    class FileSizeStats;


    /**
     * Modeless dialog to display file size statistics:
     * median, min, max, quartiles etc.
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

    public slots:

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
	 * Calculate the statistics from the tree.
	 **/
	void calc();

	/**
	 * One-time initialization of the widgets in this window.
	 **/
	void initWidgets();

        /**
         * Return text for a quantile of 'order' named 'name'.
         **/
        QStringList quantile( int order, const QString & name );

        /**
         * Fill a quantile table for 'order' quantiles with content.
         **/
        void fillQuantileTable( QTableWidget * table, int order );


	//
	// Data members
	//

	Ui::FileSizeStatsWindow *   _ui;
        FileInfo *                  _subtree;
        QString                     _suffix;
	FileSizeStats *		    _stats;
    };

} // namespace QDirStat


#endif // FileSizeStatsWindow_h
