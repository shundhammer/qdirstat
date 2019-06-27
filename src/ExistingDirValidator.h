/*
 *   File name: ExistingDirValidator.h
 *   Summary:	QDirStat widget support classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef ExistingDirValidator_h
#define ExistingDirValidator_h


#include <QValidator>


namespace QDirStat
{
    /**
     * Validator class for QCombobox and related to validate names of existing
     * directories.
     *
     * See ShowUnpkgFilesDialog for a usage example.
     **/
    class ExistingDirValidator: public QValidator
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	ExistingDirValidator( QObject * parent );

	/**
	 * Destructor.
	 **/
	virtual ~ExistingDirValidator();

	/**
	 * Validate the input string.
	 **/
	QValidator::State validate( QString & input, int & pos) const Q_DECL_OVERRIDE;


    signals:

	void isOk( bool ok );

    };	// class ExistingDirValidator

}	// namespace QDirStat

#endif	// ExistingDirValidator_h
