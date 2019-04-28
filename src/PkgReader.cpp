/*
 *   File name: PkgReader.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "PkgReader.h"
#include "PkgQuery.h"
#include "DirTree.h"
#include "DirReadJob.h"         // LocalDirReadJob
#include "Logger.h"
#include "Exception.h"

#define DEBUG_LIMIT_READ_JOBS   0


using namespace QDirStat;


PkgReader::PkgReader( DirTree * tree ):
    _tree( tree )
{
    logInfo() << endl;
}


PkgReader::~PkgReader()
{
    // Intentionally NOT deleting the PkgInfo * items of _pkgList:
    // They are now owned by the DirTree.
}


void PkgReader::read( const PkgFilter & filter )
{
    logInfo() << endl;

    CHECK_PTR( _tree );
    _tree->clear();

    _pkgList = PkgQuery::installedPkg();
    filterPkgList( filter );
    handleMultiPkg();
    addPkgToTree();
    createReadJobs();

    // Ownership of the PkgInfo * items in _pkgList was transferred to the
    // tree, so intentionally NOT calling qDeleteItems( _pkgList ) !

    _pkgList.clear();
    _multiPkg.clear();
}


void PkgReader::filterPkgList( const PkgFilter & filter )
{
    if ( filter.filterMode() == PkgFilter::SelectAll )
        return;

    PkgInfoList matches;

    foreach ( PkgInfo * pkg, _pkgList )
    {
        if ( filter.matches( pkg->baseName() ) )
        {
            // logDebug() << "Selecting pkg " << pkg << endl;
            matches << pkg;
        }
    }

    _pkgList = matches;
}


void PkgReader::handleMultiPkg()
{
    _multiPkg.clear();

    foreach ( PkgInfo * pkg, _pkgList )
    {
        _multiPkg.insert( pkg->baseName(), pkg );
    }

    foreach ( const QString & pkgName, _multiPkg.uniqueKeys() )
    {
        createDisplayName( pkgName );
    }
}


void PkgReader::createDisplayName( const QString & pkgName )
{
    PkgInfoList pkgList = _multiPkg.values( pkgName );

    if ( pkgList.size() < 2 )
        return;

    QString version  = pkgList.first()->version();
    QString arch     = pkgList.first()->arch();

    bool sameVersion = true;
    bool sameArch    = true;

    for ( int i = 1; i < pkgList.size(); ++i )
    {
        if ( pkgList.at( i )->version() != version )
            sameVersion = false;

        if ( pkgList.at( i )->arch() != arch )
            sameArch = false;
    }

    if ( ! sameVersion )
    {
        logDebug() << "Found multi version pkg " << pkgName
                   << " same arch: " << sameArch
                   << endl;
    }

    foreach ( PkgInfo * pkg, pkgList )
    {
        QString name = pkgName;

        if ( ! sameVersion )
        {
            name += "-" + pkg->version();
            pkg->setMultiVersion( true );
        }

        if ( ! sameArch )
        {
            name += ":" + pkg->arch();
            pkg->setMultiArch( true );
        }

        // logDebug() << " Setting name " << name << endl;
        pkg->setName( name );
    }
}


void PkgReader::addPkgToTree()
{
    CHECK_PTR( _tree );
    CHECK_PTR( _tree->root() );

    PkgInfo * top = new PkgInfo( _tree, _tree->root(), "Pkg:" );
    CHECK_NEW( top );
    _tree->root()->insertChild( top );

    foreach ( PkgInfo * pkg, _pkgList )
    {
        pkg->setTree( _tree );
        top->insertChild( pkg );
    }

    top->setReadState( DirFinished );
    _tree->sendFinalizeLocal( top );
    top->finalizeLocal();
}


void PkgReader::createReadJobs()
{
    foreach ( PkgInfo * pkg, _pkgList )
    {
        PkgReadJob * job = new PkgReadJob( _tree, pkg );
        CHECK_NEW( job );
        _tree->addJob( job );

#if DEBUG_LIMIT_READ_JOBS
        static int debugCount = 0;

        if ( ++debugCount > 100 )
            return;
#endif
    }
}






PkgReadJob::PkgReadJob( DirTree * tree, PkgInfo * pkg ):
    DirReadJob( tree, pkg ),
    _pkg( pkg )
{

}


PkgReadJob::~PkgReadJob()
{

}


void PkgReadJob::startReading()
{
    // logInfo() << "Reading " << _pkg << endl;
    CHECK_PTR( _pkg );

    _pkg->setReadState( DirReading );

    foreach ( const QString & path, PkgQuery::fileList( _pkg ) )
    {
        if ( ! path.isEmpty() )
            addFile( path );
    }

    finalizeAll( _pkg );
    _tree->sendReadJobFinished( _pkg );
    finished();
    // Don't add anything after finished() since this deletes this job!
}


void PkgReadJob::addFile( const QString & fileListPath )
{
    // logDebug() << "Adding " << fileListPath << " to " << _pkg << endl;

    QStringList remaining = fileListPath.split( "/", QString::SkipEmptyParts );
    QStringList currentPath;
    DirInfo *   parent = _pkg;

    while ( ! remaining.isEmpty() )
    {
        QString currentName = remaining.takeFirst();
        currentPath << currentName;
        FileInfo * newParent = _pkg->locate( parent, QStringList() << currentName );

        if ( ! newParent )
        {
            QString path = QString( "/" ) + currentPath.join( "/" );

            newParent = LocalDirReadJob::stat( path, _tree, parent,
                                               false ); // doThrow

            if ( ! newParent )
            {
                parent->setReadState( DirError );
                return;
            }

            // logDebug() << "Created " << newParent << endl;
        }

        if ( ! remaining.isEmpty() )
        {
            parent = newParent->toDirInfo();

            if ( ! parent )
            {
                logWarning() << newParent << " should be a directory, but is not" << endl;
                return;
            }
        }
    }
}


void PkgReadJob::finalizeAll( DirInfo * subtree )
{
    FileInfo * child = subtree->firstChild();

    while( child )
    {
        if ( child->isDirInfo() )
            finalizeAll( child->toDirInfo() );

        child = child->next();
    }

    if ( subtree->readState() != DirError )
        subtree->setReadState( DirFinished );

    _tree->sendFinalizeLocal( subtree );
    subtree->finalizeLocal();
}

