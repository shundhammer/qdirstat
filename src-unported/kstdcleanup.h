/*
 *   File name:	kstdcleanup.h
 *   Summary:	Support classes for QDirStat
 *   License:   GPL V2 - See file LICENSE for details.
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 *
 */


#ifndef KStdCleanup_h
#define KStdCleanup_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

// Forward declarations
class KActionCollection;


namespace QDirStat
{
    /**
     * Predefined standard @ref KCleanup actions to be performed on
     * @ref KDirTree items.
     *
     * This class is not meant to be ever instantiated - use the static methods
     * only.
     * 
     * For details about what each individual method does, refer to the help
     * file. Use the old (QDirStat 0.86) help file in case the current help
     * file isn't available yet.
     *
     * @short QDirStat standard cleanup actions
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

}	// namespace QDirStat


#endif // ifndef KStdCleanup_h


// EOF
