/*
 *   File name: PkgReader.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "PkgReader.h"
#include "PkgManager.h"
#include "DirTree.h"
#include "Logger.h"
#include "Exception.h"


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


void PkgReader::read()
{
    logInfo() << endl;

    CHECK_PTR( _tree );
    _tree->clear();

    _pkgList = PkgQuery::installedPkg();
    handleMultiPkg();
    addPkgToTree();
    createReadJobs();

    // The PkgInfo * items in _pkgList are now owned by the tree,
    // so intentionally NOT calling qDeleteItems( _pkgList )

    _pkgList.clear();
    _multiPkg.clear();
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
            name += "-" + pkg->version();

        if ( ! sameArch )
            name += ":" + pkg->arch();

        // logDebug() << " Setting name " << name << endl;
        pkg->setName( name );
    }
}


void PkgReader::addPkgToTree()
{
    CHECK_PTR( _tree );
    CHECK_PTR( _tree->root() );

    PkgInfo * top = new PkgInfo( _tree, _tree->root(), "Pkg:/" );
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


    _pkg->setReadState( DirFinished );
    _tree->sendFinalizeLocal( _pkg );
    _pkg->finalizeLocal();

    finished();
    // Don't add anything after finished() since this deletes this job!
}
