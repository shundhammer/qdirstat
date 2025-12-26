/*
 *   File name: Translator.cpp
 *   Summary:	Helper class for user message translations
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 *              Donated by the Myrlyn project
 */


#include <string.h>             // strlen()
#include <libintl.h>            // dgettext(), bindtextdomain()
#include "Logger.h"
#include "Translator.h"

#define ENABLE_QT_INTERNAL_MESSAGES     0
#define VERBOSE_QT_INTERNAL_MESSAGES    1


bool Translator::_useFakeTranslations = false;


Translator::Translator( const QString & textdomain, QObject * parent )
    : QTranslator( parent )
    , _textdomain( textdomain.toUtf8() )
{
    // bindtextdomain( textdomain.data(), "/usr/share/locale" );
    // Not needed: /usr/share/locale is the default location.

    // Initialize _fakeTemplate with "xixoxixoxixo..."

    for ( int i=0; i < 10; i++ )
        _fakeTemplate += "xixo";
}


Translator::~Translator()
{
    // NOP
}


QString
Translator::translate( const char * context_str,
                       const char * sourceText,
                       const char * disambiguation,
                       int          nPlural         ) const
{
    Q_UNUSED( context_str );
    Q_UNUSED( disambiguation );
    Q_UNUSED( nPlural );

    if ( _useFakeTranslations )
        return fakeTranslation( sourceText ); // -> "xixoxixo..."


#if ENABLE_QT_INTERNAL_MESSAGES

    QString context( QString::fromUtf8( context_str ) );

    if ( context.startsWith( 'Q' )         // Qt class?
         && ! context.startsWith( "QY2" )  // Our own classes
         && context != "QObject"           // Used for all kinds of things
         && context != "QIODevice" )
    {
#if VERBOSE_QT_INTERNAL_MESSAGES
        logVerbose() << "Qt-internal message: "
                     << "context: " << context
                     << " msg: "    << sourceText
                     << endl;
#endif

        // For libQt-internal texts (file dialogs etc.), use the parent class
        // for Qt's built-in translations.

        return QTranslator::translate( context_str,
                                       sourceText,
                                       disambiguation,
                                       nPlural );
    }
#endif

    return dgettext( _textdomain.data(), sourceText );
}


QString
Translator::fakeTranslation( const char * sourceText ) const
{
    return _fakeTemplate.left( strlen( sourceText ) );
}


void
Translator::useFakeTranslations( bool val )
{
    _useFakeTranslations = val;
}
