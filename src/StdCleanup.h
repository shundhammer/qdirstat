/*
 *   File name: StdCleanup.h
 *   Summary:	QDirStat classes to reclaim disk space
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef StdCleanup_h
#define StdCleanup_h

#include <QObject>
#include <QList>

#define USE_DEBUG_ACTIONS 0


namespace QDirStat
{
    class Cleanup;

    /**
     * Create any of the predefined standard Cleanup actions to be
     * performed on DirTree items. Ownership of the objects is passed to the
     * caller.
     *
     * This class is not meant to be ever instantiated - use the static methods
     * only.
     **/

    class StdCleanup
    {
    public:

	static Cleanup * openFileManagerHere( QObject * parent = 0 );
	static Cleanup * openTerminalHere   ( QObject * parent = 0 );
	static Cleanup * checkFileType      ( QObject * parent = 0 );
	static Cleanup * compressSubtree    ( QObject * parent = 0 );
	static Cleanup * makeClean	    ( QObject * parent = 0 );
	static Cleanup * gitClean	    ( QObject * parent = 0 );
	static Cleanup * deleteJunk	    ( QObject * parent = 0 );
	static Cleanup * hardDelete	    ( QObject * parent = 0 );
	static Cleanup * clearDirContents   ( QObject * parent = 0 );
#if USE_DEBUG_ACTIONS
	static Cleanup * echoargs	    ( QObject * parent = 0 );
	static Cleanup * echoargsMixed	    ( QObject * parent = 0 );
	static Cleanup * segfaulter	    ( QObject * parent = 0 );
	static Cleanup * commandNotFound    ( QObject * parent = 0 );
	static Cleanup * sleepy		    ( QObject * parent = 0 );
#endif

	/**
	 * Create all of the above and return them in a list.
	 **/
	static CleanupList stdCleanups( QObject * parent );

    private:
	/**
	 * Prevent instances of this class - private constructor / destructor.
	 **/
	StdCleanup()	{}
	~StdCleanup()	{}
    };

}	// namespace QDirStat


#endif // ifndef StdCleanup_h

