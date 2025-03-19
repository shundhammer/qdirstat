/*
 *   File name: QtCompat.h
 *   Summary:	Helpers for Qt4, Qt5, Qt6 compatibility
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef QtCompat_h
#define QtCompat_h

#include <QString>
#include <QComboBox>
#include <QRegExp>

#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))


#include <QTextDocument> // Qt::escape()
#include <QApplication>
#include <QLineEdit>

#define setSectionResizeMode setResizeMode
#define sectionResizeMode    resizeMode

inline QString qHtmlEscape( const QString & text )
{
    return Qt::escape( text );
}


inline QPalette qAppPalette()
{
    return QApplication::palette();
}


inline void qSetComboBoxText( QComboBox *     comboBox,
			      const QString & text )
{
    QLineEdit * lineEdit = comboBox->lineEdit();

    if ( lineEdit )
	lineEdit->setText( text );
}


inline void qEnableClearButton( QComboBox * comboBox )
{
    Q_UNUSED( comboBox );
}


inline void qEnableClearButton( QLineEdit * lineEdit )
{
    Q_UNUSED( lineEdit );
}



#else // Qt >= 5.x


#include <QGuiApplication>
#include <QLineEdit>


inline QString qHtmlEscape( const QString & text )
{
    return text.toHtmlEscaped();
}


inline QPalette qAppPalette()
{
    return QGuiApplication::palette();
}


inline void qSetComboBoxText( QComboBox *     comboBox,
			      const QString & text )
{
    comboBox->setCurrentText( text );
}


inline void qEnableClearButton( QComboBox * comboBox )
{
    QLineEdit * lineEdit = comboBox->lineEdit();

    if ( lineEdit )
	lineEdit->setClearButtonEnabled( true );
}


inline void qEnableClearButton( QLineEdit * lineEdit )
{
    lineEdit->setClearButtonEnabled( true );
}


#endif


// Qt5 and earlier QRegExp exists
#if (QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 ))
    inline QString qregexp_replaceIn(const QRegExp& regexp, const QString& source, const QString& after)
    {
        QString str = source;
        return str.replace( regexp, after );
    }

    inline QString qregexp_removeIn(const QRegExp& regexp, const QString& source)
    {
        QString str = source;
        return str.remove( regexp );
    }

    inline QStringList qregexp_splitString(const QRegExp& regexp, const QString& source, Qt::SplitBehavior behavior = Qt::KeepEmptyParts )
    {
       QString str = source;
       return str.split(regexp, behavior);
    }

    inline bool qregexp_containedIn( const QRegExp& regexp, const QString& source)
    {
        return source.contains(regexp);
    }

    inline int qregexp_countIn( const QRegExp& regexp, const QString& source )
    {
        return source.count( regexp );
    }

    inline QStringList qregexp_filterList( const QRegExp& regexp, const QStringList& source )
    {
        return source.filter( regexp );
    }


//    #define QEnterEvent QEvent

// Qt6, QRegExp is only available in compatibility modes
#else
    inline QString qregexp_replaceIn(const QRegExp& regexp, const QString& source, const QString& after)
    {
        return regexp.replaceIn( source, after);
    }

    inline QString qregexp_removeIn(const QRegExp& regexp, const QString& source)
    {
        return regexp.removeIn( source );
    }

    inline QStringList qregexp_splitString(const QRegExp& regexp, const QString& source, Qt::SplitBehavior behavior = Qt::KeepEmptyParts )
    {
        return regexp.splitString(source, behavior );
    }

    inline bool qregexp_containedIn( const QRegExp& regexp, const QString& source)
    {
        return regexp.containedIn( source );
    }

    inline int qregexp_countIn( const QRegExp& regexp, const QString& source )
    {
        return regexp.countIn(source);
    }

    inline QStringList qregexp_filterList( const QRegExp& regexp, const QStringList& source )
    {
        return regexp.filterList(source);
    }

#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#define ENDL Qt::endl
#define SKIPEMPTYPARTS Qt::SkipEmptyParts
#else
#define ENDL endl
#define SKIPEMPTYPARTS QString::SkipEmptyParts
#endif

#endif // QtCompat_h
