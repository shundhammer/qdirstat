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


MessagePanel::MessagePanel( QWidget * parent ):
    QWidget( parent ),
    _ui( new Ui::MessagePanel )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );

    // The dummy placeholder is only needed for handling in Qt designer.
    delete _ui->dummy;
    _ui->dummy = 0;
}


MessagePanel::~MessagePanel()
{

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
