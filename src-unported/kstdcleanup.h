/*
 *   File name:	kstdcleanup.h
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2008-11-27
 */


#ifndef KStdCleanup_h
#define KStdCleanup_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

// Forward declarations
class KActionCollection;


namespace KDirStat
{
    /**
     * Predefined standard @ref KCleanup actions to be performed on
     * @ref KDirTree items.
     *
     * This class is not meant to be ever instantiated - use the static methods
     * only.
     * 
     * For details about what each individual method does, refer to the help
     * file. Use the old (KDirStat 0.86) help file in case the current help
     * file isn't available yet.
     *
     * @short KDirStat standard cleanup actions
     **/
    
    class KStdCleanup
    {
    public:
	static KCleanup *openInKonqueror	( KActionCollection *parent = 0 );
	static KCleanup *openInTerminal		( KActionCollection *parent = 0 );
	static KCleanup *compressSubtree	( KActionCollection *parent = 0 );
	static KCleanup *makeClean		( KActionCollection *parent = 0 );
	static KCleanup *deleteTrash		( KActionCollection *parent = 0 );
	static KCleanup *moveToTrashBin		( KActionCollection *parent = 0 );
	static KCleanup *hardDelete		( KActionCollection *parent = 0 );
	
    private:
	/**
	 * Prevent instances of this class - private constructor / destructor.
	 **/
	KStdCleanup()	{}
	~KStdCleanup()	{}
    };

}	// namespace KDirStat


#endif // ifndef KStdCleanup_h


// EOF
