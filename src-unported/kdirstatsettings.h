/*
 *   File name:	kdirstatsettings.h
 *   Summary:	Settings dialog for QDirStat
 *   License:   GPL V2 - See file LICENSE for details.
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 *
 */


#ifndef QDirStatSettings_h
#define QDirStatSettings_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <qlistbox.h>
#include <kdialogbase.h>
#include "kcleanup.h"
#include "kcleanupcollection.h"
#include "kdirstatapp.h"


class QCheckBox;
class QComboBox;
class QHGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSlider;
class QSpinBox;
class QVGroupBox;
class QWidget;
class QListView;
class QPopupMenu;

class KColorButton;


#define QDirStatSettingsMaxColorButton	12


namespace QDirStat
{
    class CleanupListBox;
    class CleanupPropertiesPage;
    class DirTreeView;
    class KTreemapView;


    /**
     * Settings dialog for QDirStat
     *
     * @short Settings dialog for QDirStat
     **/
    class SettingsDialog: public KDialogBase
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 *
	 * Notice there is no parent widget passed but the application's main
	 * window so its functions can be accessed. The parent of this widget
	 * is always 0 since this is a dialog.
	 **/

	SettingsDialog( QDirStatApp * mainWin );

	/**
	 * Destructor.
	 **/
	virtual ~SettingsDialog();


	/**
	 * Overwritten from @ref QDialog() to get any chance to set up the
	 * dialog contents when the dialog gets shown - every time, not just at
	 * program startup when the settings dialog is created (!).
	 *
	 * QTabDialog used to have 'aboutToShow()' for a good reason, but the
	 * creators of @ref KDialogBase in their infinite wisdom chose not to
	 * include anything similar. How is that supposed to work, anyway?
	 * Everything I saw in any other KDE sources looked to me like ugly
	 * hacks to work around this. Am I really supposed to destroy my
	 * settings dialog and create a new one every time it pops up? This can
	 * certainly not be the way to go.
	 *
	 * This overwritten show() method sends that @ref aboutToShow() signal
	 * before calling the parent class show() method.
	 **/
	virtual void show();


    public slots:

        /**
	 * Reimplemented from @ref KDialogBase to ask for confirmation.
	 * Emits signal @ref defaultClicked() when the user confirms.
	 **/
        virtual void slotDefault();

        /**
	 * Reimplemented from @ref KDialogBase to set the appropriate help
	 * topic prior to invoking online help.
	 **/
        virtual void slotHelp();


    signals:

	/**
	 * Emitted when (you might have guessed it) the dialog is about to be
	 * shown. Connect this to slots that fill the individual dialog pages'
	 * widgets contents (input fields etc.)
	 **/
	void aboutToShow();

    protected:

