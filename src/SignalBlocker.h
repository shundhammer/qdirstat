/*
 *   File name: SignalBlocker.h
 *   Summary:	Helper class to block undesired Qt signals
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef SignalBlocker_h
#define SignalBlocker_h

#include <QObject>

/**
 * Helper class to block signals for an object while this object exists.
 * The old signal blocking state is automatically restored once the
 * SignalBlocker goes out of scope.
 */
class SignalBlocker
{
public:
    SignalBlocker( QObject *obj ):
	_obj( obj )
    {
	_oldBlocked = obj->signalsBlocked();
	obj->blockSignals( true );
    }

    ~SignalBlocker()
    {
	_obj->blockSignals( _oldBlocked );
    }

private:
    QObject *_obj;
    bool     _oldBlocked;
};

#endif // SignalBlocker_h

