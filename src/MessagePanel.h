/*
 *   File name: MessagePanel.h
 *   Summary:	Container widget for PanelMessages
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef MessagePanel_h
#define MessagePanel_h

#include <QWidget>
#include "ui_message-panel.h"


namespace QDirStat
{
    class PanelMessage;

    /**
     * Container widget for a (small) number of PanelMessages.
     * This widget does not scroll.
     **/
    class MessagePanel: public QWidget
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 **/
	MessagePanel( QWidget * parent );

	/**
	 * Destructor.
	 **/
	virtual ~MessagePanel();

	/**
	 * Add a PanelMessage and show this widget if it is not visible yet.
	 **/
	void add( PanelMessage * msg );

	/**
	 * Delete all PanelMessages.
	 **/
	void clear();

    protected:

	Ui::MessagePanel * _ui;

    };	// class MessagePanel

}	// namespace QDirStat

#endif	// MessagePanel_h
