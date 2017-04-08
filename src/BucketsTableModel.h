/*
 *   File name: BucketsTableModel.h
 *   Summary:	Data model for buckets table
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef BucketsTableModel_h
#define BucketsTableModel_h

#include <QAbstractTableModel>


namespace QDirStat
{
    class HistogramView;

    /**
     * Data model for the buckets table in the file size statistics window.
     * This displays the data of that window's histogram view in a table.
     **/
    class BucketsTableModel: public QAbstractTableModel
    {
        Q_OBJECT

    public:
        enum Column
        {
            StartCol,
            EndCol,
            ValueCol,
            ColCount
        };

        /**
         * Constructor.
         **/
        BucketsTableModel( QWidget * parent, HistogramView * histogram );

        /**
         * Destructor.
         **/
        virtual ~BucketsTableModel();

        /**
         * Return the associated histogram view.
         **/
        HistogramView * histogram() const { return _histogram; }

        /**
         * Notification that data in the histogram have been reset.
         **/
        void reset();


        //
        // Overloaded model methods
        //

        /**
	 * Return the number of rows (direct tree children) for 'parent'.
	 **/
	virtual int rowCount   ( const QModelIndex & parent ) const Q_DECL_OVERRIDE;

	/**
	 * Return the number of columns for 'parent'.
	 **/
	virtual int columnCount( const QModelIndex & parent ) const Q_DECL_OVERRIDE;

	/**
	 * Return data to be displayed for the specified model index and role.
	 **/
	virtual QVariant data( const QModelIndex & index, int role ) const Q_DECL_OVERRIDE;

	/**
	 * Return header data for the specified section.
	 **/
	virtual QVariant headerData( int	     section,
				     Qt::Orientation orientation,
				     int	     role ) const Q_DECL_OVERRIDE;

	/**
	 * Return item flags for the specified model index. This specifies if
	 * the item can be selected, edited etc.
	 **/
	virtual Qt::ItemFlags flags( const QModelIndex &index ) const Q_DECL_OVERRIDE;


    protected:

        HistogramView * _histogram;
    };
}

#endif // BucketsTableModel_h
