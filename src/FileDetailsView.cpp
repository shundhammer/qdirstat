/*
 *   File name: FileDetailsView.h
 *   Summary:	Details view for the currently selected file or directory
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Qt4Compat.h"

#include "FileDetailsView.h"
#include "AdaptiveTimer.h"
#include "DirInfo.h"
#include "DirTreeModel.h"
#include "FileInfoSet.h"
#include "MimeCategorizer.h"
#include "PkgQuery.h"
#include "SystemFileChecker.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

#define ALLOCATED_FAT_PERCENT	33
#define MAX_SYMLINK_TARGET_LEN	25

using namespace QDirStat;


FileDetailsView::FileDetailsView( QWidget * parent ):
    QStackedWidget( parent ),
    _ui( new Ui::FileDetailsView ),
    _pkgUpdateTimer( new AdaptiveTimer( this ) ),
    _labelLimit( 40 )
{
    CHECK_NEW( _ui );
    CHECK_NEW( _pkgUpdateTimer );

    _ui->setupUi( this );
    clear();
    readSettings();

    _normalTextColor = qAppPalette().color( QPalette::Active, QPalette::WindowText );
    _labelLimit = 0; // Unlimited
    // TO DO: Read _labelLimit from the config file

    _pkgUpdateTimer->addDelayStage(    0 );
    _pkgUpdateTimer->addDelayStage(  333 ); // millisec
    _pkgUpdateTimer->addDelayStage(  800 ); // millisec
    _pkgUpdateTimer->addDelayStage( 1200 ); // millisec

    _pkgUpdateTimer->addCoolDownPeriod(	 500 ); // millisec
    _pkgUpdateTimer->addCoolDownPeriod( 1500 ); // millisec
    _pkgUpdateTimer->addCoolDownPeriod( 3000 ); // millisec

    connect( _pkgUpdateTimer, SIGNAL( deliverRequest( QVariant ) ),
	     this,	      SLOT  ( updatePkgInfo ( QVariant ) ) );
}


FileDetailsView::~FileDetailsView()
{
    writeSettings();
    delete _ui;
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
    _ui->fileTypeLabel->setText( formatFilesystemObjectType( file->mode() ) );

    bool isSymLink = file->isSymLink();

    _ui->fileSymLinkTargetCaption->setVisible( isSymLink );
    _ui->fileSymLinkTargetLabel->setVisible  ( isSymLink );
    _ui->fileSymLinkBrokenWarning->setVisible( file->isBrokenSymLink() );

    _ui->fileMimeCategoryCaption->setVisible ( ! isSymLink );
    _ui->fileMimeCategoryLabel->setVisible   ( ! isSymLink );

    if ( isSymLink )
    {
	QString fullTarget  = file->symLinkTarget();
	QString shortTarget = fullTarget;
	QString contextText;

	if ( shortTarget.length() >= MAX_SYMLINK_TARGET_LEN )
	{
	    if ( shortTarget.contains( '/' ) )
		shortTarget = ".../" + baseName( shortTarget );
	}

	if ( shortTarget != fullTarget )
	    contextText = fullTarget;

	_ui->fileSymLinkTargetLabel->setText( shortTarget );
	_ui->fileSymLinkTargetLabel->setContextText( contextText );

    }
    else // ! isSymLink
    {
	QString category = mimeCategory( file );

	_ui->fileMimeCategoryCaption->setEnabled( ! category.isEmpty() );
	_ui->fileMimeCategoryLabel->setEnabled	( ! category.isEmpty() );
	_ui->fileMimeCategoryLabel->setText( category );
    }

    setFileSizeLabel( _ui->fileSizeLabel, file );
    setFileAllocatedLabel( _ui->fileAllocatedLabel, file );

    _ui->fileUserLabel->setText( file->userName() );
    _ui->fileGroupLabel->setText( file->groupName() );
    _ui->filePermissionsLabel->setText( formatPermissions( file->mode() ) );
    _ui->fileMTimeLabel->setText( formatTime( file->mtime() ) );

    if ( ! file->isSparseFile() )
	suppressIfSameContent( _ui->fileSizeLabel, _ui->fileAllocatedLabel, _ui->fileAllocatedCaption );
}


void FileDetailsView::setFileSizeLabel( FileSizeLabel * label,
					FileInfo *	file )
{
    CHECK_PTR( file );

    QString text;

    if ( file->links() > 1 )
    {
	label->setText( tr( "%1 / %2 Links" )
			.arg( formatSize( file->rawByteSize() ) )
			.arg( file->links() ) );

	if ( file->rawByteSize() >= 1024 ) // Not useful below 1 kB
	{
	    label->setContextText( tr( "%1 / %2 Links" )
				   .arg( formatByteSize( file->rawByteSize() ) )
				   .arg( file->links() ) );
	}
    }
    else
    {
	label->setValue( file->rawByteSize() );
    }
}


void FileDetailsView::setFileAllocatedLabel( FileSizeLabel * label,
					     FileInfo *	     file )
{
    CHECK_PTR( file );

    if ( file->links() > 1 )
    {
	label->setText( tr( "%1 / %2 Links" )
			.arg( formatSize( file->rawAllocatedSize() ) )
			.arg( file->links() ) );

	label->setContextText( tr( "%1 / %2 Links" )
			       .arg( formatByteSize( file->rawAllocatedSize() ) )
			       .arg( file->links() ) );
    }
    else
    {
	label->setValue( file->allocatedSize() );
    }

    _ui->fileAllocatedLabel->setBold( file->usedPercent() < ALLOCATED_FAT_PERCENT ||
				      file->isSparseFile() );
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

    QString name    = dir->isPseudoDir() ? dir->name() : ( dir->baseName() + "/" );
    QString dirType = dir->isPseudoDir() ? tr( "Pseudo Directory" ) : tr( "Directory" );

    if ( dir->isMountPoint() )
	dirType = tr( "Mount Point" );

    setLabelLimited(_ui->dirNameLabel, name );
    _ui->dirTypeLabel->setText( dirType );

    _ui->dirIcon->setVisible( ! dir->readError() );
    _ui->dirUnreadableIcon->setVisible( dir->readError() );

    // Subtree information

    showSubtreeInfo( dir );
    showDirNodeInfo( dir );
}


void FileDetailsView::showSubtreeInfo( DirInfo * dir )
{
    CHECK_PTR( dir );
    QString msg;

    _ui->dirLockedIcon->setVisible( dir->readError() );

    switch ( dir->readState() )
    {
	case DirQueued:
	case DirReading:		msg = tr( "[Reading]"		); break;

	case DirOnRequestOnly:		msg = tr( "[Not Read]"		); break;
	case DirPermissionDenied:	msg = tr( "[Permission Denied]" ); break;
	case DirError:			msg = tr( "[Read Error]"	); break;

	case DirFinished:
	case DirCached:
	case DirAborted:
	    break;
    }

    if ( msg.isEmpty() )
    {
	// No special msg -> show summary fields

	QString prefix = dir->sizePrefix();

	setLabel( _ui->dirTotalSizeLabel,   dir->totalSize(),	       prefix );
	setLabel( _ui->dirAllocatedLabel,   dir->totalAllocatedSize(), prefix );
	setLabel( _ui->dirItemCountLabel,   dir->totalItems(),	       prefix );
	setLabel( _ui->dirFileCountLabel,   dir->totalFiles(),	       prefix );
	setLabel( _ui->dirSubDirCountLabel, dir->totalSubDirs(),       prefix );
	_ui->dirLatestMTimeLabel->setText( formatTime( dir->latestMtime() ) );

	suppressIfSameContent( _ui->dirTotalSizeLabel, _ui->dirAllocatedLabel, _ui->dirAllocatedCaption );
	_ui->dirAllocatedLabel->setBold( dir->totalUsedPercent() < ALLOCATED_FAT_PERCENT );
    }
    else  // Special msg -> show it and clear all summary fields
    {
	_ui->dirTotalSizeLabel->setText( msg );
	_ui->dirAllocatedLabel->clear();
	_ui->dirItemCountLabel->clear();
	_ui->dirFileCountLabel->clear();
	_ui->dirSubDirCountLabel->clear();
	_ui->dirLatestMTimeLabel->clear();
    }
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
	_ui->dirMTimeLabel->setVisible	( dir->mtime() > 0);
	_ui->dirMTimeLabel->setText( formatTime( dir->mtime() ) );


	// Show permissions in read if there was a "permission denied" error
	// while reading this directory

	setLabelColor( _ui->dirPermissionsLabel,
		       dir->readState() == DirPermissionDenied ?
		       _dirReadErrColor : _normalTextColor );
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

    QString msg;

    switch ( pkg->readState() )
    {
	case DirQueued:
	case DirReading:	   msg = tr( "[Reading]"	   ); break;

	case DirPermissionDenied:  msg = tr( "[Permission Denied]" ); break;
	case DirError:		   msg = tr( "[Read Error]"	   ); break;
	case DirAborted:	   msg = tr( "[Aborted]"	   ); break;

	case DirFinished:
	    break;

	case DirCached:
	case DirOnRequestOnly:
	    logError() << "Invalid readState for a Pkg" << endl;
	    break;
    }

    if ( msg.isEmpty() )
    {
	// No special msg -> show summary fields

	QString prefix = pkg->sizePrefix();

	setLabel( _ui->pkgTotalSizeLabel,   pkg->totalSize()	      );
	setLabel( _ui->pkgAllocatedLabel,   pkg->totalAllocatedSize() );
	setLabel( _ui->pkgItemCountLabel,   pkg->totalItems()	      );
	setLabel( _ui->pkgFileCountLabel,   pkg->totalFiles()	      );
	setLabel( _ui->pkgSubDirCountLabel, pkg->totalSubDirs()	      );

	suppressIfSameContent( _ui->pkgTotalSizeLabel, _ui->pkgAllocatedLabel, _ui->pkgAllocatedCaption );
    }
    else  // Special msg -> show it and clear all summary fields
    {
	_ui->pkgTotalSizeLabel->setText( msg );
	_ui->pkgAllocatedLabel->clear();
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
	setLabel( _ui->pkgSummaryTotalSizeLabel,   pkg->totalSize()	     );
	setLabel( _ui->pkgSummaryAllocatedLabel,   pkg->totalAllocatedSize() );
	setLabel( _ui->pkgSummaryItemCountLabel,   pkg->totalItems()	     );
	setLabel( _ui->pkgSummaryFileCountLabel,   pkg->totalFiles()	     );
	setLabel( _ui->pkgSummarySubDirCountLabel, pkg->totalSubDirs()	     );

	suppressIfSameContent( _ui->pkgSummaryTotalSizeLabel,
			       _ui->pkgSummaryAllocatedLabel,
			       _ui->pkgSummaryAllocatedCaption );
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
	    if ( pkg->readError() )
		msg = tr( "[Read Error]" );
	}

	_ui->pkgSummaryTotalSizeLabel->setText( msg );
	_ui->pkgSummaryAllocatedLabel->clear();
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

    setLabel( _ui->selItemCount,	     sel.count()	      );
    setLabel( _ui->selTotalSizeLabel,	     sel.totalSize()	      );
    setLabel( _ui->selAllocatedLabel,	     sel.totalAllocatedSize() );
    setLabel( _ui->selFileCountLabel,	     fileCount		      );
    setLabel( _ui->selDirCountLabel,	     dirCount		      );
    setLabel( _ui->selSubtreeFileCountLabel, subtreeFileCount	      );

    suppressIfSameContent( _ui->selTotalSizeLabel, _ui->selAllocatedLabel, _ui->selAllocatedCaption );
}


void FileDetailsView::setLabel( QLabel *	label,
				int		number,
				const QString & prefix )
{
    CHECK_PTR( label );
    label->setText( prefix + QString::number( number ) );
}


void FileDetailsView::setLabel( FileSizeLabel * label,
				FileSize	size,
				const QString & prefix )
{
    CHECK_PTR( label );
    label->setValue( size, prefix );
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


void FileDetailsView::suppressIfSameContent( FileSizeLabel * origLabel,
					     FileSizeLabel * cloneLabel,
					     QLabel	   * caption )
{
    if ( origLabel->text() == cloneLabel->text() )
    {
	cloneLabel->clear();
	caption->setEnabled( false );
    }
    else
    {
	caption->setEnabled( true );
    }
}


QString FileDetailsView::mimeCategory( FileInfo * file )
{
    MimeCategory * category = MimeCategorizer::instance()->category( file );

    return category ? category->name() : "";
}


void FileDetailsView::setLabelColor( QLabel * label, const QColor & color )
{
    CHECK_PTR( label );

    QPalette pal = label->palette();
    pal.setColor( QPalette::WindowText, color );
    label->setPalette( pal );
}


void FileDetailsView::readSettings()
{
    Settings settings;
    settings.beginGroup( "DetailsPanel" );

    _dirReadErrColor = readColorEntry( settings, "DirReadErrColor", QColor( Qt::red ) );

    settings.endGroup();
}


void FileDetailsView::writeSettings()
{
    Settings settings;
    settings.beginGroup( "DetailsPanel" );

    writeColorEntry( settings, "DirReadErrColor", _dirReadErrColor );

    settings.endGroup();
}

