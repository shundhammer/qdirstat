/*
 *   File name: DirTreePkgFilter.h
 *   Summary:	Package manager support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirTreePkgFilter_h
#define DirTreePkgFilter_h

#include "DirTreeFilter.h"


namespace QDirStat
{
    class PkgManager;
    class PkgFileListCache;

    /**
     * Concrete DirTreeFilter class to ignore files that belong to any
     * installed package during directory reading.
     **/
    class DirTreePkgFilter: public DirTreeFilter
    {
    public:

	/**
	 * Constructor.
	 **/
	DirTreePkgFilter( PkgManager * pkgManager );

	/**
	 * Destructor.
	 **/
	virtual ~DirTreePkgFilter();

	/**
	 * Return 'true' if the filesystem object specified by 'path' should
	 * be ignored, 'false' if not.
	 *
	 * Implemented from DirTreeFilter.
	 **/
	virtual bool ignore( const QString & path ) const Q_DECL_OVERRIDE;


    protected:

	PkgFileListCache * _fileListCache;

    };	// class DirTreeFilter

}	// namespace QDirStat

#endif	// DirTreePkgFilter_h
