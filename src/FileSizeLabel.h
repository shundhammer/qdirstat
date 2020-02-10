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
         * 'prefix' is an optional text prefix like "> " to indicate that the
         * exact value is unknown (e.g. because of insuficcient permissions in
         * a directory tree). The prefix is also displayed in the context menu.
         *
         * The initial value is -1 which will be formatted as an empty string.
         **/
        void setValue( FileSize val, const QString & prefix = "" );

        /**
         * Return the value.
         **/
        FileSize value() const { return _value; }

        /**
         * Return the last used prefix.
         *
         * Notice that the prefix cannot be set separately, only together with
         * the value in setValue().
         **/
        QString prefix() const { return _prefix; }

        /**
         * Set a custom text. This text may or may not contain the value.
         *
         * The context menu is only displayed if the value is non-negative.
         *
         * The prefix is only used in the context menu. It is the caller's
         * responsibility to also add it to the custom text if that is desired.
         **/
        void setText( const QString &   newText,
                      FileSize          newValue = -1,
                      const QString &   prefix   = "" );


    protected:

        /**
         * Event handler for the context menu event.
         *
         * Reimplemented from QLabel / QWidget.
         **/
        virtual void contextMenuEvent( QContextMenuEvent * event );


        // Data members

        FileSize _value;
        QString  _prefix;
    };

} // namespace QDirStat

#endif // FileSizeLabel_h
