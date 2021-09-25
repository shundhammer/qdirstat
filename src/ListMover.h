#ifndef ListMover_h
#define ListMover_h

namespace QDirStat
{
    /**
     * Template class to move items in a QList<Value_t> up, down, to the top,
     * or to the bottom.
     **/
    template<class Value_t> class ListMover
    {
    public:

        ListMover( QList<Value_t> & list ):
            _list( list )
            {}

        void moveUp( Value_t item )
        {
            int oldPos = _list.indexOf( item );

            if ( oldPos > 0 )
            {
                _list.removeAt( oldPos );
                _list.insert( oldPos - 1, item );
            }
        }


        void moveDown( Value_t item )
        {
            int oldPos = _list.indexOf( item );

            if ( oldPos < _list.size() - 1 )
            {
        	_list.removeAt( oldPos );
        	_list.insert( oldPos + 1, item );
            }
        }


        void moveToTop( Value_t item )
        {
            int oldPos = _list.indexOf( item );

            if ( oldPos > 0 )
            {
        	_list.removeAt( oldPos );
        	_list.insert( 0, item );
            }
        }


        void moveToBottom( Value_t item )
        {
            int oldPos = _list.indexOf( item );

            if ( oldPos < _list.size() - 1 )
            {
        	_list.removeAt( oldPos );
        	_list.insert( _list.size(), item );
           }
        }

    protected:
        QList<Value_t> & _list;
    };

    typedef ListMover<void *> PtrListMover;

}       // namespace QDirStat

#endif	// ListMover_h
