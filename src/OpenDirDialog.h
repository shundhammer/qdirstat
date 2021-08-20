/*
 *   File name: OpenDirDialog.h
 *   Summary:	QDirStat "open directory" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef OpenDirDialog_h
#define OpenDirDialog_h

#include <QDialog>
#include <QModelIndex>

#include "ui_open-dir-dialog.h"

class QFileSystemModel;


namespace QDirStat
{
    class ExistingDirValidator;

    /**
     * Dialog to let the user select installed packages to open, very much like
     * a "get existing directory" dialog, but returning a PkgFilter instead.
     **/
    class OpenDirDialog: public QDialog
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 *
	 * Consider using the static methods instead.
	 **/
	OpenDirDialog( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~OpenDirDialog();

	/**
	 * Open an "open directory" dialog, wait for the user to select one and
         * return that path. If the user cancelled the dialog, this returns an
         * empty string.
         *
         * 'crossFilesystems_ret' (if non-null) returns the "cross filesystems"
         * flag of the dialog.
	 **/
	static QString askOpenDir( bool *    crossFilesystems_ret,
                                   QWidget * parent = 0 );

	/**
	 * The path of the directory the user selected.
	 **/
        QString selectedPath() const;

        /**
         * The "cross filesystems" flag of this dialog (overriding the global
         * "cross filesystems" setting" from the config dialog).
         **/
        bool crossFilesystems() const;

        /**
         * Return this dialog's path selector so it can be populated.
         **/
        PathSelector * pathSelector() const { return _ui->pathSelector; }


    public slots:

        /**
         * Set a path in the dirTree.
         **/
        void setPath( const QString & path );

        /**
         * Set a path in the dirTree and expand (open) that branch.
         **/
        void setPathAndExpand( const QString & path );

        /**
         * Set a path in the dirTree and accept the dialog.
         **/
        void setPathAndAccept( const QString & path );

        /**
         * Go up one directory level.
         **/
        void goUp();

	/**
	 * Read settings from the config file
	 **/
	void readSettings();

	/**
	 * Write settings to the config file
	 **/
	void writeSettings();


    protected slots:

        /**
         * Notification that the user selected a directory in the tree
         **/
        void treeSelection( const QModelIndex & newCurrentItem,
                            const QModelIndex & oldCurrentItem );

        /**
         * Notification that the user edited a path in the combo box.
         * 'ok' is the result of the validator's check.
         **/
        void pathEdited( bool ok );

        /**
         * Select a directory once everything is initialized and the first
         * signals are processed.
         **/
        void initialSelection();


    protected:

        void initPathComboBox();
        void initDirTree();
        void initConnections();

        void populatePathComboBox( const QString & path );


	Ui::OpenDirDialog *     _ui;
        QFileSystemModel *      _filesystemModel;
	QPushButton *           _okButton;
        ExistingDirValidator *  _validator;
        bool                    _settingPath;
        QString                 _lastPath;

        static bool             _crossFilesystems;

    };	// class OpenDirDialog

}	// namespace QDirStat

#endif	// OpenDirDialog_h
