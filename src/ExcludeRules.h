/*
 *   File name: ExcludeRules.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef ExcludeRules_h
#define ExcludeRules_h

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QList>
#include <QTextStream>

#include "ListMover.h"


namespace QDirStat
{
    class DirInfo;

    /**
     * One single exclude rule to check text (file names) against.
     **/
    class ExcludeRule
    {
    public:

	/**
	 * Constructor from a QRegExp.
	 *
	 * 'useFullPath' indicates if this exclude rule uses the full path
	 * ('true') or only the file name without path ('false') for matching.
         *
         * 'checkAnyFileChild' specifies whether the non-directory children of
         * the directory should be used for matching rather than the path or
         * name of the directory itself. That makes it possible to exclude a
         * directory that contains a file ".nobackup".
	 **/
	ExcludeRule( const QRegExp & regexp,
                     bool            useFullPath       = false,
                     bool            checkAnyFileChild = false );

	/**
	 * Constructor from a QString. The string will be used as a regexp.
	 *
	 * 'useFullPath' indicates if this exclude rule uses the full path
	 * ('true') or only the file name without path ('false') for matching.
         *
         * 'checkAnyFileChild' specifies whether the non-directory children of
         * the directory should be used for matching rather than the path or
         * name of the directory itself. That makes it possible to exclude a
         * directory that contains a file ".nobackup".
	 **/
	ExcludeRule( const QString & regexp,
                     bool            useFullPath       = false,
                     bool            checkAnyFileChild = false );

	/**
	 * Destructor.
	 **/
	virtual ~ExcludeRule();

	/**
	 * Check a file name with or without its full path against this exclude
	 * rule: If useFullPath() is 'true', the 'fullPath' parameter is used
	 * for matching, if it is 'false', 'fileName' is used.
	 *
	 * Returns 'true' if the string matches, i.e. the file should be
	 * excluded.
	 **/
	bool match( const QString & fullPath, const QString & fileName );

        /**
         * If this exclude rule has the 'checkAnyFileChild' flag set, check if
         * any non-directory direct child of 'dir' (or of its dot entry if it
         * has one) matches the rule.
         *
         * This returns 'false' immediately if 'checkAnyFileChild' is not set.
         **/
        bool matchDirectChildren( DirInfo * dir );

	/**
	 * Returns this rule's regular expression.
	 **/
	const QRegExp & regexp() const { return _regexp; }

	/**
	 * Change this rule's regular expression.
	 **/
	void setRegexp( const QRegExp & regexp ) { _regexp = regexp; }

	/**
	 * Return 'true' if this exclude rule uses the full path to match
	 * against, 'false' if it only uses the file name without path.
	 **/
	bool useFullPath() const { return _useFullPath; }

	/**
	 * Set the 'full path' flag.
	 **/
	void setUseFullPath( bool useFullPath ) { _useFullPath = useFullPath; }

        /**
         * Return 'true' if this exclude rule should be used to check against
         * any direct non-directory child of a directory rather than just the
         * directory name or path.
         **/
        bool checkAnyFileChild() const { return _checkAnyFileChild; }

        /**
         * Set the 'check any file child' flag.
         **/
        void setCheckAnyFileChild( bool check ) { _checkAnyFileChild = check; }

    private:

	QRegExp _regexp;
	bool	_useFullPath;
        bool    _checkAnyFileChild;
    };


    typedef QList<ExcludeRule *> ExcludeRuleList;
    typedef ExcludeRuleList::const_iterator ExcludeRuleListIterator;


    /**
     * Container for multiple exclude rules. This can be used as a singleton
     * class. Use the static methods or instance() to access the singleton.
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
    class ExcludeRules: public QObject
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 *
	 * Most applications will want to use instance() instead to create
	 * and use a singleton object of this class.
	 **/
	ExcludeRules();

        /**
         * Constructor from a QStringList: Create and add an exclude rule
         * matching a full path for each path in the list with match mode
         * "wildcard".
         **/
        ExcludeRules( const QStringList & paths );

	/**
	 * Destructor.
	 **/
	virtual ~ExcludeRules();

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
	 * Remove an exclude rule from this rule set and delete it.
	 **/
	void remove( ExcludeRule * rule );

	/**
	 * Create a new rule with 'regexp', 'useFullPath' and
	 * 'checkAnyFileChild' and add it to this rule set.
	 **/
	static void add( const QRegExp & regexp,
                         bool            useFullPath       = false,
                         bool            checkAnyFileChild = false );

	static void add( const QString & regexp,
                         bool            useFullPath       = false,
                         bool            checkAnyFileChild = false );

	/**
	 * Check a file name against the exclude rules. Each exclude rule
	 * decides individually based on its configuration if it checks against
	 * the full path or against the file name without path, so both have to
	 * be provided here.
	 *
	 * This will return 'true' if the text matches any rule.
	 *
	 * Note that this operation will move current().
	 **/
	bool match( const QString & fullPath, const QString & fileName );

        /**
         * Check the direct non-directory children of 'dir' against any rules
         * that have the 'checkAnyFileChild' flag set.
         *
	 * This will return 'true' if the text matches any rule.
         **/
        bool matchDirectChildren( DirInfo * dir );

	/**
	 * Find the exclude rule that matches 'text'.
	 * Return 0 if there is no match.
	 *
	 * This is intended to explain to the user which rule matched.
	 **/
	const ExcludeRule * matchingRule( const QString & fullPath,
					  const QString & fileName );

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

    public slots:

	/**
	 * Move an exclude rule one position up in the list.
	 **/
	void moveUp( ExcludeRule * rule );

	/**
	 * Move an exclude rule one position down in the list.
	 **/
	void moveDown( ExcludeRule * rule );

	/**
	 * Move an exclude rule to the top of the list.
	 **/
	void moveToTop( ExcludeRule * rule );

	/**
	 * Move an exclude rule to the bottom of the list.
	 **/
	void moveToBottom( ExcludeRule * rule );

	/**
	 * Clear all existing exclude rules and read exclude rules from the
	 * settings file.
	 **/
	void readSettings();

	/**
	 * Write all exclude rules to the settings file.
	 **/
	void writeSettings();

    protected:

        /**
         * Add some default rules.
         **/
        void addDefaultRules();

    private:

	ExcludeRuleList		 _rules;
	ListMover<ExcludeRule *> _listMover;
	ExcludeRule *		 _lastMatchingRule;
        bool                     _defaultRulesAdded;
    };


    /**
     * Print the regexp of a FileInfo in a debug stream.
     **/
    inline QTextStream & operator<< ( QTextStream & stream, const ExcludeRule * rule )
    {
	if ( rule )
	    stream << "<ExcludeRule \"" << rule->regexp().pattern() << "\""
                   << ( rule->useFullPath() ? " (full path)" : "" )
                   << ">";
	else
	    stream << "<NULL ExcludeRule *>";

	return stream;
    }

}	// namespace QDirStat


#endif // ifndef ExcludeRules_h

