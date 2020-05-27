/*
 *   File name: MimeCategorizer.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef MimeCategorizer_h
#define MimeCategorizer_h

#include <QObject>
#include <QMap>

#include "MimeCategory.h"


namespace QDirStat
{
    class FileInfo;

    /**
     * Class to determine the MimeCategory of filenames.
     *
     * This class is optimized for performance since the names of all files in
     * QDirStat's DirTree need to be checked (something in the order of 200,000
     * in a typical Linux root filesystem).
     *
     * This is a singleton class. Use instance() to get the instance. Remember
     * to call instance()->writeSettings() in an appropriate destructor in the
     * application to write the settings to disk.
     **/
    class MimeCategorizer: public QObject
    {
	Q_OBJECT

    protected:

	/**
	 * Constructor.
	 * This is a singleton class; use instance() instead.
	 **/
	MimeCategorizer();

	/**
	 * Destructor.
	 **/
	virtual ~MimeCategorizer();


    public:

	/**
	 * Get the singleton for this class. The first call to this will create
	 * it.
	 **/
	static MimeCategorizer * instance();

	/**
	 * Return the MimeCategory for a FileInfo item or 0 if it doesn't fit
	 * into any of the available categories.
	 **/
	MimeCategory * category( FileInfo * item );

	/**
	 * Return the MimeCategory for a filename or 0 if it doesn't fit into
	 * any of the available categories.
	 *
	 * If 'suffix_ret' is non-null, it returns the suffix used if the
	 * category was found by a suffix rule. If the category was not found
	 * or if a regexp (rather than a suffix rule) matched, this returns an
	 * empty string.
	 **/
	MimeCategory * category( const QString & filename, QString * suffix_ret = 0 );

	/**
	 * Add a MimeCategory.
	 **/
	void add( MimeCategory * category );

	/**
	 * Remove and delete a MimeCategory.
	 **/
	void remove( MimeCategory * category );

	/**
	 * Return the number of MimeCategories.
	 **/
	int size() const { return _categories.size(); }

	/**
	 * Return the MimeCategories list.
	 **/
	const MimeCategoryList & categories() const { return _categories; }

	/**
	 * Clear all categories.
	 **/
	void clear();


    public slots:

	/**
	 * Read the MimeCategory parameter from the settings.
	 **/
	void readSettings();

	/**
	 * Write the MimeCategory parameter to the settings.
	 **/
	void writeSettings();


    protected:

	/**
	 * Build the internal maps and clear the _mapsDirty flag.
	 **/
	void buildMaps();

	/**
	 * Add all suffixes in 'suffixList' as key to 'suffixMap' with value
	 * 'category'.
	 *
	 * This provides a really fast map lookup for each suffix.
	 **/
	void addSuffixes( QMap<QString, MimeCategory *> & suffixMap,
			  MimeCategory			* category,
			  const QStringList		& suffixList  );

	/**
	 * Iterate over all categories and try all patterns until the first
	 * match. Return the matched category or 0 if none matched.
	 **/
	MimeCategory * matchPatterns( const QString & filename ) const;

	/**
	 * Add default categories in case none were read from the settings.
	 **/
	void addDefaultCategories();

	//
	// Data members
	//

	static MimeCategorizer *	_instance;

	bool				_mapsDirty;
	MimeCategoryList		_categories;

	QMap<QString, MimeCategory *>	_caseInsensitiveSuffixMap;
	QMap<QString, MimeCategory *>	_caseSensitiveSuffixMap;

    };	// class MimeCategorizer

}	// namespace QDirStat

#endif	// MimeCategorizer_h
