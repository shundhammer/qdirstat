/*
 *   File name: SearchFilter.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef SearchFilter_h
#define SearchFilter_h

#include <QString>
#include <QRegExp>
#include <QTextStream>


namespace QDirStat
{
    /**
     * Base class for search filters like PkgFilter or FileSearchFilter.
     **/
    class SearchFilter
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
         * Constructor: Create a search filter with the specified pattern and
         * filter mode.
         *
         * Filter mode "Auto" tries to guess a useful mode from the pattern:
         *
         * - If it's a fixed string without any wildcards, it uses
         *   "StartsWith".
         * - If it contains "*" wildcard characters, it uses "Wildcard".
         * - If it contains ".*" or "^" or "$", it uses "RegExp".
         * - If it starts with "=", it uses "ExactMatch".
         * - If it's empty, it uses "SelectAll".
         **/
        SearchFilter( const QString & pattern,
                      FilterMode      filterMode = Auto );

        /**
         * Check if a string matches this filter.
         **/
        bool matches( const QString & str ) const;

        /**
         * Return the pattern.
         **/
        const QString & pattern() const { return _pattern; }

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
         * Guess the filter mode from the pattern if "Auto" was selected.
         **/
        void guessFilterMode();


        // Data members

        QString         _pattern;
        QRegExp         _regexp;
        FilterMode      _filterMode;

    };  // class SearchFilter


    inline QTextStream & operator<< ( QTextStream        & stream,
                                      const SearchFilter & filter )
    {
        stream << filter.pattern() << " filter mode \""
               << SearchFilter::toString( filter.filterMode() ) << "\"";

        return stream;
    }
}

#endif  // SearchFilter_h
