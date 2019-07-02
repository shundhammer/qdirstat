/*
 *   File name: Qt4Compat.h
 *   Summary:	Helpers for Qt4 compatibility
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef Qt4Compat_h
#define Qt4Compat_h

#include <QString>
#include <QComboBox>


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


#else // Qt 5.x


#include <QGuiApplication>


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


#endif



#endif // Qt4Compat_h
