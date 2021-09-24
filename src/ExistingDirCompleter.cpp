/*
 *   File name: ExistingDirCompleter.h
 *   Summary:	QDirStat widget support classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QFileSystemModel>

#include "ExistingDirCompleter.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


ExistingDirCompleter::ExistingDirCompleter( QObject * parent ):
    QCompleter( parent )
{
    QFileSystemModel * model = new QFileSystemModel( this );
    CHECK_NEW( model );

    model->setRootPath( "/" );
    model->setFilter( QDir::Dirs );
    model->setReadOnly( true );

    setModel( model );
}


ExistingDirCompleter::~ExistingDirCompleter()
{
    // NOP
}

