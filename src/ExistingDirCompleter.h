/*
 *   File name: ExistingDirCompleter.h
 *   Summary:   QDirStat widget support classes
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:    Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef ExistingDirCompleter_h
#define ExistingDirCompleter_h


#include <QCompleter>


/**
 * Completer class for QCombobox and related to complete names of existing
 * directories.
 *
 * See ShowUnpkgFilesDialog for a usage example.
 **/
class ExistingDirCompleter: public QCompleter
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    ExistingDirCompleter( QObject * parent );

    /**
     * Destructor.
     **/
    virtual ~ExistingDirCompleter();

};  // class ExistingDirCompleter


#endif  // ExistingDirCompleter_h
