/*
 *   File name: MainWindowHelp.cpp
 *   Summary:	Help menu actions in the QDirStat main window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include <QMessageBox>

#include "MainWindow.h"
#include "SysUtil.h"
#include "Version.h"
#include "Exception.h"
#include "Logger.h"

using namespace QDirStat;


void MainWindow::showAboutDialog()
{
    QString homePage = "https://github.com/shundhammer/qdirstat";
    QString mailTo   = "qdirstat@gmx.de";

    QString text = QString( "<h2>QDirStat %1</h2>" ).arg( QDIRSTAT_VERSION );
    text += "<p>";
    text += tr( "Qt-based directory statistics -- showing where all your disk space has gone "
		" and trying to help you to clean it up." );
    text += "</p><p>";
    text += "(c) 2015-2022 Stefan Hundhammer";
    text += "</p><p>";
    text += tr( "Contact: " ) + QString( "<a href=\"mailto:%1\">%2</a>" ).arg( mailTo ).arg( mailTo );
    text += "</p><p>";
    text += QString( "<p><a href=\"%1\">%2</a></p>" ).arg( homePage ).arg( homePage );
    text += tr( "License: GPL V2 (GNU General Public License Version 2)" );
    text += "</p><p>";
    text += tr( "This is free Open Source software, provided to you hoping that it might be "
		"useful for you. It does not cost you anything, but on the other hand there "
		"is no warranty or promise of anything." );
    text += "</p><p>";
    text += tr( "This software was made with the best intentions and greatest care, but still "
		"there is the off chance that something might go wrong which might damage "
		"data on your computer. Under no circumstances will the authors of this program "
		"be held responsible for anything like that. Use this program at your own risk." );
    text += "</p>";

    QMessageBox::about( this, tr( "About QDirStat" ), text );
}


void MainWindow::showDonateDialog()
{
    QString dUrl = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=EYJXAVLGNRR5W";

    QString text = "<h2>Donate</h2>";
    text += "<p>";
    text += tr( "QDirStat is Free Open Source Software." );
    text += "</p><p>";
    text += tr( "If you find it useful, please consider donating.\n" );
    text += tr( "You can donate any amount of your choice:" );
    text += "</p><p>";

    text.replace( " ", "&nbsp;" );

    text += QString( "<a href=\"%1\">QDirStat at PayPal</a>" ).arg(_dUrl );
    text += "</p><p>";
    text += tr( "(external browser window)" );
    text += "</p>";


    QMessageBox::about( this, tr( "Donate" ), text );
}


void MainWindow::openActionUrl()
{
    // Use a QAction that was set up in Qt Designer to just open an URL in an
    // external web browser.
    //
    // This misappropriates the action's statusTip property to store the URL in
    // a field that is easily accessible in Qt Designer, yet doesn't get in the
    // way: It's not displayed automatically unlike the toolTip property.

    QAction * action = qobject_cast<QAction *>( sender() );

    if ( action )
    {
        QString url = action->statusTip();

        if ( url.isEmpty() )
            logError() << "No URL in statusTip() for action " << action->objectName();
        else
            SysUtil::openInBrowser( url );
    }
}
