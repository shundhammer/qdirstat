/*
 *   File name: BusyPopup.h
 *   Summary:	QDirStat generic widget classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef BusyPopup_h
#define BusyPopup_h

#include <QLabel>
#include <QString>


namespace QDirStat
{
    /**
     * Simple popup dialog to show a message just prior to a lengthy operation.
     * The popup does some event handling to make sure that it really appears
     * before that lengthy operation starts: It processes events for some
     * milliseconds so its own show, resize, move and paint events are
     * processed.
     *
     * Usage:
     *
     *	   BusyPopup msg( "Calculating...", mainWin );
     *	   calc();
     *
     * In the normal case, just let the variable go out of scope, and the popup
     * is destroyed and thus closed. Of course you can also create it with
     * 'new' and destroy it with 'delete' or simply call 'hide()'.
     **/
    class BusyPopup: public QLabel
    {
	Q_OBJECT

    public:

	/**
	 * Create a BusyPopup with the specified text.
	 *
	 * If 'autoPost' is 'true', automatically post it, i.e. show it and
	 * process events for some milliseconds to makes sure it is rendered.
	 **/
	BusyPopup( const QString & text,
		   QWidget *	   parent   = 0,
		   bool		   autoPost = true );

	/**
	 * Destructor.
	 **/
	virtual ~BusyPopup();

	/**
	 * Show the BusyPopup and process events for some milliseconds to make
	 * sure it is rendered. This is done automatically if 'autoPost' is
	 * 'true' in the constructor.
	 **/
	void post();

	/**
	 * Process events (except user input events) for the specified time.
	 **/
	void processEvents( int millisec );

	/**
	 * Process a show event.
	 *
	 * Reimplemented from QLabel / QWidget.
	 **/
	virtual void showEvent( QShowEvent * event ) Q_DECL_OVERRIDE;


    protected:

	bool _posted;

    };	// BusyPopup

}	// namespace QDirStat


#endif	// BusyPopup_h
