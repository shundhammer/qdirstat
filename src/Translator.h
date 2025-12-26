/*
 *   File name: Translator.h
 *   Summary:	Helper class for user message translations
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 *              Donated by the Myrlyn project
 */


#ifndef Translator_h
#define Translator_h

#include <QTranslator>
#include <QByteArray>

/**
 * Class to manage translations for widget texts and other user-visible
 * messages.
 *
 * This uses Qt mechanisms like Qt::tr(), but it maps them to GNU gettext calls
 * like dgettext(). Since it uses common Qt mechanisms, it also works for Qt
 * .ui files generated with Qt Designer.
 **/
class Translator: public QTranslator
{
    Q_OBJECT

public:

    Translator( const QString & textdomain, QObject * parent = 0 );
    virtual ~Translator();

    /**
     * Reimplemented from QTranslator:
     *
     * Use GNU gettext functions like dgettext() to get the message
     * translations unless a Qt-internal context is detected.
     *
     * Notice that this is also used for Qt's own messages that need to be
     * translated, such as predefined dialogs (file dialog) and context menus
     * (e.g. when right-clicking input fields, lists, scroll bars), so we
     * cannot simply use GNU gettext everywhere.
     **/
    virtual QString translate( const char * context,
                               const char * sourceText,
                               const char * disambiguation = 0,
                               int          nPlural        = -1 ) const override;

    /**
     * Use fake translations: Translate every message with "xixoxixo..." of the
     * same length as the original message. This can be used to check if all
     * user messages are actually marked for translation.
     **/
    static void useFakeTranslations( bool val = true );


protected:

    /**
     * Return a fake translation "xixoxixoxixo" with the same length as
     * 'sourceText'. This can be useful to see where translation markers are
     * missing in the code.
     *
     * Use the '--fake-translations' command line option to enable this.
     *
     **/
    QString fakeTranslation( const char * sourceText ) const;

    //
    // Data members
    //

    QByteArray  _textdomain;   // QByteArray to avoid repeated toUtf8() conversions
    QString     _fakeTemplate;

    static bool _useFakeTranslations;

};      // class Translator


#endif  // Translator_h
