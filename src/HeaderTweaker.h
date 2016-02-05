/*
 *   File name: HeaderTweaker.h
 *   Summary:	Helper class for DirTreeView
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef HeaderTweaker_h
#define HeaderTweaker_h


#include <QObject>

class QHeaderView;
class QAction;


namespace QDirStat
{
    class DirTreeView;

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

    protected slots:

	/**
	 * Post a context menu for the header at 'pos'.
	 **/
	void contextMenu( const QPoint & pos );

	/**
	 * Set auto size mode for all columns on or off.
	 **/
	void setAllColumnsAutoSize( bool autoSize );

	/**
	 * Return 'true' if auto size mode for all columns is on.
	 **/
	bool allColumnsAutoSize() const;

	/**
	 * Initialize the header view.
	 **/
	void initHeader() const;


    protected:

	/**
	 * Create internally used actions and connect them to the appropriate
	 * slots.
	 **/
	void createActions();


	//
	// Data members
	//

	DirTreeView	  * _treeView;
	QHeaderView	  * _header;
	bool		    _allColumnsAutoSize;
	QAction		  * _actionAllColumnsAutoSize;

    };	// class HeaderTweaker

}	// namespace QDirStat

#endif	// HeaderTweaker_h
