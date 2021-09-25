/*
 *   File name: FormatUtil.h
 *   Summary:	String formatting utilities for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef FormatUtil_h
#define FormatUtil_h


#include <sys/types.h>
#include <sys/stat.h>

#include <QString>
#include <QTextStream>

#include "FileSize.h"


namespace QDirStat
{
    /**
     * Format a file / subtree size human readable, i.e. in "GB" / "MB"
     * etc. rather than huge numbers of digits. 'precision' is the number of
     * digits after the decimal point.
     *
     * Note: For logDebug() etc., operator<< is overwritten to do exactly that:
     *
     *	   logDebug() << "Size: " << x->totalSize() << endl;
     **/
    QString formatSize( FileSize size );

    // Can't use a default argument when using this as a function pointer,
    // so we really need the above overloaded version.
    QString formatSize( FileSize size, int precision );

    /**
     * Format a file / subtree size as bytes, but still human readable with a
     * space as a thousands separator, i.e. "12 345 678 Bytes".
     *
     * Intentionally NOT using the locale's thousands separator since this
     * causes confusion to no end when it's only one of them, and it's unclear
     * what locale is used: German uses "," as the decimal separator and "." as
     * the thousands separator, exactly the other way round as English. So it's
     * never clear if 12.345 is a little more than twelve or twelve thousand.
     * A space character avoids this confusion.
     **/
    QString formatByteSize( FileSize size );

    /**
     * Format a timestamp (like the latestMTime()) human-readable.
     **/
    QString formatTime( time_t rawTime );

    /**
     * Format a millisecond-based time
     **/
    QString formatMillisec( qint64 millisec, bool showMillisec = true );

    /**
     * Format a percentage.
     **/
    QString formatPercent( float percent );

    /**
     * Format the mode (the permissions bits) returned from the stat() system
     * call in the commonly used formats, both symbolic and octal, e.g.
     *	   drwxr-xr-x  0755
     **/
    QString formatPermissions( mode_t mode );

    /**
     * Format a number in octal with a leading zero.
     **/
    QString formatOctal( int number );

    /**
     * Return the mode (the permission bits) returned from stat() like the
     * "ls -l" shell command does, e.g.
     *
     *	   drwxr-xr-x
     *
     * 'omitTypeForRegularFiles' specifies if the leading "-" should be omitted.
     **/
    QString symbolicMode( mode_t perm, bool omitTypeForRegularFiles = false );

    /**
     * Format the filesystem object type from a mode, e.g. "Directory",
     * "Symbolic Link", "Block Device", "File".
     **/
    QString formatFilesystemObjectType( mode_t mode );


    /**
     * Human-readable output of a file size in a debug stream.
     **/
    inline QTextStream & operator<< ( QTextStream & stream, FileSize lSize )
    {
	stream << formatSize( lSize );

	return stream;
    }

}       // namespace QDirStat

#endif  // FormatUtil_h
