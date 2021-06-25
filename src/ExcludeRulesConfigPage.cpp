/*
 *   File name: ExcludeRulesConfigPage.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "ExcludeRulesConfigPage.h"
#include "Logger.h"
#include "Exception.h"


// This is a mess that became necessary because Qt's moc cannot handle template
// classes. Yes, this is ugly.
#define EXCLUDE_RULE_CAST(VOID_PTR) (static_cast<ExcludeRule *>(VOID_PTR))

using namespace QDirStat;


ExcludeRulesConfigPage::ExcludeRulesConfigPage( QWidget * parent ):
    ListEditor( parent ),
    _ui( new Ui::ExcludeRulesConfigPage )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );
    setListWidget( _ui->listWidget );

    setMoveUpButton	 ( _ui->moveUpButton	   );
    setMoveDownButton	 ( _ui->moveDownButton	   );
    setMoveToTopButton	 ( _ui->moveToTopButton	   );
    setMoveToBottomButton( _ui->moveToBottomButton );
    setAddButton	 ( _ui->addButton	   );
    setRemoveButton	 ( _ui->removeButton	   );

    enableEditRuleWidgets( false );

    connect( _ui->patternLineEdit, SIGNAL( textChanged	 ( QString ) ),
	     this,		   SLOT	 ( patternChanged( QString ) ) );
}


ExcludeRulesConfigPage::~ExcludeRulesConfigPage()
{
    // logDebug() << "ExcludeRulesConfigPage destructor" << endl;
    delete _ui;
}


void ExcludeRulesConfigPage::setup()
{
    fillListWidget();
    updateActions();
}


void ExcludeRulesConfigPage::applyChanges()
{
    // logDebug() << endl;

    save( value( listWidget()->currentItem() ) );
    ExcludeRules::instance()->writeSettings();
}


void ExcludeRulesConfigPage::discardChanges()
{
    // logDebug() << endl;

    listWidget()->clear();
    ExcludeRules::instance()->clear();
    ExcludeRules::instance()->readSettings();
}


void ExcludeRulesConfigPage::fillListWidget()
{
    listWidget()->clear();

    for ( ExcludeRuleListIterator it = ExcludeRules::instance()->begin();
	  it != ExcludeRules::instance()->end();
	  ++it )
    {
	QListWidgetItem * item = new ListEditorItem( (*it)->regexp().pattern(), (*it) );
	CHECK_NEW( item );
	listWidget()->addItem( item );
    }

    QListWidgetItem * firstItem = listWidget()->item(0);

    if ( firstItem )
	listWidget()->setCurrentItem( firstItem );
}


void ExcludeRulesConfigPage::patternChanged( const QString & newPattern )
{
    QListWidgetItem * currentItem = listWidget()->currentItem();

    if ( currentItem )
    {
	ExcludeRule * excludeRule = EXCLUDE_RULE_CAST( value( currentItem ) );
	QRegExp regexp = excludeRule->regexp();
	regexp.setPattern( newPattern );
	excludeRule->setRegexp( regexp );
	currentItem->setText( excludeRule->regexp().pattern() );
    }
}


void ExcludeRulesConfigPage::enableEditRuleWidgets( bool enable )
{
    _ui->rightColumnWidget->setEnabled( enable );
}


void ExcludeRulesConfigPage::save( void * value )
{
    ExcludeRule * excludeRule = EXCLUDE_RULE_CAST( value );
    // logDebug() << excludeRule << endl;

    if ( ! excludeRule || updatesLocked() )
	return;

    QRegExp regexp;

    if ( _ui->regexpRadioButton->isChecked() )
	regexp.setPatternSyntax( QRegExp::RegExp );
    else if ( _ui->wildcardsRadioButton->isChecked() )
	regexp.setPatternSyntax( QRegExp::Wildcard );
    else if ( _ui->fixedStringRadioButton->isChecked() )
	regexp.setPatternSyntax( QRegExp::FixedString );

    regexp.setPattern( _ui->patternLineEdit->text() );

    excludeRule->setRegexp( regexp );
    excludeRule->setUseFullPath( _ui->fullPathRadioButton->isChecked() );
    excludeRule->setCheckAnyFileChild( _ui->checkAnyFileChildRadioButton->isChecked() );
}


void ExcludeRulesConfigPage::load( void * value )
{
    ExcludeRule * excludeRule = EXCLUDE_RULE_CAST( value );
    // logDebug() << excludeRule << endl;

    if ( updatesLocked() )
        return;

    if ( ! excludeRule )
    {
        enableEditRuleWidgets( false );
        _ui->patternLineEdit->setText( "" );

	return;
    }

    enableEditRuleWidgets( true );
    _ui->patternLineEdit->setText( excludeRule->regexp().pattern() );

    switch ( excludeRule->regexp().patternSyntax() )
    {
	case QRegExp::RegExp:	   _ui->regexpRadioButton->setChecked( true );
	    break;
	case QRegExp::Wildcard:	   _ui->wildcardsRadioButton->setChecked( true );
	    break;
	case QRegExp::FixedString: _ui->fixedStringRadioButton->setChecked( true );
	    break;

	default:
	    break;
    }

    if ( excludeRule->useFullPath() )
	_ui->fullPathRadioButton->setChecked( true );
    else
	_ui->dirNameWithoutPathRadioButton->setChecked( true );

    if ( excludeRule->checkAnyFileChild() )
        _ui->checkAnyFileChildRadioButton->setChecked( true );
}


void * ExcludeRulesConfigPage::createValue()
{
    QRegExp regExp( "", Qt::CaseSensitive, QRegExp::Wildcard );
    ExcludeRule * excludeRule = new ExcludeRule( regExp );
    CHECK_NEW( excludeRule );
    ExcludeRules::instance()->add( excludeRule );

    return excludeRule;
}


void ExcludeRulesConfigPage::removeValue( void * value )
{
    ExcludeRule * excludeRule = EXCLUDE_RULE_CAST( value );
    CHECK_PTR( excludeRule );

    ExcludeRules::instance()->remove( excludeRule );
}


QString ExcludeRulesConfigPage::valueText( void * value )
{
    ExcludeRule * excludeRule = EXCLUDE_RULE_CAST( value );
    CHECK_PTR( excludeRule );

    return excludeRule->regexp().pattern();
}


QString ExcludeRulesConfigPage::deleteConfirmationMessage( void * value )
{
    ExcludeRule * excludeRule = EXCLUDE_RULE_CAST( value );
    return tr( "Really delete exclude rule \"%1\"?" ).arg( excludeRule->regexp().pattern() );
}


void ExcludeRulesConfigPage::moveValue( void * value, const char * operation )
{
    ExcludeRule * excludeRule = EXCLUDE_RULE_CAST( value );

    QMetaObject::invokeMethod( ExcludeRules::instance(),
			       operation,
			       Qt::DirectConnection,
			       Q_ARG( ExcludeRule *, excludeRule ) );
}
