/*
 *   File name: DelayedRebuilder.h
 *   Summary:	Utility class to handle delayed rebuilding of widgets
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DelayedRebuilder_h
#define DelayedRebuilder_h


#include <QObject>


namespace QDirStat
{
    /**
     * This is a utility class to handle delayed rebuilding for widgets:
     *
     * Rebuilds are not executed right away, but delayed for a few milliseconds
     * in case another rebuild is requested immediately. This is useful for
     * resize events to prevent doing expensive widget rebuilds more often than
     * necessary.
     **/
    class DelayedRebuilder: public QObject
    {
        Q_OBJECT

    public:
        /**
         * Constructor.
         **/
        DelayedRebuilder( QObject * parent = 0 );

        /**
         * Destructor.
         **/
        virtual ~DelayedRebuilder();

        /**
         * Return 'true' if this is the first rebuild ever.
         **/
        bool firstRebuild() const { return _firstRebuild; }

        /**
         * Return the number of pending rebuilds.
         **/
        int pendingRebuildCount() const { return _pendingRebuildCount; }

        /**
         * Change the default 200 millisec delay to a new value.
         **/
        void setDelay( int delayMillisec ) { _delayMillisec = delayMillisec; }

    public slots:

        /**
         * Schedule a rebuild after the timeout is over. Repeated calls to this
         * while the timeout is still running will result in all scheduled
         * rebuilds except the last one to be ignored.
         **/
        void scheduleRebuild();

    signals:
        /**
         * Emitted when the rebuild should really be done: When the timeout is
         * over and there are no more other pending rebuilds scheduled.
         **/
        void rebuild();

    protected slots:

        /**
         * Slot that is called when the timeout is over: This will emit a
         * rebuild() signal if there are no more pending rebuilds.
         **/
        void rebuildDelayed();

    protected:

        bool _firstRebuild;
        int  _pendingRebuildCount;
        int  _delayMillisec;
    };

}	// namespace QDirStat


#endif // ifndef DelayedRebuilder_h
