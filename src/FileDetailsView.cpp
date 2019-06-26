/*
 *   File name: FileDetailsView.h
 *   Summary:	Details view for the currently selected file or directory
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "FileDetailsView.h"
#include "AdaptiveTimer.h"
#include "DirInfo.h"
#include "FileInfoSet.h"
#include "MimeCategorizer.h"
#include "PkgQuery.h"
#include "SystemFileChecker.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


FileDetailsView::FileDetailsView( QWidget * parent ):
    QStackedWidget( parent ),
    _ui( new Ui::FileDetailsView ),
    _pkgUpdateTimer( new AdaptiveTimer( this ) ),
    _labelLimit( 40 ),
    _mimeCategorizer( 0 )
{
    CHECK_NEW( _ui );
    CHECK_NEW( _pkgUpdateTimer );

    _ui->setupUi( this );
    clear();

    _labelLimit = 0; // Unlimited
    // TO DO: Read _labelLimit from the config file

    _pkgUpdateTimer->addDelayStage(    0 );
    _pkgUpdateTimer->addDelayStage(  333 ); // millisec
    _pkgUpdateTimer->addDelayStage(  800 ); // millisec
    _pkgUpdateTimer->addDelayStage( 1200 ); // millisec

    _pkgUpdateTimer->addCoolDownPeriod(  500 ); // millisec
    _pkgUpdateTimer->addCoolDownPeriod( 1500 ); // millisec
    _pkgUpdateTimer->addCoolDownPeriod( 3000 ); // millisec

    connect( _pkgUpdateTimer, SIGNAL( deliverRequest( QVariant ) ),
             this,            SLOT  ( updatePkgInfo ( QVariant ) ) );
}


FileDetailsView::~FileDetailsView()
{

}


void FileDetailsView::clear()
{
    setCurrentPage( _ui->emptyPage );
}


void FileDetailsView::setCurrentPage( QWidget *page )
{
    // Simply hiding all other widgets is not enough: The QStackedLayout will
    // still reserve screen space for the largest widget. The other pages
    // really need to be removed from the layout. They are still children of
    // the QStackedWidget, but no longer in the layout.

    while ( count() > 0 )
        removeWidget( widget( 0 ) );

    addWidget( page );
    setCurrentWidget( page );
}


void FileDetailsView::showDetails( const FileInfoSet & selectedItems )
{
    FileInfoSet sel = selectedItems.normalized();

    if ( sel.isEmpty() )
    {
	clear();
    }
    else if ( sel.size() == 1 )
    {
	FileInfo * item = sel.first();

	if ( item->isDirInfo() )
	    showDetails( item->toDirInfo() );
	else
	    showDetails( item );
    }
    else
    {
	showSelectionSummary( sel );
    }
}


void FileDetailsView::showDetails( FileInfo * file )
{
    if ( ! file )
    {
	clear();
	return;
    }

    if ( file->isPkgInfo() )
    {
        showDetails( file->toPkgInfo() );
    }
    else if ( file->isDirInfo() )
    {
	showDetails( file->toDirInfo() );
    }
    else
    {
	// logDebug() << "Showing file details about " << file << endl;

	setCurrentPage( _ui->fileDetailsPage );
	showFileInfo( file );

        PkgInfo * pkg = file->pkgInfoParent();

        if ( pkg )
        {
            // The simple case: We are in package view mode, and this file has
            // a PkgInfo ancestor. So we know that this belongs to a package,
            // also to which one, and that this is inherently a system file
            // since it belongs to a package.

            setSystemFileWarningVisibility( true );
            _ui->filePackageLabel->setEnabled( true );
            _ui->filePackageLabel->setText( pkg->name() );
        }
        else
        {
            // The not-so-simple case: Use heuristics to check if this file is
            // in a system directory and then query the package manager after a
            // timeout what (if any) package this file belongs to.

            showFilePkgInfo( file );
        }
    }
}


void FileDetailsView::showFileInfo( FileInfo * file )
{
    CHECK_PTR( file );

    setLabelLimited(_ui->fileNameLabel, file->baseName() );
    _ui->fileTypeLabel->setText( formatFileSystemObjectType( file->mode() ) );

    QString category = mimeCategory( file );

    _ui->fileMimeCategoryCaption->setEnabled( ! category.isEmpty() );
    _ui->fileMimeCategoryLabel->setEnabled( ! category.isEmpty() );
    _ui->fileMimeCategoryLabel->setText( category );

    _ui->fileSizeLabel->setText( fileSizeText( file ) );
    _ui->fileUserLabel->setText( file->userName() );
    _ui->fileGroupLabel->setText( file->groupName() );
    _ui->filePermissionsLabel->setText( formatPermissions( file->mode() ) );

    _ui->fileMTimeLabel->setText( formatTime( file->mtime() ) );
}


QString FileDetailsView::fileSizeText( FileInfo * file ) const
{
    CHECK_PTR( file );

    // Taken from DirTreeModel::sizeColText()

    QString text;

    if ( file->isFile() && ( file->links() > 1 ) ) // Regular file with multiple links
    {
	if ( file->isSparseFile() )
	{
	    text = tr( "%1 / %2 Links (allocated: %3)" )
		.arg( formatSize( file->byteSize() ) )
		.arg( formatSize( file->links() ) )
		.arg( formatSize( file->allocatedSize() ) );
	}
	else
	{
	    text = tr( "%1 / %2 Links" )
		.arg( formatSize( file->byteSize() ) )
		.arg( file->links() );
	}
    }
    else // No multiple links or no regular file
    {
	if ( file->isSparseFile() )
	{
	    text = tr( "%1 (allocated: %2)" )
		.arg( formatSize( file->byteSize() ) )
		.arg( formatSize( file->allocatedSize() ) );
	}
	else
	{
	    text = formatSize( file->size() );
	}
    }

    return text;
}


void FileDetailsView::showFilePkgInfo( FileInfo * file )
{
    CHECK_PTR( file );

    bool isSystemFile = SystemFileChecker::isSystemFile( file );
    setSystemFileWarningVisibility( isSystemFile );

    if ( PkgQuery::foundSupportedPkgManager() )
    {
        setFilePkgBlockVisibility( isSystemFile );

            if ( isSystemFile )
            {
                QString delayHint = QString( _pkgUpdateTimer->delayStage(), '.' );
                _ui->filePackageLabel->setText( delayHint );

                _ui->filePackageCaption->setEnabled( true );
                _pkgUpdateTimer->delayedRequest( file->url() );
            }
    }
    else // No supported package manager found
    {
        setFilePkgBlockVisibility( false );
    }
}


void FileDetailsView::updatePkgInfo( const QVariant & pathVariant )
{
    QString path = pathVariant.toString();
    // logDebug() << "Updating pkg info for " << path << endl;

    QString pkg = PkgQuery::owningPkg( path );
    _ui->filePackageLabel->setText( pkg );
    _ui->filePackageCaption->setEnabled( ! pkg.isEmpty() );
}


void FileDetailsView::setSystemFileWarningVisibility( bool visible )
{
    _ui->fileSystemFileWarning->setVisible( visible );
    _ui->fileSpacerCaption2->setVisible( visible );
}

void FileDetailsView::setFilePkgBlockVisibility( bool visible )
{
    _ui->filePackageCaption->setVisible( visible );
    _ui->filePackageLabel->setVisible( visible );
}


void FileDetailsView::showDetails( DirInfo * dir )
{
    // logDebug() << "Showing dir details about " << dir << endl;

    if ( ! dir )
    {
	clear();
	return;
    }

    setCurrentPage( _ui->dirDetailsPage );

    QString name = dir->isPseudoDir() ? dir->name() : ( dir->baseName() + "/" );
    QString dirType = dir->isPseudoDir() ? tr( "Pseudo Directory" ) : tr( "Directory" );

    setLabelLimited(_ui->dirNameLabel, name );
    _ui->dirTypeLabel->setText( dirType );

    // Subtree information

    showSubtreeInfo( dir );
    showDirNodeInfo( dir );
}


void FileDetailsView::showSubtreeInfo( DirInfo * dir )
{
    CHECK_PTR( dir );

    if ( ! dir->isBusy() &&
         ( dir->readState() == DirFinished ||
           dir->readState() == DirCached     ) )
    {
        setLabel( _ui->dirTotalSizeLabel,   dir->totalSize()    );
        setLabel( _ui->dirItemCountLabel,   dir->totalItems()   );
        setLabel( _ui->dirFileCountLabel,   dir->totalFiles()   );
        setLabel( _ui->dirSubDirCountLabel, dir->totalSubDirs() );
    }
    else
    {
        QString msg;

        if ( dir->isBusy() )
        {
            msg = tr( "[Reading]" );
        }
        else
        {
            switch ( dir->readState() )
            {
                case DirOnRequestOnly: msg = tr( "[Not Read]"   ); break;
                case DirAborted:       msg = tr( "[Aborted]"    ); break;
                case DirError:         msg = tr( "[Read Error]" ); break;

                default:
                    break;
            }
        }

        _ui->dirTotalSizeLabel->setText( msg );
        _ui->dirItemCountLabel->clear();
        _ui->dirFileCountLabel->clear();
        _ui->dirSubDirCountLabel->clear();
    }

    _ui->dirLatestMTimeLabel->setText( formatTime( dir->latestMtime() ) );
}


void FileDetailsView::showDirNodeInfo( DirInfo * dir )
{
    CHECK_PTR( dir );
    setDirBlockVisibility( ! dir->isPseudoDir() );

    if ( ! dir->isPseudoDir() )
    {
        _ui->dirOwnSizeCaption->setVisible( dir->size() > 0 );
        _ui->dirOwnSizeLabel->setVisible  ( dir->size() > 0 );
	setLabel( _ui->dirOwnSizeLabel, dir->size() );

	_ui->dirUserLabel->setText( dir->userName() );
	_ui->dirGroupLabel->setText( dir->groupName() );
	_ui->dirPermissionsLabel->setText( formatPermissions( dir->mode() ) );


        _ui->dirMTimeCaption->setVisible( dir->mtime() > 0 );
        _ui->dirMTimeLabel->setVisible  ( dir->mtime() > 0);
	_ui->dirMTimeLabel->setText( formatTime( dir->mtime() ) );
    }
}


void FileDetailsView::setDirBlockVisibility( bool visible )
{
    _ui->dirDirectoryHeading->setVisible( visible );

    _ui->dirOwnSizeCaption->setVisible( visible );
    _ui->dirUserCaption->setVisible( visible );
    _ui->dirGroupCaption->setVisible( visible );
    _ui->dirPermissionsCaption->setVisible( visible );
    _ui->dirMTimeCaption->setVisible( visible );

    _ui->dirOwnSizeLabel->setVisible( visible );
    _ui->dirUserLabel->setVisible( visible );
    _ui->dirGroupLabel->setVisible( visible );
    _ui->dirPermissionsLabel->setVisible( visible );
    _ui->dirMTimeLabel->setVisible( visible );

    // A dot entry cannot have directory children
    _ui->dirSubDirCountCaption->setVisible( visible );
    _ui->dirSubDirCountLabel->setVisible( visible );
}


void FileDetailsView::showDetails( PkgInfo * pkg )
{
    // logDebug() << "Showing pkg details about " << pkg << endl;

    if ( ! pkg )
    {
	clear();
	return;
    }

    if ( pkg->url() == "Pkg:/" )
    {
        showPkgSummary( pkg );
        return;
    }

    setCurrentPage( _ui->pkgDetailsPage );

    setLabelLimited( _ui->pkgNameLabel, pkg->name() );
    _ui->pkgVersionLabel->setText( pkg->version() );
    _ui->pkgArchLabel->setText( pkg->arch() );

    if ( ! pkg->isBusy() && pkg->readState() == DirFinished )
    {
        setLabel( _ui->pkgTotalSizeLabel,   pkg->totalSize()    );
        setLabel( _ui->pkgItemCountLabel,   pkg->totalItems()   );
        setLabel( _ui->pkgFileCountLabel,   pkg->totalFiles()   );
        setLabel( _ui->pkgSubDirCountLabel, pkg->totalSubDirs() );
    }
    else
    {
        QString msg;

        if ( pkg->isBusy() )
        {
            msg = tr( "[Reading]" );
        }
        else
        {
            if ( pkg->readState() == DirError )
                msg = tr( "[Read Error]" );
        }

        _ui->pkgTotalSizeLabel->setText( msg );
        _ui->pkgItemCountLabel->clear();
        _ui->pkgFileCountLabel->clear();
        _ui->pkgSubDirCountLabel->clear();
    }

    _ui->pkgLatestMTimeLabel->setText( formatTime( pkg->latestMtime() ) );
}


void FileDetailsView::showPkgSummary( PkgInfo * pkg )
{
    // logDebug() << "Showing pkg details about " << pkg << endl;

    if ( ! pkg )
    {
	clear();
	return;
    }

    if ( pkg->url() != "Pkg:/" )
    {
        showDetails( pkg );
        return;
    }

    setCurrentPage( _ui->pkgSummaryPage );

    setLabel( _ui->pkgSummaryPkgCountLabel, pkg->directChildrenCount() );

    if ( ! pkg->isBusy() && pkg->readState() == DirFinished )
    {
        setLabel( _ui->pkgSummaryTotalSizeLabel,   pkg->totalSize()    );
        setLabel( _ui->pkgSummaryItemCountLabel,   pkg->totalItems()   );
        setLabel( _ui->pkgSummaryFileCountLabel,   pkg->totalFiles()   );
        setLabel( _ui->pkgSummarySubDirCountLabel, pkg->totalSubDirs() );
    }
    else
    {
        QString msg;

        if ( pkg->isBusy() )
        {
            msg = tr( "[Reading]" );
        }
        else
        {
            if ( pkg->readState() == DirError )
                msg = tr( "[Read Error]" );
        }

        _ui->pkgSummaryTotalSizeLabel->setText( msg );
        _ui->pkgSummaryItemCountLabel->clear();
        _ui->pkgSummaryFileCountLabel->clear();
        _ui->pkgSummarySubDirCountLabel->clear();
    }

    _ui->pkgSummaryLatestMTimeLabel->setText( formatTime( pkg->latestMtime() ) );
}


void FileDetailsView::showSelectionSummary( const FileInfoSet & selectedItems )
{
    // logDebug() << "Showing selection summary" << endl;

    setCurrentPage( _ui->selectionSummaryPage );
    FileInfoSet sel = selectedItems.normalized();

    int fileCount	 = 0;
    int dirCount	 = 0;
    int subtreeFileCount = 0;

    foreach ( FileInfo * item, sel )
    {
	if ( item->isDir() )
	{
	    ++dirCount;
	    subtreeFileCount += item->totalFiles();
	}
	else
	    ++fileCount;
    }

    _ui->selFileCountCaption->setEnabled( fileCount > 0 );
    _ui->selFileCountLabel->setEnabled( fileCount > 0 );

    _ui->selDirCountCaption->setEnabled( dirCount > 0 );
    _ui->selDirCountLabel->setEnabled( dirCount > 0 );

    _ui->selSubtreeFileCountCaption->setEnabled( subtreeFileCount > 0 );
    _ui->selSubtreeFileCountLabel->setEnabled( subtreeFileCount > 0 );

    setLabel( _ui->selItemCount,	     sel.count()      );
    setLabel( _ui->selTotalSizeLabel,	     sel.totalSize()  );
    setLabel( _ui->selFileCountLabel,	     fileCount	      );
    setLabel( _ui->selDirCountLabel,	     dirCount	      );
    setLabel( _ui->selSubtreeFileCountLabel, subtreeFileCount );
}


void FileDetailsView::setLabel( QLabel * label, int number )
{
    CHECK_PTR( label );
    label->setText( QString::number( number ) );
}


void FileDetailsView::setLabel( QLabel * label, FileSize size )
{
    CHECK_PTR( label );
    label->setText( formatSize( size ) );
}


void FileDetailsView::setLabelLimited( QLabel * label, const QString & text )
{
    CHECK_PTR( label );
    QString limitedText = limitText( text );
    label->setText( limitedText );
}


QString FileDetailsView::limitText( const QString & longText )
{
    if ( _labelLimit < 1 || longText.size() < _labelLimit )
	return longText;

    QString limited = longText.left( _labelLimit / 2 - 2 );
    limited += "...";
    limited += longText.right( _labelLimit / 2 - 1 );

    logDebug() << "Limiting \"" << longText << "\"" << endl;

    return limited;
}


QString FileDetailsView::mimeCategory( FileInfo * file )
{
    if ( ! _mimeCategorizer )
    {
        _mimeCategorizer = new MimeCategorizer( this );
        CHECK_NEW( _mimeCategorizer );
    }

    MimeCategory * category = _mimeCategorizer->category( file );

    return category ? category->name() : "";
}
