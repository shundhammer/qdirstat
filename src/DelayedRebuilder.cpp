/*
 *   File name: DelayedRebuilder.cpp
 *   Summary:	Utility class to handle delayed rebuilding of widgets
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QTimer>

#include "DelayedRebuilder.h"
#include "Logger.h"

#define DefaultRebuildDelayMillisec 200

using namespace QDirStat;


DelayedRebuilder::DelayedRebuilder( QObject * parent ):
    QObject( parent ),
    _firstRebuild( true ),
    _pendingRebuildCount(0),
    _delayMillisec( DefaultRebuildDelayMillisec )
{

}


DelayedRebuilder::~DelayedRebuilder()
{

}


void DelayedRebuilder::scheduleRebuild()
{
    ++_pendingRebuildCount;
    QTimer::singleShot( _delayMillisec, this, SLOT( rebuildDelayed() ) );
}


void DelayedRebuilder::rebuildDelayed()
{
    if ( --_pendingRebuildCount > 0 ) // Yet another rebuild scheduled (by timer)?
        return;                       // -> do nothing, it will be in vain anyway

    _pendingRebuildCount = 0;
    _firstRebuild       = false;
    emit rebuild();
}
