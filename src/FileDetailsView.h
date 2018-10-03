/*
 *   File name: FileDetailsView.h
 *   Summary:	Details view for the currently selected file or directory
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef FileDetailsView_h
#define FileDetailsView_h

#include <QStackedWidget>
#include "FileInfoSet.h"
#include "ui_file-details-view.h"


namespace QDirStat
{
    /**
     * Details view for the current selection (file, directory, multiple
     * objects).
     *
     * This shows detailed information such as sizes, permissions, mtime
     * etc. depending on what type of object and how many of them are selected.
     **/
    class FileDetailsView: public QStackedWidget
    {
        Q_OBJECT

    public:

	/**
	 * Constructor
	 **/
        FileDetailsView( QWidget * parent = 0 );

	/**
	 * Destructor
	 **/
        virtual ~FileDetailsView();

        /**
         * Return the label limit, i.e. the maximum number of characters for
         * certain fields that can otherwise grow out of bounds.
         **/
        int labelLimit() const { return _labelLimit; }

        /**
         * Set the label limit. Notice that if a label needs to be limited, it
         * will get three characters less than this value to compensate for the
         * "..." ellipsis that indicates that it was cut off.
         **/
        void setLabelLimit( int newLimit ) { _labelLimit = newLimit; }

    public slots:

        /**
         * Show an empty page
         **/
        void clear();

        /**
         * Show the details of the currently selected items as appropriate:
         * - File details if exactly one file is selected
         * - Directory details if exactly one directory is selected
         * - An empty page is nothing is selected
         * - A selection summary if more than one item is selected
         **/
        void showDetails( const FileInfoSet & selectedItems );

        /**
         * Show details about a file
         **/
        void showDetails( FileInfo * fileInfo );

        /**
         * Show details about a directory
         **/
        void showDetails( DirInfo * dirInfo );

        /**
         * Show a summary of the current selection
         **/
        void showSelectionSummary( const FileInfoSet & selectedItems );

    protected:

        /**
         * Return the path of a fileInfo's parent directory.
         **/
        QString parentPath( FileInfo * fileInfo );

        /**
         * Set a label with a number.
         **/
        void setLabel( QLabel * label, int number );

        /**
         * Set a label with a file size.
         **/
        void setLabel( QLabel * label, FileSize size );

        /**
         * Set a label with a text of limited size.
         **/
        void setLabelLimited( QLabel * label, const QString & text );

        QString limitText( const QString & longText );

        // Boilerplate widget setting methods

        void showFileInfo( FileInfo * file );
        void showFilePkgInfo( FileInfo * file );
        void setSystemFileBlockVisibility( bool visible );

        void showSubtreeInfo( DirInfo * dir );
        void showDirNodeInfo( DirInfo * dir );
        void setDirBlockVisibility( bool visible );


        // Data members

        Ui::FileDetailsView * _ui;
        int                   _labelLimit;

    };  // class FileDetailsView
}	// namespace QDirStat

#endif // FileDetailsView_h
