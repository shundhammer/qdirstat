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
         * Populate the widgets from the collected information.
         **/
        void populate();


        //
        // Data members
        //

        Ui::FileTypeStatsWindow * _ui;
        QMap<QString, FileSize>   _suffixSum;
        QMap<QString, int>        _suffixCount;

        DirTree *                 _tree;
    };
}



#endif // FileTypeStatsWindow_h
