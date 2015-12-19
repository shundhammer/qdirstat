/*
 *   File name: ExcludeRules.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef ExcludeRules_h
#define ExcludeRules_h


#include <QString>
#include <QRegExp>
#include <QList>


namespace QDirStat
{
    /**
     * One single exclude rule to check text (file names) against.
     * It can be enabled or disabled. Only enabled rules can ever match; a
     * disabled exclude rule will never exclude anything.
     **/
    class ExcludeRule
    {
    public:

	/**
	 * Constructor from a QRegExp.
	 **/
	ExcludeRule( const QRegExp & regexp );

	/**
	 * Constructor from a QString. The string will be used as a regexp.
	 **/
	ExcludeRule( const QString & regexp );

	/**
	 * Destructor.
	 **/
	virtual ~ExcludeRule();

	/**
	 * Check a string (usually a file name) against this exclude rule.
	 * Returns 'true' if the string matches, i.e. if the object this string
	 * belongs to should be excluded.
	 *
	 * Only enabled exclude rules will ever match; as long as it is
	 * disabled, this will always return 'false'.
	 **/
	bool match( const QString & text );

	/**
	 * Returns this rule's regular expression.
	 **/
	QRegExp regexp() const { return _regexp; }

	/**
	 * Change this rule's regular expression.
	 **/
	void setRegexp( const QRegExp & regexp ) { _regexp = regexp; }

	/**
	 * Check if this rule is enabled.
	 **/
	bool isEnabled() const { return _enabled; }

	/**
	 * Enable or disable this rule.
	 * New rules are always enabled by default.
	 **/
	void enable( bool enable = true ) { _enabled = enable; }


    private:

	QRegExp _regexp;
	bool	_enabled;
    };


    typedef QList<ExcludeRule *> ExcludeRuleList;
    typedef ExcludeRuleList::const_iterator ExcludeRuleListIterator;


    /**
     * Container for multiple exclude rules.
     *
     * Normal usage:
     *
     *	   ExcludeRules::instance()->add( new ExcludeRule( ... ) );
     *	   ...
     *	   if ( ExcludeRules::instance()->match( filename ) )
     *	   {
     *	       // exclude this file
     *	   }
     **/
    class ExcludeRules
    {
    public:

	/**
	 * Constructor.
	 *
	 * Most applications will want to use excludeRules() instead to create
	 * and use a singleton object of this class.
	 **/
	ExcludeRules();

	/**
	 * Destructor.
	 **/
	~ExcludeRules();

	/**
	 * Return the singleton object of this class.
	 * This will create one if there is none yet.
	 **/
	static ExcludeRules * instance();

	/**
	 * Add an exclude rule to this rule set.
	 * This transfers ownership of that rule to this rule set;
	 * it will be destroyed with 'delete' after use.
	 **/
	void add( ExcludeRule * rule );

	/**
	 * Create a new rule with 'regexp' and add it to this rule set.
	 **/
	static void add( const QRegExp & regexp );
	static void add( const QString & regexp );

	/**
	 * Check a string against the exclude rules.
	 * This will return 'true' if the text matches any (enabled) rule.
	 *
	 * Note that this operation will move current().
	 **/
	bool match( const QString & text );

	/**
	 * Find the exclude rule that matches 'text'.
	 * Return 0 if there is no match.
	 *
	 * This is intended to explain to the user which rule matched.
	 **/
	const ExcludeRule * matchingRule( const QString & text );

	/**
	 * Return the last matching rule or 0 if there was none.
	 * Each call to match() will reset this.
	 **/
	ExcludeRule * lastMatchingRule() const { return _lastMatchingRule; }

	/**
	 * Clear (delete) all exclude rules.
	 **/
	void clear();

	/**
	 * Return 'true' if the exclude rules are empty, i.e. if there are no
	 * exclue rules, 'false' otherwise.
	 **/
	bool isEmpty() { return _rules.isEmpty(); }

	/**
	 * Return a const iterator for the first exclude rule.
	 **/
	ExcludeRuleListIterator begin() { return _rules.constBegin(); }

	/**
	 * Return a const iterator for the last exclude rule.
	 **/
	ExcludeRuleListIterator end()	{ return _rules.constEnd(); }

    private:

	ExcludeRuleList _rules;
	ExcludeRule *	_lastMatchingRule;
    };


    /**
     * Print the regexp of a @ref FileInfo in a debug stream.
     **/
    inline QTextStream & operator<< ( QTextStream & stream, const ExcludeRule * rule )
    {
	if ( rule )
	    stream << "<ExcludeRule \"" << rule->regexp().pattern() << "\">";
	else
	    stream << "<NULL ExcludeRule *>";

	return stream;
    }

}	// namespace QDirStat


#endif // ifndef ExcludeRules_h