	QDirStatApp *	_mainWin;
	int		_cleanupsPageIndex;
	int		_treeColorsPageIndex;
	int		_treemapPageIndex;
	int		_generalSettingsPageIndex;

    }; // class SettingsDialog



    /**
     * Abstract base class for all settings pages. Contains stubs for methods
     * that all settings pages have in common: setup(), apply(),
     * revertToDefaults().
     *
     * Note: This class contains pure virtuals - it cannot be
     * instantiated. Rather, derive your own classes from this one.
     **/
    class SettingsPage: public QWidget
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 *
	 * Sets up standard connections to the methods defined in this class,
	 * e.g., apply(), setup(), revertToDefaults().
	 **/
	SettingsPage( SettingsDialog *	dialog,
		       QWidget *		parent );

	/**
	 * Destructor.
	 **/
	virtual ~SettingsPage();


    public slots:

	/**
	 * Apply the changes.
	 *
	 * Derived classes need to reimplement this method.
	 **/
        virtual void apply() = 0;

	/**
	 * Revert all values to their defaults.
	 *
	 * Derived classes need to reimplement this method.
	 **/
	virtual void revertToDefaults() = 0;

	/**
	 * Set up all fields prior to displaying the dialog.
	 *
	 * Derived classes need to reimplement this method.
	 **/
	virtual void setup() = 0;


    public:

	/**
	 * Returns the page index of this page.
	 * This seems to be the only way to find out which settings page is in
	 * the foreground for a @ref KDialogBase page.
	 **/
	int pageIndex() { return _pageIndex; }

    protected:

	int _pageIndex;

    }; // class SettingsPage



    /**
     * Settings tab page for the tree colors.
     *
     * Uses a vertical slider on the left side and a column of color
     * selection buttons on the right side. The slider enables/disables
     * the color buttons from top to bottom (at least one button is always
     * enabled). Each button represents the percentage fill color of one
     * directory level within the tree. When the tree widget runs out of
     * colors (i.e. there are more directory levels than different
     * colors), it will wrap around to the first color.
     *
     * @short settings page for tree colors
     * @author Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
     **/
    class TreeColorsPage: public SettingsPage
    {
	Q_OBJECT

    public:

	/**
	 * Constructor
	 **/
	TreeColorsPage( SettingsDialog *	dialog,
			 QWidget *		parent,
			 QDirStatApp *		mainWin );

	/**
	 * Destructor
	 **/
	virtual ~TreeColorsPage();


    public slots:

	/**
	 * Apply the changes.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
        virtual void apply();

	/**
	 * Revert all values to their defaults.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
	virtual void revertToDefaults();

	/**
	 * Set up all fields prior to displaying the dialog.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
	virtual void setup();


    protected slots:

	/**
	 * Enable all colors up to color no. 'maxColors'.
	 **/
	void enableColors( int maxColors );


    protected:


	QDirStatApp *		_mainWin;
	DirTreeView *		_treeView;
	QSlider *		_slider;
	KColorButton *		_colorButton [ QDirStatSettingsMaxColorButton ];
	QLabel *		_colorLabel  [ QDirStatSettingsMaxColorButton ];

	int			_maxButtons;

    }; // class TreeColorsPage



    /**
     * Settings tab page for cleanup actions.
     *
     * Uses a CleanupListBox for selection of one cleanup action and a
     * CleanupPropertiesPage for editing this cleanup action's
     * properties. This class handles just the switching between the individual
     * cleanups. It copies the cleanup actions inserted and works with the
     * copies only until it is requested to save the changes or revert all
     * values to their defaults.
     *
     * @short settings page for cleanup actions
     **/
    class CleanupPage: public SettingsPage
    {
	Q_OBJECT

    public:

	/**
	 * Constructor
	 **/
	CleanupPage( SettingsDialog *	dialog,
		      QWidget *		parent,
		      QDirStatApp *	mainWin );

	/**
	 * Destructor
	 **/
	virtual ~CleanupPage();

	/**
	 * Insert an entry for a cleanup action. This is the original value
	 * that will be changed only when receiving the apply() or
	 * defaultValues() signals.
	 **/
	void insert( Cleanup *cleanup );

	/**
	 * Import all cleanup actions from the originals (from the main
	 * window) to internal working copies.
	 **/
	void importCleanups();

	/**
	 * Copy the internal working copies of the cleanup actions back to
	 * the main window's originals. Take care of pending changes within
	 * the current properties page's fields prior to that.
	 **/
	void exportCleanups();


    public slots:

	/**
	 * Apply the changes.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
        virtual void apply();

	/**
	 * Revert all values to their defaults.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
	virtual void revertToDefaults();

	/**
	 * Set up all fields prior to displaying the dialog.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
	virtual void setup();

	/**
	 * Switch back and forth between all the cleanup actions very much
	 * like in a tab dialog: Exchange field contents of the cleanup
	 * properties page with the cleanup specified. Store the old
	 * properties page contents in the working copies of the cleanups.
	 **/
	void changeCleanup( Cleanup * cleanup );


    protected:

	/**
	 * Retrieve any pending changes from the properties page and store
	 * them in the cleanup specified.
	 **/
	void storeProps( Cleanup * cleanup );


	//
	// Data members
	//

	CleanupListBox *		_listBox;
	CleanupPropertiesPage *	_props;
	QDirStatApp *			_mainWin;

	CleanupCollection		_workCleanupCollection;
	Cleanup *			_currentCleanup;

    }; // class CleanupPage



    /**
     * List box for cleanup actions.
     *
     * This is meant as a substitute for a tabbed dialog inside the tabbed
     * dialog which would be much too wide and possibly confusing. Plus, this
     * list box is supposed to take care of its own geometry - the normal
     * dumbass list box obviously cannot do that. It just uses some random
     * geometry, relying on scroll bars for everything else. But in this
     * special case we want all items to be visible at all times without scroll
     * bars.
     *
     * @short cleanup list box
     **/
    class CleanupListBox: public QListBox
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	CleanupListBox( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~CleanupListBox() {};

	/**
	 * Reimplemented so we can make sure all items are visible at all times
	 * without scrolling. In fact, we never want to see a scroll bar with
	 * this kind of list box.
	 **/
	virtual QSize sizeHint() const;

	/**
	 * Insert an entry for a cleanup action into the list box. Uses the
	 * cleanup action's internally stored title for display.
	 **/
	void insert( Cleanup * cleanup );

	/**
	 * Returns the currently selected cleanup of 0 if nothing is selected.
	 **/
	Cleanup * selection()	{ return _selection; }

	/**
	 * Update the list item's text that corresponds to 'cleanup' - the user
	 * may have entered a new cleanup name. '0' means "check all items".
	 **/
	void updateTitle( Cleanup * cleanup = 0 );


    signals:

	/**
	 * Emitted when the user selects a list item, i.e. a cleanup action.
	 **/
	void selectCleanup( Cleanup * cleanup );


    protected slots:

	/**
	 * Select an item.
	 **/
	void selectCleanup( QListBoxItem * item );


    protected:

	Cleanup * _selection;

    }; // class CleanupListBox



    /**
     * List box item for a CleanupListBox.
     **/
    class CleanupListBoxItem: public QListBoxText
    {
    public:

	/**
	 * Constructor.
	 **/
	CleanupListBoxItem( CleanupListBox *	listBox,
			     Cleanup *	 	cleanup );

	/**
	 * Returns the corresponding cleanup.
	 **/
	Cleanup * cleanup() { return _cleanup; }

	/**
	 * Update the list box display with the cleanup's name which may have
	 * changed - the user may have entered a new one.
	 **/
	void updateTitle();


    protected:


	// Data members

	Cleanup * _cleanup;

    }; // class CleanupListBoxItem



    /**
     * Properties page for one cleanup action.
     **/
    class CleanupPropertiesPage: public QWidget
    {
	Q_OBJECT

    public:

	/**
	 * Constructor
	 **/
	CleanupPropertiesPage( QWidget *	parent,
				QDirStatApp *	mainWin );

	/**
	 * Retrieve the page's fields' values and store them in the cleanup
	 * action.
	 **/
	Cleanup fields( void ) const;


    public slots:

	/**
	 * Set the page's fields' values with the cleanup action's
	 * contents.
	 **/
	void setFields( const Cleanup * cleanup );

	/**
	 * Enable / disable all of the properties page's fields except the
	 * 'enabled' check box.
	 **/
	void enableFields( bool active );


    protected:

	QString			_id;
	QCheckBox *		_enabled;
	QWidget *		_fields;
	QLineEdit *		_title;
	QLineEdit *		_command;
	QCheckBox *		_recurse;
	QCheckBox *		_askForConfirmation;
	QCheckBox *		_worksForDir;
	QCheckBox *		_worksForFile;
	QCheckBox *		_worksForDotEntry;
	QComboBox *		_worksForProtocols;
	QComboBox *		_refreshPolicy;

	QDirStatApp *		_mainWin;

    }; // class CleanupPropertiesPage



    /**
     * Settings tab page for general/misc settings.
     **/
    class GeneralSettingsPage: public SettingsPage
    {
	Q_OBJECT

    public:

	/**
	 * Constructor
	 **/
	GeneralSettingsPage( SettingsDialog *	dialog,
			      QWidget *		parent,
			      QDirStatApp *	mainWin );

	/**
	 * Destructor
	 **/
	virtual ~GeneralSettingsPage();


    public slots:

	/**
	 * Apply the changes.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
        virtual void apply();

	/**
	 * Revert all values to their defaults.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
	virtual void revertToDefaults();

	/**
	 * Set up all fields prior to displaying the dialog.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
	virtual void setup();

	/**
	 * Check the enabled state of all widgets depending on the value of
	 * other widgets.
	 **/
	void checkEnabledState();

				
    protected slots:
	
	/**
	 * Add a new exclude rule.
	 **/
	void addExcludeRule();

	/**
	 * Edit the currently selected exclude rule.
	 **/
	void editExcludeRule();

	/**
	 * Delete the currently selected exclude rule.
	 **/
	void deleteExcludeRule();

	/**
	 * Show a context menu for the currently selected exclude rule.
	 **/
	void showExcludeRuleContextMenu( QListViewItem *, const QPoint &, int );

    protected:

	// Data members

	QDirStatApp *	_mainWin;
	DirTreeView *	_treeView;

	QCheckBox *	_crossFileSystems;
	QCheckBox *	_enableLocalDirReader;

	QCheckBox *	_enableToolBarAnimation;
	QCheckBox *	_enableTreeViewAnimation;

	QListView *	_excludeRulesListView;
	QPushButton * 	_addExcludeRuleButton;
	QPushButton *	_editExcludeRuleButton;
	QPushButton *	_deleteExcludeRuleButton;
	QPopupMenu *	_excludeRuleContextMenu;

    }; // class GeneralSettingsPage



    /**
     * Settings tab page for treemap settings.
     **/
    class KTreemapPage: public SettingsPage
    {
	Q_OBJECT

    public:

	/**
	 * Constructor
	 **/
	KTreemapPage( SettingsDialog *	dialog,
		      QWidget *		parent,
		      QDirStatApp *	mainWin );

	/**
	 * Destructor
	 **/
	virtual ~KTreemapPage();


    public slots:

	/**
	 * Apply the changes.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
        virtual void apply();

	/**
	 * Revert all values to their defaults.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
	virtual void revertToDefaults();

	/**
	 * Set up all fields prior to displaying the dialog.
	 *
	 * Inherited from @ref SettingsPage.
	 **/
	virtual void setup();

	/**
	 * Check the enabled state of all widgets depending on the value of
	 * other widgets.
	 **/
	void checkEnabledState();


    protected:

	/**
	 * Returns the main window's current treemap view or 0 if there is
	 * none. Don't cache this value, it changes frequently!
	 **/
	KTreemapView * treemapView() const { return _mainWin->treemapView(); }

	/**
	 * Convenience method to read a color from 'config'.
	 **/
	QColor readColorEntry( KConfig * 	config,
			       const char * 	entryName,
			       QColor 		defaultColor );

	// Data members

	QDirStatApp *		_mainWin;

	
	// Widgets
	
	QCheckBox *		_squarify;
	QCheckBox *		_doCushionShading;
	QVGroupBox *		_cushionParams;
	QSlider *		    _ambientLight;
	QSpinBox *		    _ambientLightSB;
	QSlider *		    _heightScalePercent;
	QSpinBox *		    _heightScalePercentSB;
	QCheckBox *		    _ensureContrast;
	QCheckBox *		    _forceCushionGrid;
	KColorButton *		    _cushionGridColor;
	QLabel *		    _cushionGridColorL;
	QHGroupBox *		_plainTileParams;
	KColorButton *		    _fileFillColor;
	KColorButton *		    _dirFillColor;
	KColorButton *		    _outlineColor;
	KColorButton *		_highlightColor;
	QSpinBox *		_minTileSize;
	QCheckBox *		_autoResize;
	
    }; // class KTreemapPage

} // namespace QDirStat


/**
 * Add a horizontal stretch widget to take all excess space.
 **/
void addHStretch( QWidget * parent );

/**
 * Add a vertical stretch widget to take all excess space.
 **/
void addVStretch( QWidget * parent );



#endif // ifndef QDirStatSettings_h


// EOF
