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


#define DEFAULT_COOLDOWN_PERIOD 3000 // millisec
#define VERBOSE_STAGES          0
#define VERBOSE_DELAY           1


using namespace QDirStat;


AdaptiveTimer::AdaptiveTimer( QObject * parent ):
    QObject( parent )
{
    clear();

    connect( &_deliveryTimer, SIGNAL( timeout()         ),
             this,            SLOT  ( deliveryTimeout() ) );

    connect( &_coolDownTimer, SIGNAL( timeout()  ),
             this,            SLOT  ( coolDown() ) );
}


AdaptiveTimer::~AdaptiveTimer()
{

}


void AdaptiveTimer::addDelayStage( int delayMillisec )
{
    if ( _delays.isEmpty() )
        _deliveryTimer.setInterval( delayMillisec );

    _delays << delayMillisec;
}


void AdaptiveTimer::addCoolDownPeriod( int coolDownMillisec )
{
    if ( _coolDownPeriods.isEmpty() )
        _coolDownTimer.setInterval( coolDownMillisec );

    _coolDownPeriods << coolDownMillisec;
}


void AdaptiveTimer::clear()
{
    _deliveryTimer.stop();
    _deliveryTimer.setSingleShot( true );
    _deliveryTimer.setInterval( 0 );

    _coolDownTimer.stop();
    _coolDownTimer.setSingleShot( true );
    _coolDownTimer.setInterval( DEFAULT_COOLDOWN_PERIOD ); // millisec

    _delays.clear();
    _coolDownPeriods.clear();

    _delayStage    = 0;
    _coolDownStage = 0;
}


void AdaptiveTimer::delayedRequest( const QVariant & payload )
{
    // logDebug() << "Received request for " << payload.toString() << endl;
    _payload = payload;

    if ( _coolDownTimer.isActive() )
        increaseDelay();
    else
        _coolDownTimer.start();

    _deliveryTimer.start();
}


void AdaptiveTimer::deliveryTimeout()
{
    // logDebug() << "Delivering request for " << _payload.toString() << endl;
    emit deliverRequest( _payload );
}


void AdaptiveTimer::increaseDelay()
{
    if ( _delayStage < _delays.size () - 1 )
    {
        int delay = _delays[ ++_delayStage ];

#if VERBOSE_DELAY
        logDebug() << "Increasing delay to stage " << _delayStage
                   << ": " << delay << " millisec"
                   << endl;
#endif

        _deliveryTimer.setInterval( delay );
    }

    heatUp();
}


void AdaptiveTimer::decreaseDelay()
{
    if ( _delayStage > 0 )
    {
        int delay = _delays[ --_delayStage ];

#if VERBOSE_DELAY
        logDebug() << "Decreasing delay to stage " << _delayStage
                   << ": " << delay << " millisec"
                   << endl;
#endif

        _deliveryTimer.setInterval( delay );
    }

    if ( _delayStage > 0 )
        _coolDownTimer.start();
}


void AdaptiveTimer::heatUp()
{
    // logDebug() << "Heating up" << endl;

    if ( _coolDownStage < _coolDownPeriods.size() - 1 )
    {
        int coolDownInterval = _coolDownStage < _coolDownPeriods.size() ?
                                                _coolDownPeriods[ ++_coolDownStage ] : DEFAULT_COOLDOWN_PERIOD;

#if VERBOSE_STAGES
        logDebug() << "Heating up to cooldown stage " << _coolDownStage
                   << ": " << coolDownInterval << " millisec"
                   << endl;
#endif

        _coolDownTimer.setInterval( coolDownInterval );
    }

    _coolDownTimer.start();
}


void AdaptiveTimer::coolDown()
{
    // logDebug() << "Cooling down" << endl;

    if ( _coolDownStage > 0 )
    {
        --_coolDownStage;

        int coolDownInterval = _coolDownStage < _coolDownPeriods.size() ?
                                                _coolDownPeriods[ _coolDownStage ] : DEFAULT_COOLDOWN_PERIOD;

#if VERBOSE_STAGES
        logDebug() << "Cooling down to stage " << _coolDownStage
                   << ": " << coolDownInterval << " millisec"
                   << endl;
#endif

        _coolDownTimer.setInterval( coolDownInterval );
    }

    decreaseDelay();
}
