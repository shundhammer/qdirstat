/*
 *   File name: PanelMessage.cpp
 *   Summary:	Message in a panel with icon and close button
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "PanelMessage.h"
#include "SysUtil.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


PanelMessage::PanelMessage( QWidget * parent ):
    QWidget( parent ),
    _ui( new Ui::PanelMessage )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );
    _ui->headingLabel->hide();
    _ui->msgLabel->hide();
    _ui->detailsLinkLabel->hide();
}


PanelMessage::~PanelMessage()
{
    delete _ui;
}


void PanelMessage::setHeading( const QString & headingText )
{
    _ui->headingLabel->setText( headingText );
    _ui->headingLabel->show();
}


void PanelMessage::setText( const QString & bodyText )
{
    _ui->msgLabel->setText( bodyText );
    _ui->msgLabel->show();
}


void PanelMessage::setIcon( const QPixmap & pixmap )
{
    if ( ! pixmap.isNull() )
    {
	_ui->iconLabel->setPixmap( pixmap );
	_ui->iconLabel->show();
    }
    else
    {
	_ui->iconLabel->hide();
    }
}


void PanelMessage::connectDetailsLink( const QObject * receiver,
				       const char    * slotName )
{
    if ( receiver )
    {
	connect( _ui->detailsLinkLabel, SIGNAL( linkActivated( QString ) ),
		 receiver, slotName );

	_ui->detailsLinkLabel->show();
    }
    else
    {
	_ui->detailsLinkLabel->hide();
    }
}


void PanelMessage::setDetailsUrl( const QString url )
{
    _detailsUrl = url;
    connectDetailsLink( this, SLOT( openDetailsUrl() ) );
}


void PanelMessage::openDetailsUrl() const
{
    SysUtil::openInBrowser( _detailsUrl );
}
