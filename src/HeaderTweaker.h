/*
 *   File name: HeaderTweaker.h
 *   Summary:	Helper class for DirTreeView
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef HeaderTweaker_h
#define HeaderTweaker_h


#include <QHeaderView>
#include <QMap>
#include "DataColumns.h"

class QHeaderView;
class QAction;
class QMenu;


namespace QDirStat
{
    class DirTreeView;
    class ColumnLayout;

    /**
     * Decorator class for a DirTreeView's QHeaderView that takes care about
     * the header's context menu and the corresponding actions and saving and
     * restoring state.
     **/
    class HeaderTweaker: public QObject
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	HeaderTweaker( QHeaderView * header, DirTreeView * parent );

	/**
	 * Destructor.
	 **/
	virtual ~HeaderTweaker();

	/**
	 * Order the columns according to 'colOrderList'.
	 **/
	void setColumnOrder( const DataColumnList & colOrderList);

	/**
	 * Resize a header view to contents.
	 **/
	static void resizeToContents( QHeaderView * header );

    public slots:

	/**
	 * Set auto size mode for all columns on or off.
	 **/
	void setAllColumnsAutoSize( bool autoSize = true );

	/**
	 * Set interactive size mode (i.e. auto size mode off) for all columns.
	 **/
	void setAllColumnsInteractiveSize();

	/**
	 * Make all hidden columns visible again.
	 **/
	void showAllHiddenColumns();

	/**
	 * Reset all columns to defaults: Column order, visibility, auto size.
	 **/
	void resetToDefaults();

	/**
	 * Read parameters from the settings file.
	 **/
	void readSettings();

	/**
	 * Write parameters to the settings file.
	 **/
	void writeSettings();

	/**
	 * Switch the layout to the one with the specified name.
	 **/
	void changeLayout( const QString & name );


    protected slots:

	/**
	 * Initialize the header view. This makes sense only when it has
	 * columns, i.e. when the model is set, and the parent QTreeView
	 * requested header data. It makes most sense to connect this slot with
	 * the header's sectionCountChanged() signal.
	 **/
	void initHeader();

	/**
	 * Post a context menu for the header at 'pos'.
	 **/
	void contextMenu( const QPoint & pos );

	/**
	 * Hide the current column.
	 **/
	void hideCurrentCol();

	/**
	 * Show the hidden column from sender()->data().
	 **/
	void showHiddenCol();

	/**
	 * Toggle auto size of the current column.
	 **/
	void autoSizeCurrentCol();

	/**
	 * Read the settings for a layout.
	 **/
	void readLayoutSettings( ColumnLayout * layout );

	/**
	 * Write the settings for a layout.
	 **/
	void writeLayoutSettings( ColumnLayout * layout );


    protected:

	/**
	 * Create internally used actions and connect them to the appropriate
	 * slots.
	 **/
	void createActions();

	/**
	 * Create the column layouts.
	 **/
	void createColumnLayouts();

	/**
	 * Update all actions for a context menu for logical section
	 * 'section'.
	 **/
	void updateActions( int section );

	/**
	 * Create a submenu for the currently hidden columns.
	 **/
	QMenu * createHiddenColMenu( QWidget * parent );

	/**
	 * Save the current status in 'layout'.
	 **/
	void saveLayout( ColumnLayout * layout );

	/**
	 * Apply the settings from 'layout'.
	 **/
	void applyLayout( ColumnLayout * layout );

	/**
	 * Ensure consistency of a layout.
	 **/
	void fixupLayout( ColumnLayout * layout );

	/**
	 * Show the columns that are in 'columns'.
	 **/
	void setColumnVisibility( const DataColumnList & columns );

	/**
	 * Return the column name for the specified logical section number.
	 **/
	QString colName( int section ) const;

	/**
	 * Return 'true' if logical section no. 'section' has auto resize mode.
	 **/
	bool autoSizeCol( int section ) const;

	/**
	 * Add any columns that are missing from the default columns to
	 * 'colList'.
	 **/
	void addMissingColumns( DataColumnList & colList );

	/**
	 * Return the resize mode for the specified section.
	 **/
	QHeaderView::ResizeMode resizeMode( int section ) const;

	/**
	 * Set the resize mode for the specified section.
	 **/
	void setResizeMode( int section, QHeaderView::ResizeMode resizeMode );


	//
	// Data members
	//

	DirTreeView		      * _treeView;
	QHeaderView		      * _header;
	QAction			      * _actionAllColumnsAutoSize;
	QAction			      * _actionAllColumnsInteractiveSize;
	QAction			      * _actionAutoSizeCurrentCol;
	QAction			      * _actionHideCurrentCol;
	QAction			      * _actionShowAllHiddenColumns;
	QAction			      * _actionResetToDefaults;
	int				_currentSection;
	QMap<QString, ColumnLayout *>	_layouts;
	ColumnLayout *			_currentLayout;

    };	// class HeaderTweaker


    /**
     * Helper class to store information about different column layouts.
     **/
    class ColumnLayout
    {
    public:
	ColumnLayout( const QString name ):
	    name( name )
	    {}

	QString	       name;
	DataColumnList columns;
	DataColumnList defaultColumns;

    };	// class ColumnLayout

}	// namespace QDirStat

#endif	// HeaderTweaker_h
