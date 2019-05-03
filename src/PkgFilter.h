/*
 *   File name: PkgFilter.h
 *   Summary:	Package manager Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef PkgFilter_h
#define PkgFilter_h

#include <QString>
#include <QRegExp>
#include <QTextStream>


namespace QDirStat
{
    /**
     * Class to select packages from the complete set of installed packages.
     **/
    class PkgFilter
    {
    public:

        enum FilterMode
        {
            Auto,       // Guess from pattern (see below)
            Contains,   // Fixed string
            StartsWith, // Fixed string
            ExactMatch, // Fixed string
            Wildcard,
            RegExp,
            SelectAll   // Pattern is irrelevant
        };

        /**
         * Constructor: Create a package filter with the specified pattern and
         * filter mode. If the pattern is a package URL (i.e. if it starts with
         * "Pkg:" or "pkg:"), use only the relevant part of the URL:
         *
         * "Pkg:wantedPkg"        -> "wantedPkg"
         * "Pkg:/wantedPkg"       -> "wantedPkg"
         * "Pkg:/wantedPkg/foo"   -> "wantedPkg"
         * "Pkg:/wanted*Pkg/foo"  -> "wanted*Pkg"
         *
         * Filter mode "Auto" tries to guess a useful mode from the relevant
         * part of the URL:
         *
         * - If it's a fixed string without any wildcards, it uses
         *   "StartsWith".
         * - If it contains "*" wildcard characters, it uses "Wildcard".
         * - If it contains ".*" or "^" or "$", it uses "RegExp".
         * - If it starts with "=", it uses "ExactMatch".
         * - If it's empty (i.e. just "Pkg:/"), it uses "SelectAll".
         **/
        PkgFilter( const QString & pattern,
                   FilterMode      filterMode = Auto );

        /**
         * Constructor: Create a package filter with the specified regexp
         * pattern and set the filter mode according to the regexp's mode
         * (RegExp or Wildcard).
         *
         * Notice that this does NOT remove any leading "Pkg:" or "Pkg:/" part
         * or any part after a slash; it uses the pattern literally.
         **/
        PkgFilter( const QRegExp & pattern );

        /**
         * Check if a string matches this filter.
         **/
        bool matches( const QString & str ) const;

        /**
         * Check if a URL is a package URL, i.e. if it starts with "Pkg:" or
         * "pkg:".
         **/
        static bool isPkgUrl( const QString & url );

        /**
         * Return the pattern without the leading "Pkg:/".
         **/
        const QString & pattern() const { return _pattern; }

        /**
         * Return the package URL including the leading "Pkg:/".
         **/
        QString url() const;

        /**
         * Return the regexp. This is only meaningful in filter modes RegExp
         * and Wildcard.
         **/
        const QRegExp & regexp() const { return _regexp; }

        /**
         * Return the filter mode.
         **/
        FilterMode filterMode() const { return _filterMode; }

        /**
         * Convert a filter mode to a string.
         **/
        static QString toString( FilterMode filterMode );


    protected:

        /**
         * Normalize the pattern, i.e. remove any leading "Pkg:" or "Pkg:/" and
         * any trailing part after any slashes.
         **/
        void normalizePattern();

        /**
         * Guess the filter mode from the pattern if "Auto" was selected.
         **/
        void guessFilterMode();


        // Data members

        QString         _pattern;
        QRegExp         _regexp;
        FilterMode      _filterMode;

    };  // class PkgFilter


    inline QTextStream & operator<< ( QTextStream     & stream,
                                      const PkgFilter & pkgFilter )
    {
        stream << pkgFilter.url() << " filter mode \""
               << PkgFilter::toString( pkgFilter.filterMode() ) << "\"";

        return stream;
    }
}

#endif  // PkgFilter_h
