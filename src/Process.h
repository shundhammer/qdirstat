/*
 *   File name: Process.h
 *   Summary:	Qt4/5 compatibility layer for QProcess
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Authors:	 Michael Matz <matz@suse.de>
 *		 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef Process_h
#define Process_h

#include <QProcess>


namespace QDirStat
{
    /**
     * Thin compatibility layer around QProcess to maintain backwards
     * compatibility to Qt4: Enable creating a process with program to start
     * and arguments supplied in the constructor and store them for later use
     * with a plain start() without any more arguments.
     **/
    class Process: public QProcess
    {
	Q_OBJECT

    public:
	explicit Process( QObject *parent = 0 );

#if (QT_VERSION < QT_VERSION_CHECK( 5, 1, 0))

	void start();

	const QStringList & arguments() { return _arglist; }
	const QString	  & program()	{ return _prog; }

	void setProgram( const QString & prog )
	    { _prog = prog; }

	void setArguments( const QStringList & args )
	    { _arglist = args; }

    private:

	QString	    _prog;
	QStringList _arglist;

#endif	// Qt < 5.1.0

    };	// class Process
}	// namespace QDirStat

#endif	// Process_h
