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

    // Ownership of the PkgInfo * items in _pkgList was transferred to the
    // tree, so intentionally NOT calling qDeleteItems( _pkgList ) !

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

#if 0
        // FIXME: DEBUG
        // FIXME: DEBUG
        // FIXME: DEBUG
        static int debugCount = 0;

        if ( ++debugCount > 10 )
            return;

        // FIXME: DEBUG
        // FIXME: DEBUG
        // FIXME: DEBUG
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

            try
            {
                newParent = LocalDirReadJob::stat( path, _tree, parent );
                // logDebug() << "Created " << newParent << endl;
            }
            catch( const SysCallFailedException & exception )
            {
                CAUGHT( exception );
                logWarning() << "Can't check files in file list of " << _pkg << endl;

                if ( parent )
                    parent->setReadState( DirError );

                // Don't continue with this line in the file list - it's
                // pointless.
                //
                // The user may not have sufficient permissions to lstat() the
                // file itself or one of the intermediate directories, or the
                // file of one of the intermediate directory may be missing
                // (manually removed?).
                return;
            }

            CHECK_PTR( newParent );

#if 0
            if ( remaining.isEmpty() )
                logDebug() << "Created leaf item " << newParent << endl;
            else
                logDebug() << "  Created missing intermediate item " << newParent << endl;
#endif
        }

        if ( ! remaining.isEmpty() )
        {
            parent = newParent->toDirInfo();
            CHECK_PTR( parent );
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

