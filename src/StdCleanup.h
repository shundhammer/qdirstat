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


namespace QDirStat
{
    class Cleanup;

    /**
     * Create any of the predefined standard @ref Cleanup actions to be
     * performed on DirTree items. Ownership of the objects is passed to the
     * caller.
     *
     * This class is not meant to be ever instantiated - use the static methods
     * only.
     **/

    class StdCleanup
    {
    public:

	static Cleanup * openInFileManager( QObject * parent = 0 );
	static Cleanup * openInTerminal	  ( QObject * parent = 0 );
	static Cleanup * compressSubtree  ( QObject * parent = 0 );
	static Cleanup * makeClean	  ( QObject * parent = 0 );
	static Cleanup * deleteJunk	  ( QObject * parent = 0 );
	static Cleanup * moveToTrash	  ( QObject * parent = 0 );
	static Cleanup * hardDelete	  ( QObject * parent = 0 );
#if 1
	// DEBUG
	static Cleanup * echoargs	  ( QObject * parent = 0 );
	static Cleanup * pwd		  ( QObject * parent = 0 );
	// DEBUG
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

