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
         * Constructors
         **/
        PkgInfo( const QString & name,
                 const QString & version,
                 const QString & arch );

        PkgInfo( DirTree *       tree,
                 DirInfo *       parent,
                 const QString & name );

        /**
         * Destructor.
         **/
        virtual ~PkgInfo();

        /**
         * Return the package's base name, i.e. the short name without any
         * version number or architecture information. This may be different
         * from name() if this package is installed in multiple versions or for
         * different architectures. Initially, this starts with a copy of
         * name().
         **/
        const QString & baseName() const { return _baseName; }

        /**
         * Set the (display) name for this package.
         *
         * This is useful if this package is installed in multiple versions or
         * for multiple architectures; in that case, it is advisable to use the
         * base name plus either the version or the architecture or both.
         **/
        void setName( const QString & newName ) { _name = newName; }

        /**
         * Return the version of this package.
         **/
        const QString & version() const { return _version; }

        /**
         * Return the architecture of this package ("x86_64", "i386").
         **/
        const QString & arch() const { return _arch; }

        /**
         * Set the parent DirTree for this pkg.
         **/
        void setTree( DirTree * tree ) { _tree = tree; }

        /**
         * Return 'true' if this package is installed for more than one
         * architecture.
         **/
        bool isMultiArch() const { return _multiArch; }

        /**
         * Set the multiArch flag.
         **/
        void setMultiArch( bool val ) { _multiArch = val; }

        /**
         * Return 'true' if this package is installed in multiple versions
         * (but possibly for only one architecture).
         **/
        bool isMultiVersion() const { return _multiVersion; }

        /**
         * Set the multiVersion flag.
         **/
        void setMultiVersion( bool val ) { _multiVersion = val; }

        /**
         * Returns true if this is a PkgInfo object.
         *
         * Reimplemented - inherited from FileInfo.
         **/
        virtual bool isPkgInfo() const Q_DECL_OVERRIDE
            { return true; }


    protected:

        QString _baseName;
        QString _version;
        QString _arch;

        bool    _multiArch    :1;
        bool    _multiVersion :1;

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
