/*
 *   File name: AdaptiveTimer.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "AdaptiveTimer.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


AdaptiveTimer::AdaptiveTimer( QObject * parent ):
    QObject( parent )
{
    _deliveryTimer.setSingleShot( true );
    _deliveryTimer.setInterval( 0 );

    connect( &_deliveryTimer, SIGNAL( timeout()         ),
             this,            SLOT  ( deliveryTimeout() ) );
}


AdaptiveTimer::~AdaptiveTimer()
{
    
}


void AdaptiveTimer::addDelayStage( int delayMillisec )
{
    if ( _delayStages.isEmpty() )
        _deliveryTimer.setInterval( delayMillisec );
        
    _delayStages << delayMillisec;
}


void AdaptiveTimer::clear()
{
    _deliveryTimer.stop();
    _deliveryTimer.setInterval( 0 );
    _delayStages.clear();
}


void AdaptiveTimer::delayedRequest( const QVariant & payload )
{
    logDebug() << "Received request for " << payload.toString() << endl;
    _payload = payload;
    _deliveryTimer.start();
}


void AdaptiveTimer::deliveryTimeout()
{
    logDebug() << "Delivering request for " << _payload.toString() << endl;
    emit deliverRequest( _payload );
}
