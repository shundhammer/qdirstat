/*
 *   File name: MainWindowLayout.cpp
 *   Summary:	QDirStat main window layout-related functions
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "MainWindow.h"
#include "QDirStatApp.h"
#include "HeaderTweaker.h"
#include "Settings.h"
#include "Exception.h"
#include "Logger.h"

using namespace QDirStat;


void MainWindow::createLayouts()
{
    // Notice that the column layouts are handled in the HeaderTweaker and its
    // ColumnLayout helper class; see also HeaderTweaker.h and .cpp.
    //
    // The layout names "L1", "L2", "L3" here are important: They need to match
    // the names in the HeaderTweaker.

    TreeLayout * layout;

    layout = new TreeLayout( "L1" );
    CHECK_NEW( layout );
    _layouts[ "L1" ] = layout;

    layout = new TreeLayout( "L2" );
    CHECK_NEW( layout );
    _layouts[ "L2" ] = layout;

    layout = new TreeLayout( "L3" );
    CHECK_NEW( layout );
    _layouts[ "L3" ] = layout;

    // L3 is the only one where the defaults for the flags need changing.
    layout->showDetailsPanel = false;
}


void MainWindow::initLayoutActions()
{
    // Qt Designer does not support QActionGroups; it was there for Qt 3, but
    // they dropped that feature for Qt 4/5.

    _layoutActionGroup = new QActionGroup( this );
    CHECK_NEW( _layoutActionGroup );

    _layoutActionGroup->addAction( _ui->actionLayout1 );
    _layoutActionGroup->addAction( _ui->actionLayout2 );
    _layoutActionGroup->addAction( _ui->actionLayout3 );

    _ui->actionLayout1->setData( "L1" );
    _ui->actionLayout2->setData( "L2" );
    _ui->actionLayout3->setData( "L3" );
}


void MainWindow::changeLayout( const QString & name )
{
    _layoutName = name;

    if ( _layoutName.isEmpty() )
    {
	// Get the layout to use from data() from the QAction that sent the signal.

	QAction * action   = qobject_cast<QAction *>( sender() );
	_layoutName = action && action->data().isValid() ?
	    action->data().toString() : "L2";
    }

    logDebug() << "Changing to layout " << _layoutName << endl;

    _ui->dirTreeView->headerTweaker()->changeLayout( _layoutName );

    if ( _currentLayout )
	saveLayout( _currentLayout );

    if ( _layouts.contains( _layoutName ) )
    {
	_currentLayout = _layouts[ _layoutName ];
	applyLayout( _currentLayout );
    }
    else
    {
	logError() << "No layout " << _layoutName << endl;
    }
}


void MainWindow::saveLayout( TreeLayout * layout )
{
    CHECK_PTR( layout );

    layout->showCurrentPath  = _ui->actionShowCurrentPath->isChecked();
    layout->showDetailsPanel = _ui->actionShowDetailsPanel->isChecked();
}


void MainWindow::applyLayout( TreeLayout * layout )
{
    CHECK_PTR( layout );

    _ui->actionShowCurrentPath->setChecked ( layout->showCurrentPath  );
    _ui->actionShowDetailsPanel->setChecked( layout->showDetailsPanel );
}


void MainWindow::readLayoutSettings( TreeLayout * layout )
{
    CHECK_PTR( layout );

    Settings settings;
    settings.beginGroup( QString( "TreeViewLayout_%1" ).arg( layout->name ) );

    layout->showCurrentPath  = settings.value( "ShowCurrentPath" , layout->showCurrentPath  ).toBool();
    layout->showDetailsPanel = settings.value( "ShowDetailsPanel", layout->showDetailsPanel ).toBool();

    settings.endGroup();
}


void MainWindow::writeLayoutSettings( TreeLayout * layout )
{
    CHECK_PTR( layout );

    Settings settings;
    settings.beginGroup( QString( "TreeViewLayout_%1" ).arg( layout->name ) );

    settings.setValue( "ShowCurrentPath" , layout->showCurrentPath  );
    settings.setValue( "ShowDetailsPanel", layout->showDetailsPanel );

    settings.endGroup();
}

