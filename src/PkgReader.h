/*
 *   File name: PkgReader.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef PkgReader_h
#define PkgReader_h

#include <QMap>

#include "DirReadJob.h"
#include "PkgInfo.h"


namespace QDirStat
{
    // Forward declarations
    class DirTree;


    /**
     * A class for reading information about installed packages.
     *
     * This uses PkgQuery and PkgManager to read first the installed packages
     * and then one by one the file list for each of those packages in a read
     * job very much like DirReadJob.
     **/
    class PkgReader
    {
    public:

	/**
	 * Constructor. Even though this object creates a PkgReadJob for each
	 * package it finds, it is not necessary to keep this reader around
	 * longer than after read() is finished: Once created and queued, the
	 * PkgReadJobs are self-sufficient. They don't need this reader.
	 **/
	PkgReader( DirTree * tree );

	/**
	 * Destructor.
	 **/
	~PkgReader();

	/**
	 * Read all installed packages from the system's package manager(s) and
	 * create a PkgReadJob for each one to read its file list. Like all
	 * read jobs, this is done with a zero duration timer in the Qt event
	 * loop, so whenever there is no user or X11 event to process, it will
	 * pick one read job and execute it.
	 **/
	void read();

    protected:

	/**
	 * Handle packages that are installed in multiple versions or for
	 * multiple architectures: Assign a different display name to each of
	 * them.
	 **/
	void handleMultiPkg();

	/**
	 * Create a suitable display names for a package: Packages that are
	 * only installed in one version or for one architecture will simply
	 * keep their base name; others will have the version and/or the
	 * architecture appended so the user can tell them apart.
	 **/
	void createDisplayName( const QString & pkgName );

	/**
	 * Add the packages to the DirTree.
	 **/
	void addPkgToTree();

        /**
         * Create a read job for each package to read its file list and add it
         * to the read job queue.
         **/
        void createReadJobs();


	// Data members

	DirTree *                       _tree;
	PkgInfoList			_pkgList;
	QMultiMap<QString, PkgInfo *>	_multiPkg;

    };	// class PkgReader


    /**
     * A read job class for reading information about a package.
     **/
    class PkgReadJob: public DirReadJob
    {
    public:

	/**
	 * Constructor: Prepare to read the file list of existing PkgInfo node
	 * 'pkg' and create a DirInfo or FileInfo node for each item in the
	 * file list below 'pkg'.
	 *
	 * Create the job and add it to a DirReadJobQueue so it is picked up
	 * when possible. Reading is then started from the outside with
	 * startReading().
	 **/
	PkgReadJob( DirTree * tree, PkgInfo * pkg );

	/**
	 * Destructor.
	 **/
	virtual ~PkgReadJob();

	/**
	 * Start reading the file list of the package.
	 *
	 * Reimplemented from DirReadJob.
	 **/
	virtual void startReading() Q_DECL_OVERRIDE;

	/**
	 * Return the parent PkgInfo node.
	 **/
	PkgInfo * pkg() const { return _pkg; }

    protected:

	PkgInfo * _pkg;

    };	// class PkgReadJob

}	// namespace QDirStat

#endif // ifndef PkgReader_h
