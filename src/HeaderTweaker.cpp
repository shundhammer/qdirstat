/*
 *   File name: HeaderTweaker.cpp
 *   Summary:	Helper class for DirTreeView
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QMenu>
#include <QAction>

#include "Qt4Compat.h"

#include "HeaderTweaker.h"
#include "DirTreeView.h"
#include "Settings.h"
#include "Logger.h"
#include "Exception.h"
#include "SignalBlocker.h"

using namespace QDirStat;


HeaderTweaker::HeaderTweaker( QHeaderView * header, DirTreeView * parent ):
    QObject( parent ),
    _treeView( parent ),
    _header( header ),
    _currentSection( -1 ),
    _currentLayout( 0 )
{
    CHECK_PTR( _header );

    _header->setSortIndicator( NameCol, Qt::AscendingOrder );
    _header->setStretchLastSection( false );
    _header->setContextMenuPolicy( Qt::CustomContextMenu );

    setAllColumnsAutoSize( true );
    createActions();
    createColumnLayouts();

    connect( _header, SIGNAL( sectionCountChanged( int, int ) ),
	     this,    SLOT  ( initHeader()		      ) );

    connect( _header, SIGNAL( customContextMenuRequested( const QPoint & ) ),
	     this,    SLOT  ( contextMenu		( const QPoint & ) ) );
}


HeaderTweaker::~HeaderTweaker()
{
    if ( _currentLayout )
	saveLayout( _currentLayout );

    writeSettings();

    qDeleteAll( _layouts );
}


void HeaderTweaker::initHeader()
{
    // Initialize stuff when the header actually has sections: It's constructed
    // empty. It is only populated when the tree view model requests header
    // data from the data model.

    // logDebug() << "Header count: " << _header->count() << endl;
    readSettings();
}


void HeaderTweaker::createColumnLayouts()
{
    // Layout L1: Short

    ColumnLayout * layout = new ColumnLayout( "L1" );
    CHECK_PTR( layout );
    _layouts[ "L1" ] = layout;

    layout->columns << NameCol
                    << PercentBarCol
                    << PercentNumCol
                    << SizeCol
                    << LatestMTimeCol;

    // L2: Classic QDirStat Style

    layout = new ColumnLayout( "L2" );
    CHECK_PTR( layout );
    _layouts[ "L2" ] = layout;

    layout->columns << NameCol
                    << PercentBarCol
                    << PercentNumCol
                    << SizeCol
                    << TotalItemsCol
                    << TotalFilesCol
                    << TotalSubDirsCol
                    << LatestMTimeCol;

    // L3: Full

    layout = new ColumnLayout( "L3" );
    CHECK_PTR( layout );
    _layouts[ "L3" ] = layout;

    layout->columns = DataColumns::instance()->allColumns();

    foreach ( layout, _layouts )
	layout->defaultColumns = layout->columns;
}


#define CONNECT_ACTION(ACTION, RECEIVER, RCVR_SLOT) \
    connect( (ACTION), SIGNAL( triggered() ), (RECEIVER), SLOT( RCVR_SLOT ) )


void HeaderTweaker::createActions()
{
    _actionHideCurrentCol = new QAction( tr( "&Hide" ), this );
    CONNECT_ACTION( _actionHideCurrentCol, this, hideCurrentCol() );

    _actionShowAllHiddenColumns = new QAction( tr( "Show &All Hidden Columns" ), this );
    CONNECT_ACTION( _actionShowAllHiddenColumns, this, showAllHiddenColumns() );

    _actionAutoSizeCurrentCol = new QAction( tr( "A&uto Size" ), this );
    _actionAutoSizeCurrentCol->setCheckable( true );

    connect( _actionAutoSizeCurrentCol, SIGNAL( toggled		  ( bool ) ),
	     this,			SLOT  ( autoSizeCurrentCol()	   ) );

    _actionAllColumnsAutoSize = new QAction( tr( "&Auto Size" ), this );
    CONNECT_ACTION( _actionAllColumnsAutoSize, this, setAllColumnsAutoSize() );

    _actionAllColumnsInteractiveSize = new QAction( tr( "&Interactive Size" ), this );
    CONNECT_ACTION( _actionAllColumnsInteractiveSize, this, setAllColumnsInteractiveSize() );

    _actionResetToDefaults = new QAction( tr( "&Reset to Defaults" ), this );
    CONNECT_ACTION( _actionResetToDefaults, this, resetToDefaults() );
}


void HeaderTweaker::updateActions( int section )
{
    _actionHideCurrentCol->setEnabled( section != 0 );

    {
	SignalBlocker sigBlocker( _actionAutoSizeCurrentCol );
	_actionAutoSizeCurrentCol->setChecked( autoSizeCol( section ) );
    }
}


void HeaderTweaker::contextMenu( const QPoint & pos )
{
    _currentSection = _header->logicalIndexAt( pos );
    QString colName = this->colName( _currentSection );
    updateActions( _currentSection );

    QMenu menu;
    menu.addAction( tr( "Column \"%1\"" ).arg( colName ) );
    menu.addSeparator();
    menu.addAction( _actionAutoSizeCurrentCol );
    menu.addAction( _actionHideCurrentCol     );
    menu.addSeparator();
    menu.addMenu( createHiddenColMenu( &menu ) );

    QMenu allColMenu( tr( "&All Columns" ) );
    menu.addMenu( &allColMenu );
    allColMenu.addAction( _actionAllColumnsAutoSize );
    allColMenu.addAction( _actionAllColumnsInteractiveSize );
    allColMenu.addAction( _actionResetToDefaults );

    menu.exec( _header->mapToGlobal( pos ) );
}


QMenu * HeaderTweaker::createHiddenColMenu( QWidget * parent )
{
    int actionCount = 0;
    QMenu * hiddenColMenu = new QMenu( tr( "Hi&dden Columns" ), parent );

    for ( int section = 0; section < _header->count(); ++section )
    {
	if ( _header->isSectionHidden( section ) )
	{
	    ++actionCount;
	    QString text = tr( "Show Column \"%1\"" ).arg( this->colName( section ) );
	    QAction * showAction = new QAction( text, hiddenColMenu );
	    CHECK_NEW( showAction );

	    showAction->setData( section );
	    hiddenColMenu->addAction( showAction );

	    connect( showAction, SIGNAL( triggered() ),
		     this,	 SLOT  ( showHiddenCol()   ) );
	}
    }

    if ( actionCount == 0 )
	hiddenColMenu->setEnabled( false );
    else if ( actionCount > 1 )
    {
	hiddenColMenu->addSeparator();
	hiddenColMenu->addAction( _actionShowAllHiddenColumns );
    }

    return hiddenColMenu;
}


QString HeaderTweaker::colName( int section ) const
{
    DataColumn col = DataColumns::instance()->reverseMappedCol( static_cast<DataColumn>( section ) );
    QString name = _treeView->model()->headerData( col,
						   Qt::Horizontal,
						   Qt::DisplayRole ).toString();
    if ( col == UndefinedCol )
	logError() << "No column at section " << section << endl;

    return name;
}


bool HeaderTweaker::autoSizeCol( int section ) const
{
    return resizeMode( section ) == QHeaderView::ResizeToContents;
}


void HeaderTweaker::hideCurrentCol()
{
    if ( _currentSection >= 0 )
    {
	logDebug() << "Hiding column \"" << colName( _currentSection ) << "\"" << endl;
	_header->setSectionHidden( _currentSection, true );
    }

    _currentSection = -1;
}


void HeaderTweaker::autoSizeCurrentCol()
{
    if ( _currentSection >= 0 )
    {
	setResizeMode( _currentSection,
		       _actionAutoSizeCurrentCol->isChecked() ?
		       QHeaderView::ResizeToContents :
		       QHeaderView::Interactive );
    }
    else
	logWarning() << "No current section" << endl;

    _currentSection = -1;
}


void HeaderTweaker::setAllColumnsAutoSize( bool autoSize )
{
    QHeaderView::ResizeMode resizeMode = autoSize ?
	QHeaderView::ResizeToContents :
	QHeaderView::Interactive;

    for ( int section = 0; section < _header->count(); ++section )
    {
	setResizeMode( section, resizeMode );
    }
}


void HeaderTweaker::setAllColumnsInteractiveSize()
{
    setAllColumnsAutoSize( false );
}


void HeaderTweaker::showHiddenCol()
{
    QAction * action = qobject_cast<QAction *>( sender() );

    if ( ! action )
    {
	logError() << "Wrong sender type: "
		   << sender()->metaObject()->className() << endl;
	return;
    }

    if ( action->data().isValid() )
    {
	int section = action->data().toInt();

	if ( section >= 0 && section < _header->count() )
	{
	    logDebug() << "Showing column \"" << colName( section ) << "\"" << endl;
	    _header->setSectionHidden( section, false );
	}
	else
	    logError() << "Section index out of range: " << section << endl;
    }
    else
    {
	logError() << "No data() set for this QAction" << endl;
    }
}


void HeaderTweaker::showAllHiddenColumns()
{
    for ( int section = 0; section < _header->count(); ++section )
    {
	if ( _header->isSectionHidden( section ) )
	{
	    logDebug() << "Showing column \"" << colName( section ) << "\"" << endl;
	    _header->setSectionHidden( section, false );
	}
    }
}


void HeaderTweaker::resetToDefaults()
{
    if ( _currentLayout )
    {
	_currentLayout->columns = _currentLayout->defaultColumns;
	applyLayout( _currentLayout );
    }
}


void HeaderTweaker::setColumnOrder( const DataColumnList & columns )
{
    DataColumnList colOrderList = columns;
    addMissingColumns( colOrderList );

    int visualIndex = 0;

    foreach ( DataColumn col, colOrderList )
    {
	if ( visualIndex < _header->count() )
	{
	    // logDebug() << "Moving " << col << " to position " << visualIndex << endl;
	    _header->moveSection( _header->visualIndex( col ), visualIndex++ );
	}
	else
	    logWarning() << "More columns than header sections" << endl;
    }
}


void HeaderTweaker::readSettings()
{
    Settings settings;
    settings.beginGroup( "TreeViewColumns" );

    //
    // Set all column widths that are specified
    //

    for ( int section = 0; section < _header->count(); ++section )
    {
	DataColumn col	 = static_cast<DataColumn>( section );
	QString colName	 = DataColumns::toString( col );
	QString widthKey = QString( "Width_%1" ).arg( colName );

	int width = settings.value( widthKey, -1 ).toInt();

	if ( width > 0 )
	{
	    setResizeMode( section, QHeaderView::Interactive );
	    _header->resizeSection( section, width );
	}
	else
	{
	    setResizeMode( section, QHeaderView::ResizeToContents );
	}
    }

    settings.endGroup();

    foreach ( ColumnLayout * layout, _layouts )
	readLayoutSettings( layout );
}


void HeaderTweaker::readLayoutSettings( ColumnLayout * layout )
{
    CHECK_PTR( layout );

    Settings settings;
    settings.beginGroup( QString( "TreeViewLayout_%1" ).arg( layout->name ) );

    QStringList strList = settings.value( "Columns" ).toStringList();
    layout->columns = DataColumns::fromStringList( strList );

    fixupLayout( layout );

    settings.endGroup();
}


void HeaderTweaker::writeSettings()
{
    Settings settings;
    settings.beginGroup( "TreeViewColumns" );

    // Remove any leftovers from old config file versions

    (void) settings.remove( "" ); // Remove all keys in this settings group


    // Save column widths

    for ( int visualIndex = 0; visualIndex < _header->count(); ++visualIndex )
    {
	int logicalIndex = _header->logicalIndex( visualIndex );
	DataColumn col	 = static_cast<DataColumn>( logicalIndex );
	QString colName	 = DataColumns::toString( col );

	QString widthKey = QString( "Width_%1" ).arg( colName );

	if ( autoSizeCol( logicalIndex ) )
	    settings.setValue( widthKey, "auto" );
	else
	    settings.setValue( widthKey, _header->sectionSize( logicalIndex ) );
    }

    settings.endGroup();


    // Save column layouts

    foreach ( ColumnLayout * layout, _layouts )
	writeLayoutSettings( layout );
}


void HeaderTweaker::writeLayoutSettings( ColumnLayout * layout )
{
    CHECK_PTR( layout );

    Settings settings;
    settings.beginGroup( QString( "TreeViewLayout_%1" ).arg( layout->name ) );
    settings.setValue( "Columns", DataColumns::toStringList( layout->columns ) );
    settings.endGroup();
}


void HeaderTweaker::setColumnVisibility( const DataColumnList & columns )
{
    for ( int section = 0; section < _header->count(); ++section )
    {
	DataColumn col = static_cast<DataColumn>( section );
	bool visible   = columns.contains( col );
	_header->setSectionHidden( DataColumns::toViewCol( section ), ! visible );
    }
}


void HeaderTweaker::addMissingColumns( DataColumnList & colList )
{
    foreach ( const DataColumn col, DataColumns::instance()->defaultColumns() )
    {
	if ( ! colList.contains( col ) )
	     colList << col;
    }
}


void HeaderTweaker::changeLayout( const QString & name )
{
    if ( ! _layouts.contains( name ) )
    {
	logError() << "No layout " << name << endl;
	return;
    }

    // logDebug() << "Changing to layout " << name << endl;

    if ( _currentLayout )
	saveLayout( _currentLayout );

    _currentLayout = _layouts[ name ];
    applyLayout( _currentLayout );
}


void HeaderTweaker::saveLayout( ColumnLayout * layout )
{
    CHECK_PTR( layout );

    layout->columns.clear();

    for ( int visualIndex = 0; visualIndex < _header->count(); ++visualIndex )
    {
	int logicalIndex = _header->logicalIndex( visualIndex );
	DataColumn col	 = static_cast<DataColumn>( logicalIndex );

	if ( ! _header->isSectionHidden( logicalIndex ) )
	    layout->columns << col;
    }
}


void HeaderTweaker::applyLayout( ColumnLayout * layout )
{
    CHECK_PTR( layout );

    fixupLayout( layout );
    setColumnOrder( layout->columns );
    setColumnVisibility( layout->columns );
}


void HeaderTweaker::fixupLayout( ColumnLayout * layout )
{
    CHECK_PTR( layout );

    if ( layout->columns.isEmpty() )
    {
	logDebug() << "Falling back to default visible columns" << endl;
	layout->columns = layout->defaultColumns;
    }

    DataColumns::ensureNameColFirst( layout->columns );
}


QHeaderView::ResizeMode HeaderTweaker::resizeMode( int section ) const
{
    return _header->sectionResizeMode( section );
}

void HeaderTweaker::setResizeMode( int section, QHeaderView::ResizeMode resizeMode )
{
    _header->setSectionResizeMode( section, resizeMode );
}


void HeaderTweaker::resizeToContents( QHeaderView * header )
{
    for ( int col = 0; col < header->count(); ++col )
	header->setSectionResizeMode( col, QHeaderView::ResizeToContents );
}
