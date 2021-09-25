/*
 *   File name: UnpkgSettings.h
 *   Summary:	Parameters for "unpackaged files" view
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef UnpkgSettings_h
#define UnpkgSettings_h


#include <QString>
#include <QStringList>
#include <QTextStream>


namespace QDirStat
{
    /**
     * Settings for the "unpackages files" view
     **/
    class UnpkgSettings
    {
    public:

        enum InitPolicy
        {
            ReadFromConfig,     // Read from config file
            DefaultValues,      // Use predefined default values
            Empty               // Use empty strings / string lists
        };

        /**
         * Constructor.
         **/
        UnpkgSettings( InitPolicy initPolicy );

        //
        // Data members (all public!)
        //

        QString         startingDir;
        QStringList     excludeDirs;
        QStringList     ignorePatterns;

	/**
	 * Read settings from the config file
	 **/
	void read();

	/**
	 * Write settings to the config file
	 **/
	void write();

        /**
         * Dump the values to the log
         **/
        void dump() const;

        /**
         * Return the default starting directory.
         **/
        static QString defaultStartingDir();

	/**
	 * Return the default exclude directories.
	 **/
	static QStringList defaultExcludeDirs();

	/**
	 * Return the default ignore patterns.
	 **/
	static QStringList defaultIgnorePatterns();

    };  // UnpkgSettings

}       // namespace QDirStat

#endif  // UnpkgSettings_h
