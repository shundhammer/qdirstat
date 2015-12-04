/*
 *   File name: kexcluderules.h
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2008-12-18
 */


#ifndef KExcludeRules_h
#define KExcludeRules_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <qstring.h>
#include <qregexp.h>
#include <qptrlist.h>


namespace KDirStat
{
    /**
     * One single exclude rule to check text (file names) against.
     * It can be enabled or disabled. Only enabled rules can ever match; a
     * disabled exclude rule will never exclude anything.
     **/
    class KExcludeRule
    {
    public:

	/**
	 * Constructor.
	 **/
	KExcludeRule( const QRegExp & regexp );

	/**
	 * Destructor.
	 **/
	virtual ~KExcludeRule();

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

	QRegExp	_regexp;
	bool	_enabled;
    };


    /**
     * Container for multiple exclude rules.
     *
     * Normal usage:
     *
     *     KExcludeRules::excludeRules()->add( new KExcludeRule( ... ) );
     *     ...
     *     if ( KExcludeRules::excludeRules()->match( filename ) )
     *	   {
     *         // exclude this file
     *     }
     **/
    class KExcludeRules
    {
    public:

	/**
	 * Constructor.
	 *
	 * Most applications will want to use excludeRules() instead to create
	 * and use a singleton object of this class.
	 **/
	KExcludeRules();

	/**
	 * Destructor.
	 **/
	~KExcludeRules();

	/**
	 * Return the singleton object of this class.
	 * This will create one if there is none yet.
	 **/
	static KExcludeRules * excludeRules();

	/**
	 * Add an exclude rule to this rule set.
	 * This transfers ownership of that rule to this rule set;
	 * it will be destroyed with 'delete' after use.
	 **/
	void add( KExcludeRule * rule );

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
	const KExcludeRule * matchingRule( const QString & text );

	/**
	 * Returns the first exclude rule of this rule set
	 * or 0 if there is none.
	 **/
	KExcludeRule * first() { return _rules.first(); }
	
	/**
	 * Returns the next exclude rule (after first() or next() )
	 * of this rule set or 0 if there is no more.
	 **/
	KExcludeRule * next() { return _rules.next(); }
	
	/**
	 * Returns the current exclude rule of this rule set
	 * or 0 if there is none.
	 **/
	KExcludeRule * current() { return _rules.current(); }

	/**
	 * Clear (delete) all exclude rules.
	 **/
	void clear() { _rules.clear(); }

    private:

	QPtrList<KExcludeRule>    _rules;
    };

}	// namespace KDirStat


#endif // ifndef KExcludeRules_h


// EOF

