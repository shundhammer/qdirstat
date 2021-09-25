/*
 *   File name: ExistingDirValidator.h
 *   Summary:	QDirStat widget support classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QDir>

#include "ExistingDirValidator.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


ExistingDirValidator::ExistingDirValidator( QObject * parent ):
    QValidator( parent )
{
}


ExistingDirValidator::~ExistingDirValidator()
{
    // NOP
}


QValidator::State
ExistingDirValidator::validate( QString & input, int & pos) const
{
    Q_UNUSED( pos );

    bool ok = ! input.isEmpty() && QDir( input ).exists();

    // This is a complex way to do
    //    emit isOk( ok );
    // from this const method

    const_cast<ExistingDirValidator *>( this )->isOk( ok );

#if 0
    logDebug() << "Checking \"" << input << "\": "
               << ( ok ? "OK" : "no such directory" )
               << endl;
#endif

    return ok ? QValidator::Acceptable : QValidator::Intermediate;
}
