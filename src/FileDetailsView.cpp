/*
 *   File name: FileDetailsView.h
 *   Summary:	Details view for the currently selected file or directory
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "FileDetailsView.h"
#include "DirInfo.h"
#include "FileInfoSet.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


FileDetailsView::FileDetailsView( QWidget * parent ):
    QStackedWidget( parent ),
    _ui( new Ui::FileDetailsView ),
    _labelLimit( 40 )
{
    CHECK_NEW( _ui );
    _ui->setupUi( this );
    clear();

    // TODO: Read _labelLimit from the config file
}


FileDetailsView::~FileDetailsView()
{

}


void FileDetailsView::clear()
{
    setCurrentWidget( _ui->emptyPage );
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
    // logDebug() << "Showing file details about " << file << endl;

    if ( ! file )
    {
        clear();
        return;
    }

    setCurrentWidget( _ui->fileDetailsPage );
    showFileInfo( file );
    showFilePkgInfo( file );
}


void FileDetailsView::showFileInfo( FileInfo * file )
{
    CHECK_PTR( file );

    setLabelLimited(_ui->fileNameLabel, file->name() );
    _ui->fileTypeLabel->setText( formatFileSystemObjectType( file->mode() ) );

    // TODO: Mime type
    _ui->fileMimeTypeLabel->setText( "" );

    setLabel( _ui->fileSizeLabel, file->totalSize() );
    _ui->fileUserLabel->setText( file->userName() );
    _ui->fileGroupLabel->setText( file->groupName() );
    _ui->filePermissionsLabel->setText( formatPermissions( file->mode() ) );

    _ui->fileMTimeLabel->setText( formatTime( file->mtime() ) );
}


void FileDetailsView::showFilePkgInfo( FileInfo * file )
{
    CHECK_PTR( file );

    setSystemFileBlockVisibility( false ); // TO DO

    // FIXME: TO DO
    // FIXME: TO DO
    // FIXME: TO DO
}


void FileDetailsView::setSystemFileBlockVisibility( bool visible )
{
    _ui->fileSystemFileWarning->setVisible( visible );
    _ui->filePackageCaption->setVisible( visible );
    _ui->filePackageLabel->setVisible( visible );
    _ui->fileSpacerCaption2->setVisible( visible );
}


void FileDetailsView::showDetails( DirInfo * dir )
{
    // logDebug() << "Showing dir details about " << dir << endl;

    if ( ! dir )
    {
        clear();
        return;
    }

    setCurrentWidget( _ui->dirDetailsPage );

    QString name = dir->isDotEntry() ? FileInfo::dotEntryName() : ( baseName( dir->url() ) + "/" );
    QString dirType = dir->isDotEntry() ? tr( "Pseudo Directory" ) : tr( "Directory" );

    setLabelLimited(_ui->dirNameLabel, name );
    _ui->dirTypeLabel->setText( dirType );

    // Subtree information

    showSubtreeInfo( dir );
    showDirNodeInfo( dir );
}


void FileDetailsView::showSubtreeInfo( DirInfo * dir )
{
    CHECK_PTR( dir );

    setLabel( _ui->dirTotalSizeLabel, dir->totalSize() );
    setLabel( _ui->dirItemCountLabel, dir->totalItems() );
    setLabel( _ui->dirFileCountLabel, dir->totalFiles() );
    setLabel( _ui->dirSubDirCountLabel, dir->totalSubDirs() );

    _ui->dirLatestMTimeLabel->setText( formatTime( dir->latestMtime() ) );
}


void FileDetailsView::showDirNodeInfo( DirInfo * dir )
{
    CHECK_PTR( dir );
    setDirBlockVisibility( ! dir->isDotEntry() );

    if ( ! dir->isDotEntry() )
    {
        setLabel( _ui->dirOwnSizeLabel, dir->size() );
        _ui->dirUserLabel->setText( dir->userName() );
        _ui->dirGroupLabel->setText( dir->groupName() );
        _ui->dirPermissionsLabel->setText( formatPermissions( dir->mode() ) );
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
}


void FileDetailsView::showSelectionSummary( const FileInfoSet & selectedItems )
{
    // logDebug() << "Showing selection summary" << endl;

    setCurrentWidget( _ui->selectionSummaryPage );
    FileInfoSet sel = selectedItems.normalized();

    int fileCount        = 0;
    int dirCount         = 0;
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

    _ui->selFileCountLabel->setEnabled( fileCount > 0 );
    _ui->selDirCountLabel->setEnabled( dirCount > 0 );
    _ui->selSubtreeFileCountLabel->setEnabled( subtreeFileCount > 0 );

    setLabel( _ui->selItemCount,             sel.count()      );
    setLabel( _ui->selTotalSizeLabel,        sel.totalSize()  );
    setLabel( _ui->selFileCountLabel,        fileCount        );
    setLabel( _ui->selDirCountLabel,         dirCount         );
    setLabel( _ui->selSubtreeFileCountLabel, subtreeFileCount );
}


void FileDetailsView::setLabel( QLabel * label, int number )
{
    label->setText( QString::number( number ) );
}


void FileDetailsView::setLabel( QLabel * label, FileSize size )
{
    label->setText( formatSize( size ) );
}


void FileDetailsView::setLabelLimited( QLabel * label, const QString & text )
{
    QString limitedText = limitText( text );
    label->setText( limitedText );
}


QString FileDetailsView::limitText( const QString & longText )
{
    if ( longText.size() < _labelLimit )
        return longText;

    QString limited = longText.left( _labelLimit / 2 - 2 );
    limited += "...";
    limited += longText.right( _labelLimit / 2 - 1 );

    logDebug() << "Limiting \"" << longText << "\"" << endl;

    return limited;
}
