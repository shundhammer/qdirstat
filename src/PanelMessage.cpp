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




PanelMessage::PanelMessage( QWidget * parent ):
    QWidget( parent ),
    _ui( new Ui::PanelMessage )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );
    initDetailsLinkLabel();

    _ui->headingLabel->hide();
    _ui->msgLabel->hide();
    _ui->detailsLinkLabel->hide();
}


PanelMessage::~PanelMessage()
{
    delete _ui;
}


void PanelMessage::initDetailsLinkLabel()
{
    // Override the HTML on the "Details..." label
    // to sanitize it from undesired styling (GitHub issue #213)

    QString html = "<a href=\"details\">";
    html += tr( "Details..." );
    html += "</a>";

    _ui->detailsLinkLabel->setTextFormat( Qt::RichText );
    _ui->detailsLinkLabel->setText( html );
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
