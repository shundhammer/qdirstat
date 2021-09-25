/*
 *   File name: MimeCategory.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef MimeCategory_h
#define MimeCategory_h


#include <QString>
#include <QList>
#include <QStringList>
#include <QColor>
#include <QRegExp>
#include <QTextStream>


namespace QDirStat
{
    typedef QList<QRegExp> QRegExpList;

    /**
     * Class that represents a category of MIME types like video, music,
     * images, summarizing more detailed MIME types like video/mp4, video/mpeg,
     * video/x-flic etc.
     *
     * The idea is to collect those detailed types in one category to give it
     * common attributes like the QDirStat treemap color so the user can get an
     * impression how much disk space each type consumes. If there are too many
     * different colors, it is difficult to tell which is which, so we are
     * summarizing similar MIME types into their common category.
     *
     * To match files against a MimeCategory, simple wildcard regexps are
     * used. It does not try any fancy file content matching, it just checks
     * the filename. In most cases this will be a filename extension (a
     * suffix), and in most cases this comparison will be case
     * insensitive. This class tries to optimize for this for improved
     * performance.
     **/
    class MimeCategory
    {
    public:
	/**
	 * Create a MimeCategory with the specified name and optional color.
	 **/
	MimeCategory( const QString & name,
                      const QColor  & color = QColor() );

	/**
	 * Destructor.
	 **/
	~MimeCategory();

	/**
	 * Set the color for this category.
	 **/
	void setColor( const QColor & color ) { _color = color; }

	/**
	 * Return the color for this category.
	 **/
	QColor color() const { return _color; }

	/**
	 * Return the name of this category.
	 **/
	QString name() const { return _name; }

	/**
	 * Set the name of this category.
	 **/
	void setName( const QString & newName ) { _name = newName; }

	/**
	 * Add a filename suffix (extension) to this category.
	 * A leading "*." or "*" is cut off.
	 **/
	void addSuffix( const QString &	    suffix,
			Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive );

	/**
	 * Add a filename pattern to this category. If the pattern starts with
	 * "*." and does not contain any other wildcard characters, add it as a
	 * suffix. Otherwise, this will become a QRegExp::Wildcard regexp.
	 **/
	void addPattern( const QString &     pattern,
			 Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive );

	/**
	 * Add a list of patterns. See addPattern() for details.
	 **/
	void addPatterns( const QStringList & patterns,
			  Qt::CaseSensitivity caseSensitivity );

        /**
         * Add a list of filename suffixes (extensions) to this category.
	 * A leading "*." or "*" is cut off.
         **/
	void addSuffixes( const QStringList & suffixes,
                          Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive );

	/**
	 * Clear any suffixes or patterns for this category.
	 **/
	void clear();

	/**
	 * Return the list of case-insensitive suffixes for this category.
	 * The suffixes do not contain any leading wildcard or dot,
	 * i.e. it will be "tar.bz2", not ".tar.bz2" or "*.tar.bz2".
	 **/
	const QStringList & caseInsensitiveSuffixList() const
	    { return _caseInsensitiveSuffixList; }

	/**
	 * Return the list of case-sensitive suffixes for this category.
	 * The suffixes do not contain any leading wildcard or dot,
	 * i.e. it will be "tar.bz2", not ".tar.bz2" or "*.tar.bz2".
	 **/
	const QStringList & caseSensitiveSuffixList() const
	    { return _caseSensitiveSuffixList; }

	/**
	 * Return the list of patterns for this category that are not simple
	 * suffix patterns.
	 **/
	const QRegExpList & patternList() const
	    { return _patternList; }

	/**
	 * Return a sorted list of all either case sensitive or case
	 * insensitive suffixes and patterns for this category in human
	 * readable form, i.e. prepend suffixes with "*.":
	 * "tar.bz2" -> "*.tar.bz2".
	 *
	 * This is useful for populating widgets.
	 **/
	QStringList humanReadablePatternList( Qt::CaseSensitivity caseSensitivity );

	/**
	 * Convert a suffix list into the commonly used human readable form,
	 * i.e. prepend it with "*.": "tar.bz2" -> "*.tar.bz2".
	 **/
	static QStringList humanReadableSuffixList( const QStringList & suffixList );

	/**
	 * Filter out either case sensitive or case insensitive patterns from a
	 * pattern list and convert them into human readable form.
	 **/
	static QStringList humanReadablePatternList( const QRegExpList & patternList,
						     Qt::CaseSensitivity caseSensitivity );

    protected:

	/**
	 * Return 'true' if 'pattern' is a simple suffix pattern, i.e. it
	 * starts with "*." and does not contain any more wildcard characters.
	 **/
	bool isSuffixPattern( const QString & pattern );

	//
	// Data members
	//

	QString		_name;
	QColor		_color;
	QStringList	_caseInsensitiveSuffixList;
	QStringList	_caseSensitiveSuffixList;
	QRegExpList	_patternList;

    };	// class MimeCategory


    typedef QList<MimeCategory *> MimeCategoryList;


    /**
     * Human-readable output of a MimeCategory in a debug stream.
     **/
    inline QTextStream & operator<< ( QTextStream & str, MimeCategory * category )
    {
        if ( category )
            str << "<MimeCategory " << category->name() << ">";
        else
            str << "<NULL MimeCategory *>";

	return str;
    }


}	// namespace QDirStat

#endif	// MimeCategory_h
