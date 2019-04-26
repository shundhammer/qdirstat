/*
 *   File name: PkgInfo.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef PkgInfo_h
#define PkgInfo_h

#include <QList>

#include "DirInfo.h"


namespace QDirStat
{
    class DirTree;

    /**
     * Information about one (installed) package.
     **/
    class PkgInfo: public DirInfo
    {
    public:

        /**
         * Constructor.
         **/
        PkgInfo( const QString & name,
                 const QString & version,
                 const QString & arch    );

        /**
         * Destructor.
         **/
        virtual ~PkgInfo();

        /**
         * Return the version of this package.
         **/
        const QString & version() const { return _version; }

        /**
         * Return the architecture of this package ("x86_64", "i386").
         **/
        const QString & arch() const { return _arch; }

    protected:
        QString _version;
        QString _arch;

    };  // class PkgInfo


    typedef QList<PkgInfo *> PkgInfoList;


    /**
     * Print the debugUrl() of a PkgInfo in a debug stream.
     **/
    inline QTextStream & operator<< ( QTextStream & stream, const PkgInfo * info )
    {
	if ( info )
	{
	    if ( info->checkMagicNumber() )
		stream << "<Pkg " << info->debugUrl() << ">";
	    else
		stream << "<INVALID PkgInfo *>";
	}
	else
	    stream << "<NULL PkgInfo *>";

	return stream;
    }

}       // namespace QDirStat


#endif // ifndef PkgInfo_h
