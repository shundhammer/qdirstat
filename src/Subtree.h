/*
 *   File name: Subtree.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef Subtree_h
#define Subtree_h


#include "FileInfo.h"
#include "DirTree.h"


namespace QDirStat
{
    /**
     * Class to store information about a subtree of a DirTree.
     *
     * This is basically a wrapper around a FileInfo pointer that takes the
     * very limited life time of such a pointer into account: Whenever a part
     * of the DirTree is refreshed (e.g. after cleanup actions), all pointers
     * in that subtree become invalid. While the DirTree does send signals when
     * that happens, in many cases it is overkill to connect to those signals
     * and monitor all the time for the off-chance that the one FileInfo
     * pointer we keep is affected.
     *
     * This class simply stores the URL of the subtree and locates the FileInfo
     * item in the tree when needed. In addition to that, it can also fall back
     * to the tree's root if that URL no longer exists in the tree.
     *
     * Not using Qt's signals and slots has the added benefit of not needing to
     * inherit QObject which means that instances of this class do not need to
     * be created on the heap with 'new', and they can be aggregated in other
     * classes.
     **/
    class Subtree
    {
    public:
        /**
         * Constructor.
         **/
        Subtree( DirTree * tree = 0 ):
            _tree( tree ),
            _useRootFallback( true )
            {}

        /**
         * Return the DirTree.
         **/
        DirTree * tree() const { return _tree; }

        /**
         * Return the URL.
         **/
        QString url() const;

        /**
         * Return 'true' if the tree's root item should be used as a fallback
         * if no URL is set or if no item with that URL can be located. The
         * default is 'true'.
         **/
        bool useRootFallback() const { return _useRootFallback; }

        /**
         * Enable or disable using the tree's root as a fallback.
         **/
        void setUseRootFallback( bool val ) { _useRootFallback = val; }

        /**
         * Get the corresponding subtree item from the DirTree via the URL.
         *
         * If a URL was set before (typically by setting the subtree), this
         * traverses the DirTree to find the item with that URL. This is an
         * expensive operation.
         *
         * If no URL was set or if no item with that URL could be found and the
         * 'useRootFallback' flag is set, the root item of the DirTree is used.
         *
         * This might return 0 if no tree was set (also typically by setting
         * the subtree) or if using the root as a fallback is disabled and the
         * URL could not be found in the DirTree.
         **/
        FileInfo * subtree();

        /**
         * Dereference operator. This is an alias for subtree(): Get the
         * subtree via the URL.
         **/
        FileInfo * operator()() { return subtree(); }

        /**
         * Set the subtree. This also sets the tree and the URL which both can
         * be obtained from 'subtree'. This pointer is not stored internally,
         * just the URL and the tree.
         *
         * Setting the subtree to 0 clears the URL, but not the tree. That
         * means if using the root as a fallback is enabled the next call to
         * subtree() will return the tree's root.
         **/
        void set( FileInfo * subtree );

        /**
         * Clear the subtree (but keep the tree).
         **/
        void clear() { set( 0 ); }

        /**
         * Return 'true' if this subtree is empty, i.e. if it was cleared or if
         * no FileInfo was ever set.
         **/
        bool isEmpty() { return _url.isEmpty(); }

        /**
         * Assignment Operator for a FileInfo pointer. This is an alias for
         * set().
         **/
        Subtree & operator=( FileInfo * subtree )
            { set( subtree ); return *this; }

        /**
         * Normal assignment operator.
         **/
        Subtree & operator=( const Subtree & other )
            { clone( other ); return *this; }

        /**
         * Copy constructor.
         **/
        Subtree( const Subtree & other ) { clone( other ); }

        /**
         * Set the DirTree.
         *
         * This is typically not necessary; it is implicitly done in set().
         **/
        void setTree( DirTree * tree ) { _tree = tree; }

        /**
         * Set the URL.
         *
         * This is typically not necessary; it is implicitly done in set().
         **/
        void setUrl( const QString & newUrl ) { _url = newUrl; }

    protected:

        /**
         * Locate the FileInfo item with the stored URL in the stored tree.
         **/
        FileInfo * locate();

        /**
         * Clone subtree 'other' to this one. This is what both the assignment
         * operator (for Subtrees) and the copy constructor do internally.
         **/
        void clone( const Subtree & other );


        //
        // Data members
        //

        DirTree * _tree;
        QString   _url;

        bool      _useRootFallback;

    };	// class Subtree

}	// namespace QDirStat


#endif // ifndef Subtree_h
