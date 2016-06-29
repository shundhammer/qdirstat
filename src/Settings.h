/*
 *   File name: Settings.h
 *   Summary:	Specialized settings classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef Settings_h
#define Settings_h

#include <QSettings>
#include <QStringList>


namespace QDirStat
{
    /**
     * Specialized QSettings subclass for generic settings, i.e. the main
     * config file ~/.config/QDirStat/QDirStat.conf
     *
     * This class takes care of cleaning up leftovers of previous config file
     * formats, for example when certain settings groups (cleanups, mime
     * categories) were moved out from the main config file to specialized
     * config files.
     **/
    class Settings: public QSettings
    {
	Q_OBJECT

    public:
	/**
	 * Create a settings object with the specified name. If 'name' is
	 * empty, the application name is used, i.e. the settings are stored in
	 * the main config file.
	 **/
	Settings( const QString & name = QString() );

	/**
	 * Destructor.
	 **/
	virtual ~Settings();

	/**
	 * Name of this settings object. This returns an empty string for the
	 * generic settings, i.e. the main config file.
	 **/
	const QString & name() const { return _name; }

	/**
	 * Name of the group prefix of this settings object, e.g. "Cleanup_"
	 * for a derived class that uses settings groups [Cleanup_01],
	 * [Cleanup_02] etc.
	 **/
	const QString & groupPrefix() const { return _groupPrefix; }

	/**
	 * Find all settings groups that start with 'groupPrefix'.
	 **/
	QStringList findGroups( const QString & groupPrefix );

	/**
	 * Return true if this settings object has any settings group that
	 * starts with 'groupPrefix'.
	 **/
	bool hasGroup( const QString & groupPrefix );

	/**
	 * Remove all settings groups that start with 'groupPrefix'.
	 **/
	void removeGroups( const QString & groupPrefix );

	/**
	 * Go to the settings top level
	 **/
	void ensureToplevel();


    protected:

	/**
	 * Migrate (move) all settings groups starting with 'groupPrefix' from
	 * settings object 'from' to settings object 'to'.
	 **/
	void migrate( const QString & groupPrefix, Settings & from, Settings & to );

	/**
	 * Migrate settings of the generic settings (the main config file) to
	 * this one. Derived classes can reimplement this to make sure no
	 * duplicates of their separate config file remain in the main config
	 * file.
	 *
	 * This default implementation does nothing.
	 **/
	virtual void migrate() {}


	// Data members

	QString _name;
	QString _groupPrefix;
    };


    /**
     * Specialized settings class for cleanup actions.
     *
     * The general idea is that those settings are stored in a separate file so
     * that entire file can easily replaced by a site administrator.
     **/
    class CleanupSettings: public Settings
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	CleanupSettings();

	/**
	 * Destructor.
	 **/
	virtual ~CleanupSettings();

    protected:
	/**
	 * Migrate settings of the generic settings (the main config file) to
	 * this one. Reimplemented from Settings.
	 **/
	virtual void migrate() Q_DECL_OVERRIDE;
    };


    /**
     * Specialized settings class for MIME categories.
     *
     * The general idea is that those settings are stored in a separate file so
     * that entire file can easily replaced by a site administrator.
     **/
    class MimeCategorySettings: public Settings
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	MimeCategorySettings();

	/**
	 * Destructor.
	 **/
	virtual ~MimeCategorySettings();

    protected:
	/**
	 * Migrate settings of the generic settings (the main config file) to
	 * this one. Reimplemented from Settings.
	 **/
	virtual void migrate() Q_DECL_OVERRIDE;
    };

}	// namespace QDirStat

#endif	// Settings_h
