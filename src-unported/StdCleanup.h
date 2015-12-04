/*
 *   File name:	kstdcleanup.h
 *   Summary:	Support classes for QDirStat
 *   License:   GPL V2 - See file LICENSE for details.
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 *
 */


#ifndef StdCleanup_h
#define StdCleanup_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

// Forward declarations
class KActionCollection;


namespace QDirStat
{
    /**
     * Predefined standard @ref Cleanup actions to be performed on
     * @ref DirTree items.
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
    
    class StdCleanup
    {
    public:
	static Cleanup *openInKonqueror	( KActionCollection *parent = 0 );
	static Cleanup *openInTerminal		( KActionCollection *parent = 0 );
	static Cleanup *compressSubtree	( KActionCollection *parent = 0 );
	static Cleanup *makeClean		( KActionCollection *parent = 0 );
	static Cleanup *deleteTrash		( KActionCollection *parent = 0 );
	static Cleanup *moveToTrashBin		( KActionCollection *parent = 0 );
	static Cleanup *hardDelete		( KActionCollection *parent = 0 );
	
    private:
	/**
	 * Prevent instances of this class - private constructor / destructor.
	 **/
	StdCleanup()	{}
	~StdCleanup()	{}
    };

}	// namespace QDirStat


#endif // ifndef StdCleanup_h


// EOF
