/*
 *   File name: MainWindowLayout.cpp
 *   Summary:	Unpackaged files view functions in the QDirStat main window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "MainWindow.h"
#include "QDirStatApp.h"
#include "ShowUnpkgFilesDialog.h"
#include "DirTreePatternFilter.h"
#include "DirTreePkgFilter.h"
#include "PkgManager.h"
#include "PkgQuery.h"
#include "ExcludeRules.h"
#include "BusyPopup.h"
#include "Exception.h"
#include "Logger.h"

using namespace QDirStat;


void MainWindow::askShowUnpkgFiles()
{
    PkgManager * pkgManager = PkgQuery::primaryPkgManager();

    if ( ! pkgManager )
    {
	logError() << "No supported primary package manager" << endl;
	return;
    }

    ShowUnpkgFilesDialog dialog( this );

    if ( dialog.exec() == QDialog::Accepted )
	showUnpkgFiles( dialog.values() );
}


void MainWindow::showUnpkgFiles( const QString & url )
{
    UnpkgSettings unpkgSettings( UnpkgSettings::ReadFromConfig );
    unpkgSettings.startingDir = url;

    showUnpkgFiles( unpkgSettings );
}


void MainWindow::showUnpkgFiles( const UnpkgSettings & unpkgSettings )
{
    logDebug() << "Settings:" << endl;
    unpkgSettings.dump();

    PkgManager * pkgManager = PkgQuery::primaryPkgManager();

    if ( ! pkgManager )
    {
	logError() << "No supported primary package manager" << endl;
	return;
    }

    app()->dirTreeModel()->clear(); // For instant feedback
    BusyPopup msg( tr( "Reading file lists..." ), this );


    setUnpkgExcludeRules( unpkgSettings );
    setUnpkgFilters( unpkgSettings, pkgManager );

    // Start reading the directory

    try
    {
        QString dir = parseUnpkgStartingDir( unpkgSettings );

	app()->dirTreeModel()->openUrl( dir );
	updateWindowTitle( app()->dirTree()->url() );
    }
    catch ( const SysCallFailedException & ex )
    {
	CAUGHT( ex );
        showOpenDirErrorPopup( ex );
    }

    updateActions();
}


void MainWindow::setUnpkgExcludeRules( const UnpkgSettings & unpkgSettings )
{
    // Set up the exclude rules for directories that should be ignored

    ExcludeRules * excludeRules = new ExcludeRules( unpkgSettings.excludeDirs );
    CHECK_NEW( excludeRules );

    app()->dirTree()->setExcludeRules( excludeRules );
}


void MainWindow::setUnpkgFilters( const UnpkgSettings & unpkgSettings,
                                  PkgManager          * pkgManager )
{
    // Filter for ignoring all files from all installed packages

    DirTreeFilter * filter = new DirTreePkgFilter( pkgManager );
    CHECK_NEW( filter );

    app()->dirTree()->clearFilters();
    app()->dirTree()->addFilter( filter );


    // Add the filters for each file pattern the user explicitly requested to ignore

    foreach ( const QString & pattern, unpkgSettings.ignorePatterns )
    {
	app()->dirTree()->addFilter( DirTreePatternFilter::create( pattern ) );
    }
}


QString MainWindow::parseUnpkgStartingDir( const UnpkgSettings & unpkgSettings )
{
    QString dir = unpkgSettings.startingDir;
    dir.replace( QRegExp( "^unpkg:" ), "" );

    if ( dir != unpkgSettings.startingDir )
	logInfo() << "Parsed starting dir: " << dir << endl;

    return dir;
}


bool MainWindow::isUnpkgUrl( const QString & url )
{
    return url.startsWith( "unpkg:/" );
}
