/*
 *   File name: FileSizeLabel.h
 *   Summary:	Specialized QLabel for a file size for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef FileSizeLabel_h
#define FileSizeLabel_h


#include "PopupLabel.h"
#include "FileInfo.h"	// FileSize

class QMouseEvent;


namespace QDirStat
{
    /**
     * Widget to display a file size in human readable form (i.e. "123.4 MB")
     * and with a context menu that displays the exact byte size.
     *
     * This is just a thin wrapper around PopupLabel / QLabel.
     **/
    class FileSizeLabel: public PopupLabel
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
	void setText( const QString &	newText,
		      FileSize		newValue = -1,
		      const QString &	prefix	 = "" );

	/**
	 * Return the text for the context menu (that was set by
	 * setContextText() ).
	 **/
	virtual QString contextText() const;

	/**
	 * Clear everything, including the visible text, the numeric value,
	 * the context menu text and the bold font.
	 **/
	virtual void clear();


    protected:

	/**
	 * Return 'true' if there is anything that can be displayed in a
	 * context menu.
	 **/
	bool haveContextMenu() const;


	// Data members

	FileSize _value;
	QString	 _prefix;
    };

} // namespace QDirStat

#endif // FileSizeLabel_h
