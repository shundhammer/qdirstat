/*
 *   File name: FileTypeStats.h
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileTypeStats_h
#define FileTypeStats_h

#include <QObject>
#include <QMap>

#include "ui_file-type-stats-window.h"
#include "DirInfo.h"

#define NO_SUFFIX "//<No Suffix>" // A slash is illegal in Linux/Unix filenames


namespace QDirStat
{
    class DirTree;
    class MimeCategorizer;
    class MimeCategory;

    typedef QMap<QString, FileSize>		StringFileSizeMap;
    typedef QMap<QString, int>			StringIntMap;
    typedef QMap<MimeCategory *, FileSize>	CategoryFileSizeMap;
    typedef QMap<MimeCategory *, int>		CategoryIntMap;

    typedef StringFileSizeMap::const_iterator	StringFileSizeMapIterator;
    typedef CategoryFileSizeMap::const_iterator CategoryFileSizeMapIterator;


    /**
     * Class to calculate file type statistics for a subtree, such as how much
     * disk space is used for each kind of filename extension (*.jpg, *.mp4
     * etc.).
     **/
    class FileTypeStats: public QObject
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	FileTypeStats( QObject * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~FileTypeStats();

    public slots:

        /**
         * Calculate the statistics from a new subtree.
         **/
	void calc( FileInfo * subtree );

	/**
	 * Clear all data.
	 **/
	void clear();

    signals:

	/**
	 * Emitted when the calculation is finished. This is useful when the
	 * calc() slot is connected to some outside signal to recalculate the
	 * contents. calc() itself is synchronous, i.e. it only returns when
	 * the calculation is finished.
	 **/
	void calcFinished() const;

    public:

	/**
	 * Return the number of files in the tree with the specified suffix.
	 **/
	int suffixCount( const QString & suffix ) const;

	/**
	 * Return the total file size of files in the tree with the specified
	 * suffix.
	 **/
	FileSize suffixSum( const QString & suffix ) const;

	/**
	 * Return the number of files in the tree with the specified category.
	 **/
	int categoryCount( MimeCategory * category ) const;

	/**
	 * Return the total file size of files in the tree with the specified
	 * category.
	 **/
	FileSize categorySum( MimeCategory * category ) const;

	/**
	 * Return the category for the specified suffix or 0 if there is none.
	 **/
	MimeCategory * category( const QString & suffix ) const;

	/**
	 * Return the special category for "other", i.e. unclassified files.
	 **/
	MimeCategory * otherCategory() const { return _otherCategory; }

	/**
	 * Return the total size of the tree.
	 **/
	FileSize totalSize() const { return _totalSize; }

	/**
	 * Return the percentage of 'size' of the tree total size.
	 **/
	double percentage( FileSize size ) const;

	//
	// Iterators
	//

	StringFileSizeMapIterator suffixSumBegin() const
	    { return _suffixSum.constBegin(); }

	StringFileSizeMapIterator suffixSumEnd() const
	    { return _suffixSum.constEnd(); }

	CategoryFileSizeMapIterator categorySumBegin() const
	    { return _categorySum.constBegin(); }

	CategoryFileSizeMapIterator categorySumEnd() const
	    { return _categorySum.constEnd(); }

    protected:

	/**
	 * Collect information from the associated widget tree:
	 *
	 * Recursively go through the tree and collect sizes for each file type
	 * (filename extension).
	 **/
	void collect( FileInfo * dir );

	/**
	 * Remove useless content from the maps. On a Linux system, there tend
	 * to be a lot of files that have a '.' in the name, but it's not a
	 * meaningful suffix but a general-purpose separator for dates, SHAs,
	 * version numbers or whatever. All that stuff accumulates in the maps,
	 * and it's typically just a single file with that non-suffix. This
	 * function tries a best effort to get rid of that stuff.
	 **/
	void removeCruft();

	/**
	 * Remove empty suffix entries from the internal maps.
	 **/
	void removeEmpty();

	/**
	 * Check if a suffix is cruft, i.e. a nonstandard suffix that is not
	 * useful for display.
	 *
	 * Notice that this is a highly heuristical algorithm that might give
	 * false positives.
	 **/
	bool isCruft( const QString & suffix ) const;

	/**
	 * Check if the sums add up and how much is unaccounted for.
	 **/
	void sanityCheck();


	//
	// Data members
	//

	MimeCategory *		_otherCategory;
	MimeCategorizer *	_mimeCategorizer;

	StringFileSizeMap	_suffixSum;
	StringIntMap		_suffixCount;
	CategoryFileSizeMap	_categorySum;
	CategoryIntMap		_categoryCount;

        FileSize                _totalSize;
    };
}


#endif // FileTypeStats_h
