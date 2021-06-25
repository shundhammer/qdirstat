/*
 *   File name: MessagePanel.cpp
 *   Summary:	Container widget for PanelMessages
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "MessagePanel.h"
#include "PanelMessage.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;

QList<MessagePanel *> MessagePanel::_instances;


MessagePanel::MessagePanel( QWidget * parent ):
    QWidget( parent ),
    _ui( new Ui::MessagePanel )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );

    // The dummy placeholder is only needed for handling in Qt designer.
    delete _ui->dummy;
    _ui->dummy = 0;

    _instances << this;
}


MessagePanel::~MessagePanel()
{
    _instances.removeAll( this );
    delete _ui;
}


void MessagePanel::add( PanelMessage * msg )
{
    CHECK_PTR( msg );

    // TO DO: Check for duplicates

    msg->setParent( this );
    _ui->vBox->addWidget( msg );
    msg->show();
}


void MessagePanel::clear()
{
    foreach ( PanelMessage * msg, findChildren<PanelMessage *>() )
	delete msg;
}


MessagePanel * MessagePanel::firstInstance()
{
    return _instances.isEmpty() ? 0 : _instances.first();
}


MessagePanel * MessagePanel::lastInstance()
{
    return _instances.isEmpty() ? 0 : _instances.last();
}
