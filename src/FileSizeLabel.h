/*
 *   File name: FileSizeLabel.h
 *   Summary:	Specialized QLabel for a file size for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef FileSizeLabel_h
#define FileSizeLabel_h


#include <QLabel>

#include "FileInfo.h"   // FileSize

class QContextMenuEvent;


namespace QDirStat
{
    /**
     * Widget to display a file size in human readable form (i.e. "123.4 MB")
     * and with a context menu that displays the exact byte size.
     *
     * This is just a thin wrapper around QLabel.
     **/
    class FileSizeLabel: public QLabel
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	FileSizeLabel( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~FileSizeLabel();

        /**
         * Set the value. This will also format the value and display it in
         * human-readable format, i.e. something like "123.4 MB".
         *
         * The initial value is -1 which will be formatted as an empty string.
         **/
        void setValue( FileSize val );

        /**
         * Return the value.
         **/
        FileSize value() const { return _value; }

        /**
         * Set the text if something else than a file size is to be
         * displayed. This also sets the value to -1.
         **/
        void setText( const QString & newText );


    protected:

        /**
         * Event handler for the context menu event.
         *
         * Reimplemented from QLabel / QWidget.
         **/
        virtual void contextMenuEvent( QContextMenuEvent * event );


        // Data members

        FileSize _value;
    };

} // namespace QDirStat

#endif // FileSizeLabel_h
