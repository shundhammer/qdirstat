/*
 *   File name: MimeCategorizer.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "MimeCategorizer.h"
#include "FileInfo.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"



MimeCategorizer * MimeCategorizer::_instance = 0;


MimeCategorizer * MimeCategorizer::instance()
{
    if ( ! _instance )
    {
	_instance = new MimeCategorizer();
	CHECK_NEW( _instance );
    }

    return _instance;
}


MimeCategorizer::MimeCategorizer():
    QObject( 0 ),
    _mapsDirty( true )
{
    // logDebug() << "Creating MimeCategorizer" << endl;
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


QColor MimeCategorizer::color( FileInfo * item )
{
    MimeCategory *mimeCategory = category( item );

    return mimeCategory ? mimeCategory->color() : Qt::white;
}


MimeCategory * MimeCategorizer::category( FileInfo * item )
{
    CHECK_PTR  ( item );
    CHECK_MAGIC( item );

    if ( item->isSymLink() )
    {
	return matchCategoryName( CATEGORY_SYMLINKS );
    }
    else if ( item->isFile() )
    {
	MimeCategory *matchedCategory = category( item->name() );
	if ( ! matchedCategory && ( item->mode() & S_IXUSR  ) == S_IXUSR )
	    return matchCategoryName( CATEGORY_EXECUTABLES );

	return matchedCategory;
    }
    else
    {
	return 0;
    }
}


MimeCategory * MimeCategorizer::category( const QString & filename,
					  QString	* suffix_ret )
{
    if ( suffix_ret )
	*suffix_ret = "";

    if ( filename.isEmpty() )
	return 0;

    // Build suffix maps for fast lookup

    if ( _mapsDirty )
	buildMaps();

    MimeCategory * category = 0;

    // Find the filename suffix: Section #1
    // (ignoring any leading '.' separator)
    QString suffix = filename.section( '.', 1 );

    while ( ! suffix.isEmpty() && ! category )
    {
        // logVerbose() << "Checking " << suffix << endl;

	// Try case sensitive first

	category = _caseSensitiveSuffixMap.value( suffix, 0 );

	if ( ! category )
	    category = _caseInsensitiveSuffixMap.value( suffix.toLower(), 0 );

	if ( category ) // success
        {
            if ( suffix_ret )
                *suffix_ret = suffix;
        }
        else
        {
            // No match so far? Try the next suffix. Some files might have more
            // than one, e.g., "tar.bz2" - if there is no match for "tar.bz2",
            // there might be one for just "bz2".

            suffix = suffix.section( '.', 1 );
        }
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


MimeCategory * MimeCategorizer::matchCategoryName( const QString & categoryName ) const
{
    foreach ( MimeCategory * category, _categories )
    {
	if ( category && category->name() == categoryName )
	    return category;
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
    MimeCategorySettings settings;
    QStringList mimeCategoryGroups = settings.findGroups( settings.groupPrefix() );

    clear();

    // Read all settings groups [MimeCategory_xx] that were found

    foreach ( const QString & groupName, mimeCategoryGroups )
    {
	settings.beginGroup( groupName );

	QString name  = settings.value( "Name", groupName ).toString();
	QColor	color = readColorEntry( settings, "Color", QColor( "#b0b0b0" ) );
	QStringList patternsCaseInsensitive = settings.value( "PatternsCaseInsensitive" ).toStringList();
	QStringList patternsCaseSensitive   = settings.value( "PatternsCaseSensitive"	).toStringList();

	MimeCategory * category = new MimeCategory( name, color );
	CHECK_NEW( category );

	add( category );
	category->addPatterns( patternsCaseInsensitive, Qt::CaseInsensitive );
	category->addPatterns( patternsCaseSensitive,	Qt::CaseSensitive   );

	settings.endGroup(); // [MimeCategory_01], [MimeCategory_02], ...
    }

    if ( _categories.isEmpty() )
	addDefaultCategories();

    ensureMandatoryCategories();
}


void MimeCategorizer::writeSettings()
{
    // logDebug() << endl;
    MimeCategorySettings settings;

    // Remove all leftover cleanup descriptions
    settings.removeGroups( settings.groupPrefix() );

    for ( int i=0; i < _categories.size(); ++i )
    {
	settings.beginGroup( "MimeCategory", i+1 );

	MimeCategory * category = _categories.at(i);

	settings.setValue( "Name", category->name() );
	// logDebug() << "Adding " << groupName << ": " << category->name() << endl;
	writeColorEntry( settings, "Color", category->color() );

	QStringList patterns = category->humanReadablePatternList( Qt::CaseInsensitive );

	if ( patterns.isEmpty() )
	    patterns << "";

	settings.setValue( "PatternsCaseInsensitive", patterns );

	patterns = category->humanReadablePatternList( Qt::CaseSensitive );

	if ( patterns.isEmpty() )
	    patterns << "";

	settings.setValue( "PatternsCaseSensitive", patterns );

	settings.endGroup(); // [MimeCategory_01], [MimeCategory_02], ...
    }
}


void MimeCategorizer::ensureMandatoryCategories()
{
    // Remember this category so we don't have to search for it every time

    _executableCategory = matchCategoryName( CATEGORY_EXECUTABLES );
    if ( !_executableCategory )
    {
	// Special catch-all category for files that don't match anything else.
        // This category cannot be deleted.

	_executableCategory = new MimeCategory( tr( CATEGORY_EXECUTABLES ), Qt::magenta );
	CHECK_NEW( _executableCategory );
	add( _executableCategory );
    }

    // Remember this category so we don't have to search for it every time
    _symlinkCategory = matchCategoryName( CATEGORY_SYMLINKS );

    if ( !_symlinkCategory )
    {
	// Special category for symlinks regardless of the filename.
        // This category cannot be deleted.

	_symlinkCategory = new MimeCategory( tr( CATEGORY_SYMLINKS ), Qt::blue );
	CHECK_NEW( _symlinkCategory );
	add( _symlinkCategory );
    }
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
			   << "fsa"
			   << "jar"
			   << "rar"
			   << "rpm"
			   << "tar.bz2"
			   << "tar.gz"
			   << "tar.lz"
			   << "tar.lzo"
			   << "tar.xz"
			   << "tar.zst"
			   << "tbz2"
			   << "tgz"
			   << "txz"
			   << "tz2"
			   << "tzst"
			   << "zip"
			   << "zpaq"
			   );

    archives->addPattern( "pack-*.pack" );      // Git archive


    MimeCategory * uncompressedArchives = new MimeCategory( tr( "Uncompressed Archives" ), QColor( 128, 128, 0 ) );
    CHECK_NEW( uncompressedArchives );
    add( uncompressedArchives );

    uncompressedArchives->addSuffix( "tar"  );
    uncompressedArchives->addSuffix( "cpio" );


    MimeCategory * compressed = new MimeCategory( tr( "Compressed Files" ), Qt::green );
    CHECK_NEW( compressed );
    add( compressed );

    compressed->addSuffixes( QStringList()
             << "bz2"
             << "gz"
             << "lz"
             << "lzo"
             << "xz"
             << "zst"
             );


    MimeCategory * images = new MimeCategory( tr( "Images" ), Qt::cyan );
    CHECK_NEW( images );
    add( images );

    images->addSuffixes( QStringList()
			 << "gif"
			 << "jpeg"
			 << "jpg"
                         << "jxl"
			 << "png"
                         << "mng"
			 << "svg"
			 << "tif"
			 << "tiff"
                         << "webp"
			 << "xcf.bz2"
			 << "xcf.gz"
			 << "xpm"	 // uncompressed, but typically tiny
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
			 << "dv"
			 << "flc"
			 << "fli"
			 << "flv"
			 << "m2ts"
			 << "m4v"
			 << "mk3d"
			 << "mkv"
			 << "mov"
			 << "mp2"
			 << "mp4"
			 << "mpeg"
			 << "mpg"
			 << "mts"
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
			<< "aif"
			<< "ape"
			<< "caf"
			<< "dff"
			<< "dsf"
			<< "f4a"
			<< "f4b"
			<< "flac"
			<< "m4a"
			<< "m4b"
			<< "mid"
			<< "mka"
			<< "mp3"
			<< "oga"
			<< "ogg"
			<< "opus"
			<< "ra"
			<< "rax"
			<< "w64"
			<< "wav"
			<< "wma"
			<< "wv"
			<< "wvc"
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
		      << "ly"
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
		      << "sxi"
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
		      << "pl" << "py" << "rb" << "el" << "js" << "php" << "java"
		      << "pro" << "cmake"
		      , Qt::CaseSensitive );


    MimeCategory * obj = new MimeCategory( tr( "Object or Generated Files" ), QColor( 0xff, 0xa0, 0x00 ) );
    CHECK_NEW( obj );
    add( obj );

    obj->addSuffixes( QStringList()
		      << "o" << "lo" << "ko" << "Po" << "al" << "la" << "moc" << "elc" << "pyc"
		      , Qt::CaseSensitive );

    obj->addPatterns( QStringList()
		      << "moc_*.cpp" << "ui_*.cpp" << "qrc_*.cpp"
		      , Qt::CaseSensitive );


    MimeCategory * libs = new MimeCategory( tr( "Libraries" ), QColor( 0xff, 0xa0, 0x00 ) );
    CHECK_NEW( libs );
    add( libs );

    libs->addPattern( "lib*.so.*", Qt::CaseSensitive );
    libs->addPattern( "lib*.so",   Qt::CaseSensitive );
    libs->addPattern( "lib*.a",	   Qt::CaseSensitive );
    libs->addSuffix ( "dll" );
    libs->addSuffix ( "so" );
}
