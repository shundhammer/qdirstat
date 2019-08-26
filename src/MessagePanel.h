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
#include <QList>
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

	/**
	 * Return the first instance of this class that still exists or 0 if
	 * there is none.
	 **/
	static MessagePanel * firstInstance();

	/**
	 * Return the last instance of this class that still exists or 0 if
	 * there is none.
	 **/
	static MessagePanel * lastInstance();

	/**
	 * Return 'true' if there is an active instance of this class, 'false'
	 * if not.
	 **/
	static bool haveInstance() { return ! _instances.isEmpty(); }

    protected:

	Ui::MessagePanel * _ui;

	static QList<MessagePanel *> _instances;

    };	// class MessagePanel

}	// namespace QDirStat

#endif	// MessagePanel_h
