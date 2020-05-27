/*
 *   File name: DirTreeFilter.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirTreeFilter_h
#define DirTreeFilter_h

#include <QString>


namespace QDirStat
{
    /**
     * Abstract base class to check if a filesystem object should be ignored
     * during directory reading.
     **/
    class DirTreeFilter
    {
    public:

	/**
	 * Constructor.
	 **/
	DirTreeFilter()
	    {}

	/**
	 * Destructor.
	 **/
	virtual ~DirTreeFilter()
	    {}

	/**
	 * Return 'true' if the filesystem object specified by 'path' should
	 * be ignored, 'false' if not.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual bool ignore( const QString & path ) const = 0;

    };	// class DirTreeFilter

}	// namespace QDirStat

#endif	// DirTreeFilter_h
