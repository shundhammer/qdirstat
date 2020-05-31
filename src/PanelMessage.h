/*
 *   File name: PanelMessage.h
 *   Summary:	Message in a panel with icon and close button
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef PanelMessage_h
#define PanelMessage_h

#include <QWidget>
#include "ui_panel-message.h"


namespace QDirStat
{
    class PanelMessage;

    /**
     * Message in a small panel with an icon, a bold face heading,
     * a message text, an optional "Details..." hyperlink
     * and a small [x] window close button.
     *
     * Those panels are intended to be put into a MessagePanel
     * (but any Qt container widget can be used as well).
     *
     * The close button calls deleteLater on the panel, so it is completely
     * self-sufficient once set up.
     **/
    class PanelMessage: public QWidget
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 **/
	PanelMessage( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~PanelMessage();

	/**
	 * Set the heading text.
	 **/
	void setHeading( const QString & headingText );

	/**
	 * Set the body text.
	 **/
	void setText( const QString & bodyText );

	/**
	 * Set the icon. If not set, a generic light bulb icon is used.
	 **/
	void setIcon( const QPixmap & pixmap );

	/**
	 * Connect the "Details..." hyperlink to a receiver's slot.
	 * The hyperlink is only shown if it is connected.
	 *
	 * Use the same Qt macros as if connecting a normal widget:
	 *
	 *   connectDetailsLink( someAction, SLOT( triggered() ) );
	 **/
	void connectDetailsLink( const QObject * receiver,
				 const char    * slotName );

        /**
         * Connect the "Details..." hyperlink to a web URL that will be opened
         * in an external browser.
         **/
        void setDetailsUrl( const QString url );

        /**
         * Return the URL set with setDetailsUrl().
         **/
        QString detailsUrl() const { return _detailsUrl; }


    protected slots:

        /**
         * Open the URL set with setDetailsUrl() in an external browser.
         **/
        void openDetailsUrl() const;


    protected:

	Ui::PanelMessage * _ui;
        QString            _detailsUrl;

    };	// class PanelMessage

}	// namespace QDirStat

#endif	// PanelMessage_h
