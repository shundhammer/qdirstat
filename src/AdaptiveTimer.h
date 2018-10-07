/*
 *   File name: AdaptiveTimer.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef AdaptiveTimer_h
#define AdaptiveTimer_h

#include <QObject>
#include <QVariant>
#include <QTimer>
#include <QList>


typedef QList<int> IntList;


namespace QDirStat
{
    /**
     * Timer for delivering events that each obsolete each previous one, for
     * example for updating a widget's data.
     *
     * If events are delivered only sparingly, this acts like a zero timer,
     * i.e. each event is delivered immediately as soon as Qt returns to its
     * main loop. This causes very little (if any) delay.
     *
     * When things heat up and events are coming in rapidly, this timer
     * increases the delay of event delivery while always only delivering the
     * latest event and discarding all others that would only contain outdated
     * information anyway.
     *
     * The intention behind this is to reduce very expensive operations to a
     * minimum and only show the latest and up-to-date data.
     **/
    class AdaptiveTimer: public QObject
    {
        Q_OBJECT

    public:

        /**
         * Constructor.
         **/
        AdaptiveTimer( QObject * parent = 0 );

        /**
         * Destructor.
         **/
        virtual ~AdaptiveTimer();

        /**
         * Add another stage for increased event delivery delays.
         * If not set, there is only one stage with a zero delay.
         **/
        void addDelayStage( int delayMillisec );

        /**
         * Clear all internal data, including all defined delays and intervals.
         **/
        void clear();

    public slots:

        /**
         * Incoming request with optional user-defined payload. The payload
         * will be delivered in the deliverRequest() signal.
         *
         * If requests arrive very rapidly, only the latest one will be
         * delivered, all others will be discarded.
         **/
        void delayedRequest( const QVariant & payload = QVariant() );

    signals:

        /**
         * Outgoing request with the latest payload from the latest
         * receiveRequest() call.
         **/
        void deliverRequest( const QVariant & payload );

    protected slots:

        /**
         * Timeout for the delivery timer.
         **/
        void deliveryTimeout();

    protected:

        QVariant _payload;
        IntList  _delayStages;
        QTimer   _deliveryTimer;

    }; // class AdaptiveTimer

} // namespace QDirStat

#endif // AdaptiveTimer_h
