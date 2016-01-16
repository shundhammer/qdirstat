/*
 *   File name: MimeCategorizer.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "MimeCategorizer.h"
#include "FileInfo.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;

MimeCategorizer::MimeCategorizer( QObject * parent ):
    QObject( parent ),
    _mapsDirty( true )
{
    readSettings();
}


MimeCategorizer::~MimeCategorizer()
{
    writeSettings();
    clear();
}


void MimeCategorizer::clear()
{
    qDeleteAll( _categories );
    _categories.clear();
    _mapsDirty = true;
}


MimeCategory * MimeCategorizer::category( FileInfo * item )
{
    CHECK_PTR  ( item );
    CHECK_MAGIC( item );

    if ( item->isDir() || item->isDirInfo() )
	return 0;
    else
	return category( item->name() );
}


MimeCategory * MimeCategorizer::category( const QString & filename )
{
    if ( filename.isEmpty() )
	return 0;

    // Build suffix maps for fast lookup

    if ( _mapsDirty )
	buildMaps();

    MimeCategory * category = 0;

    // Find the filename suffix: Everything after the first '.'
    QString suffix = filename.section( '.', 1 );

    while ( ! suffix.isEmpty() )
    {
	// Try case sensitive first

	category = _caseSensitiveSuffixMap.value( suffix, 0 );

	if ( ! category )
	    category = _caseInsensitiveSuffixMap.value( suffix.toLower(), 0 );

	// No match so far? Try the next suffix. Some files might have more
	// than one, e.g., "tar.bz2" - if there is no match for "tar.bz2",
	// there might be one for just "bz2".

	suffix = suffix.section( '.', 1 );
    }

    if ( ! category ) // No match yet?
	category = matchPatterns( filename );

#if 0
    if ( category )
	logVerbose() << "Found " << category << " for " << filename << endl;
#endif

    return category;
}


MimeCategory * MimeCategorizer::matchPatterns( const QString & filename ) const
{
    foreach ( MimeCategory * category, _categories )
    {
	if ( category )
	{
	    foreach ( const QRegExp & pattern, category->patternList() )
	    {
		if ( pattern.exactMatch( filename ) )
		    return category;
	    }
	}
    }

    return 0; // No match
}


void MimeCategorizer::add( MimeCategory * category )
{
    CHECK_PTR( category );

    _categories << category;
    _mapsDirty = true;
}


void MimeCategorizer::remove( MimeCategory * category )
{
    CHECK_PTR( category );

    _categories.removeAll( category );
    delete category;
    _mapsDirty = true;
}


void MimeCategorizer::buildMaps()
{
    _caseInsensitiveSuffixMap.clear();
    _caseSensitiveSuffixMap.clear();

    foreach ( MimeCategory * category, _categories )
    {
	CHECK_PTR( category );

	addSuffixes( _caseInsensitiveSuffixMap, category, category->caseInsensitiveSuffixList() );
	addSuffixes( _caseSensitiveSuffixMap,	category, category->caseSensitiveSuffixList()	);
    }

    _mapsDirty = false;
}


void MimeCategorizer::addSuffixes( QMap<QString, MimeCategory *> & suffixMap,
				   MimeCategory			 * category,
				   const QStringList		 & suffixList  )
{
    foreach ( const QString & suffix, suffixList )
    {
	if ( suffixMap.contains( suffix ) )
	{
	    logError() << "Duplicate suffix: " << suffix << " for "
		       << suffixMap.value( suffix ) << " and " << category
		       << endl;
	}
	else
	{
	    suffixMap[ suffix ] = category;
	}
    }
}


void MimeCategorizer::readSettings()
{
    if ( _categories.isEmpty() )
	addDefaultCategories();
}


void MimeCategorizer::writeSettings()
{

}


void MimeCategorizer::addDefaultCategories()
{
    MimeCategory * junk = new MimeCategory( tr( "Junk" ), Qt::red );
    CHECK_NEW( junk );
    add( junk );

    junk->addSuffix( "~"   );
    junk->addSuffix( "bak" );
    junk->addPattern( "core", Qt::CaseSensitive );


    MimeCategory * archives = new MimeCategory( tr( "Compressed Archives" ), Qt::green );
    CHECK_NEW( archives );
    add( archives );

    archives->addSuffixes( QStringList()
			   << "7z"
			   << "arj"
			   << "cab"
			   << "cpio.gz"
			   << "deb"
			   << "rar"
			   << "rpm"
			   << "tar.bz2"
			   << "tar.gz"
			   << "tgz"
			   << "zip"
			   );


    MimeCategory * uncompressedArchives = new MimeCategory( tr( "Uncompressed Archives" ), QColor( 128, 128, 0 ) );
    CHECK_NEW( uncompressedArchives );
    add( uncompressedArchives );

    uncompressedArchives->addSuffix( "tar"  );
    uncompressedArchives->addSuffix( "cpio" );


    MimeCategory * compressed = new MimeCategory( tr( "Compressed Files" ), Qt::green );
    CHECK_NEW( compressed );
    add( compressed );

    compressed->addSuffix( ".bz2" );
    compressed->addSuffix( ".gz" );


    MimeCategory * images = new MimeCategory( tr( "Images" ), Qt::cyan );
    CHECK_NEW( images );
    add( images );

    images->addSuffixes( QStringList()
			 << "gif"
			 << "jpeg"
			 << "jpg"
			 << "png"
			 << "tif"
			 << "tiff"
			 << "xcf.bz2"
			 << "xcf.gz"
			 << "xpm"	 // uncompressed, buy typically tiny
			 );


    MimeCategory * uncompressedImages = new MimeCategory( tr( "Uncompressed Images" ), Qt::red );
    CHECK_NEW( uncompressedImages );
    add( uncompressedImages );

    uncompressedImages->addSuffixes( QStringList()
				     << "bmp"
				     << "pbm"
				     << "pgm"
				     << "pnm"
				     << "ppm"
				     << "xcf"
				     );


    MimeCategory * videos = new MimeCategory( tr( "Videos" ), QColor( 0xa0, 0xff, 0x00 ) );
    CHECK_NEW( videos );
    add( videos );

    videos->addSuffixes( QStringList()
                         << "asf"
                         << "avi"
                         << "divx"
                         << "flc"
                         << "fli"
                         << "flv"
                         << "mk3d"
                         << "mkv"
                         << "mng"
                         << "mov"
                         << "mp2"
                         << "mp4"
                         << "mpeg"
                         << "mpg"
                         << "ogm"
                         << "ogv"
                         << "rm"
                         << "vdr"
                         << "vob"
                         << "webm"
                         << "wmp"
                         << "wmv"
                         );


    MimeCategory * music = new MimeCategory( tr( "Music" ), Qt::yellow );
    CHECK_NEW( music );
    add( music );

    music->addSuffixes( QStringList()
			<< "aac"
			<< "f4a"
			<< "f4b"
			<< "m4a"
			<< "m4b"
			<< "mid"
			<< "mka"
			<< "mp3"
			<< "oga"
			<< "ogg"
			<< "ra"
			<< "rax"
			<< "wav"
			<< "wma"
			);


    MimeCategory * doc = new MimeCategory( tr( "Documents" ), Qt::blue );
    CHECK_NEW( doc );
    add( doc );

    doc->addSuffixes( QStringList()
		      << "doc"
		      << "docx"
		      << "dotx"
		      << "dvi"
		      << "dvi.bz2"
		      << "epub"
		      << "htm"
		      << "html"
		      << "md"
		      << "odb"
		      << "odc"
		      << "odg"
		      << "odp"
		      << "ods"
		      << "odt"
		      << "otc"
		      << "otp"
		      << "ots"
		      << "pdf"
		      << "potx"
		      << "ppsx"
		      << "ppt"
		      << "pptx"
		      << "ps"
		      << "sdc"
		      << "sdc.gz"
		      << "sdd"
		      << "sdp"
		      << "sdw"
		      << "sla"
		      << "sla.gz"
		      << "slaz"
		      << "txt"
		      << "xls"
		      << "xlsx"
		      << "xlt"
		      << "css"
		      << "csv"
		      << "latex"
		      << "tex"
		      << "xml"
		      );


    MimeCategory * src = new MimeCategory( tr( "Source Files" ), Qt::cyan );
    CHECK_NEW( src );
    add( src );
    src->addSuffixes( QStringList()
		      << "c" << "cpp" << "cc" << "cxx" << "h" << "hpp" << "ui"
		      << "pl" << "py" << "rb" << "el"
		      << "pro" << "cmake"
		      , Qt::CaseSensitive );


    MimeCategory * obj = new MimeCategory( tr( "Object or Generated Files" ), QColor( 0xff, 0xa0, 0x00 ) );
    CHECK_NEW( obj );
    add( obj );

    obj->addSuffixes( QStringList()
		      << "o" << "lo" << "Po" << "al" << "la" << "moc" << "elc" << "log"
		      , Qt::CaseSensitive );

    obj->addPatterns( QStringList()
		      << "moc_*.cpp" << "ui_*.cpp" << "qrc_*.cpp"
		      , Qt::CaseSensitive );


    MimeCategory * libs = new MimeCategory( tr( "Shared libraries" ), QColor( 0xff, 0xa0, 0x00 ) );
    CHECK_NEW( libs );
    add( libs );

    libs->addPattern( "lib*.so.*", Qt::CaseSensitive );
    libs->addSuffix ( "dll" );

}
