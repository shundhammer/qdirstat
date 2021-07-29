/*
 *   File name: HistoryButtons.h
 *   Summary:	History buttons handling for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef HistoryButtons_h
#define HistoryButtons_h

#include <QObject>
#include "History.h"

class QMenu;
class QAction;


namespace QDirStat
{
    class FileInfo;


    class HistoryButtons: public QObject
    {
	Q_OBJECT

    public:

        /**
         * Constructor.
         **/
	HistoryButtons( QAction * actionGoBack,
			QAction * actionGoForward );

        /**
         * Destructor.
         **/
	virtual ~HistoryButtons();

        /**
         * Clear the complete history.
         **/
        void clearHistory() { _history->clear(); }

        /**
         * Access to the history manager.
         **/
	History * history() { return _history; }


    public slots:

	/**
	 * Handle the browser-like "Go Back" button (action):
	 * Move one entry back in the history of visited directories.
	 **/
	void historyGoBack();

	/**
	 * Handle the browser-like "Go Forward" button (action):
	 * Move one entry back in the history of visited directories.
	 **/
	void historyGoForward();

	/**
	 * Enable or disable the browser-like "Go Back" and "Go Forward"
	 * actions.
	 **/
	void updateActions();

	/**
	 * Add a FileInfo item to the history if it's a directory and its URL
	 * is not the same as the current history item.
	 **/
	void addToHistory( FileInfo * item );


    signals:

	/**
	 * Emitted when a history item was activated to navigate to the
	 * specified URL.
	 **/
	void navigateToUrl( const QString & url );


    protected slots:

        /**
         * Clear the old history menu and add all current history items to it.
         **/
        void updateHistoryMenu();

	/**
	 * The user activated an action from the history menu; fetch the history
	 * item index from that action and navigate to that history item.
	 **/
	void historyMenuAction( QAction * action );


    protected:

	/**
	 * Initialize the history buttons: Change the tool buttons to handle a
	 * menu upon long click.
	 **/
	void initHistoryButtons();


	//
	// Data members
	//

	History * _history;
        QAction * _actionGoBack;
        QAction * _actionGoForward;
	QMenu	* _historyMenu;

    };
}	// namespace QDirStat

#endif	// HistoryButtons_h
