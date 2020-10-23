/*
 *   File name: PopupLabel.h
 *   Summary:	QLabel with a pop-up
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef PopupLabel_h
#define PopupLabel_h


#include <QLabel>

class QMouseEvent;


namespace QDirStat
{
    /**
     * Label widget with a text and an optional pop-up showing a longer text.
     *
     * The normal text is underlined while the mouse hovers over the label;
     * clicking the left or right mouse button opens the pop-up with the longer
     * text.
     *
     * This is very similar to a label with a tooltip, but much less intrusive
     * since hovering only underlines the normal label, making the feature
     * discoverable.
     *
     * This is just a thin wrapper around QLabel.
     **/
    class PopupLabel: public QLabel
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	PopupLabel( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~PopupLabel();

	// Use setText() and text() (both inherited from QLabel) to set and get
	// the text.

	/**
	 * Directly set the text for the context menu.
	 **/
	virtual void setContextText( const QString & text ) { _contextText = text; }

	/**
	 * Return the text for the context menu (that was set by
	 * setContextText() ).
	 **/
	virtual QString contextText() const { return _contextText; }

	/**
	 * Clear all texts, both the normal visible text and the context text.
	 **/
	virtual void clear();

	/**
	 * Set or reset the font to bold.
	 **/
	void setBold( bool bold = true );


    protected:

	/**
	 * Return 'true' if there is anything that can be displayed in a
	 * context menu.
	 **/
	virtual bool haveContextMenu() const;

	/**
	 * Show the context menu at screen position 'pos' and wait until the
	 * user closes it.  The default implementation creates a menu with one
	 * single action from contextText().
	 **/
	virtual void showContextMenu( const QPoint & pos );

	// Event handlers (all inherited from QWidget)

	virtual void mousePressEvent( QMouseEvent * event ) Q_DECL_OVERRIDE;
	virtual void enterEvent( QEvent * event ) Q_DECL_OVERRIDE;
	virtual void leaveEvent( QEvent * event ) Q_DECL_OVERRIDE;


	// Data members

	QString	 _contextText;
    };

} // namespace QDirStat

#endif // PopupLabel_h
