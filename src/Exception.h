/*
 *   File name: Exception.h
 *   Summary:	Exception classes for QDirstat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef Exception_h
#define Exception_h

#include "Logger.h"

#include <QString>


/**
 * Generic exception.
 *
 * Unlike std::exception, this class uses QString rather than const char *
 */
class Exception
{
public:
    /**
     * Constructor.
     */
    Exception( const QString &msg = QString() ):
	_what( msg ),
	_srcLine(0)
	{}

    /**
     * Destructor.
     */
    virtual ~Exception() throw()
	{}

    /**
     * Return a text description of what was wrong.
     *
     * Notice this text is intended for developers or admins, not for end
     * users.
     */
    virtual QString what() const { return _what; }

    /**
     * Return the class name of this exception as string.
     *
     * Derived classes should reimplement this and return their own name.
     */
    virtual QString className() const { return "Exception"; }

    /**
     * Return the source file name where the exception was thrown.
     * This works only if it was thrown with THROW.
     */
    QString srcFile() const { return _srcFile; }

    /**
     * Return the line number in the source file where the exception was
     * thrown. This works only if it was thrown with THROW.
     */
    int srcLine() const { return _srcLine; }

    /**
     * Return the function name where the exception was thrown.
     * This works only if it was thrown with THROW.
     */
    QString srcFunction() const { return _srcFunction; }

    /**
     * Set the source location where the exception was thrown.
     * This is used in the THROW and RETHROW macros.
     */
    void setSrcLocation( const QString &srcFile,
			 int		srcLine,
			 const QString &srcFunction ) const;

protected:
    QString _what;

    mutable QString _srcFile;
    mutable int	    _srcLine;
    mutable QString _srcFunction;
};


/**
 * Exception class for null pointers.
 * Use with CHECK_PTR().
 */
class NullPointerException: public Exception
{
public:
    NullPointerException():
	Exception( "Null pointer" )
	{}

    virtual ~NullPointerException() throw()
	{}
};


/**
 * Exception class for "out of memory" ('new' failed).
 * Use with CHECK_NEW().
 */
class OutOfMemoryException: public Exception
{
public:
    OutOfMemoryException():
	Exception( "Null pointer" )
	{}

    virtual ~OutOfMemoryException() throw()
	{}
};


class FileException: public Exception
{
public:
    FileException( const QString &filename, const QString &msg ):
	Exception( msg ),
	_filename( filename )
	{}

    virtual ~FileException() throw()
	{}

    QString filename() const { return _filename; }

private:
    QString _filename;
};


class SysCallFailedException: public Exception
{
public:
    SysCallFailedException( const QString & sysCall,
			    const QString & resourceName ):
	Exception( errMsg( sysCall, resourceName ) ),
	_sysCall( sysCall ),
	_resourceName( resourceName )
	{}

    virtual ~SysCallFailedException() throw()
	{}

    /**
     * Return the resource for which this syscall failed. This is typically a
     * file name.
     **/
    QString resourceName() const { return _resourceName; }

    QString sysCall() const { return _sysCall; }

protected:
    QString errMsg( const QString & sysCall,
		    const QString & resourceName ) const;

private:

    QString _sysCall;
    QString _resourceName;
};


class DynamicCastException: public Exception
{
public:
    DynamicCastException( const QString &expectedType ):
	Exception( "dynamic_cast failed; expected: " + expectedType )
	{}

    virtual ~DynamicCastException() throw()
	{}
};


class BadMagicNumberException: public Exception
{
public:
    BadMagicNumberException( void * badPointer ):
	Exception( QString( "Magic number check failed for address 0x%1" )
		   .arg( (qulonglong) badPointer, 0, 16 ) )
	{}

    virtual ~BadMagicNumberException() throw()
	{}
};


/**
 * Exception class for "index out of range"
 **/
class IndexOutOfRangeException : public Exception
{
public:
    /**
     * Constructor.
     *
     * 'invalidIndex' is the offending index value. It should be between
     *'validMin' and 'validMax':
     *
     *	   validMin <= index <= validMax
     **/
    IndexOutOfRangeException( int invalidIndex,
			      int validMin,
			      int validMax,
			      const QString & msg = "" )
	: Exception( errMsg( invalidIndex, validMin, validMax, msg ) )
	, _invalidIndex( invalidIndex )
	, _validMin( validMin )
	, _validMax( validMax )
	{}

    virtual ~IndexOutOfRangeException() throw()
	{}

    /**
     * Return the offending index value.
     **/
    int invalidIndex() const	{ return _invalidIndex; }

    /**
     * Return the valid minimum index.
     **/
    int validMin() const	{ return _validMin; }

    /**
     * Return the valid maximum index.
     **/
    int validMax() const	{ return _validMax; }

protected:
    QString errMsg( int	invalidIndex,
		    int	validMin,
		    int	validMax,
		    const QString & msg = "" ) const;

private:

    int _invalidIndex;
    int _validMin;
    int _validMax;
};




//
// Helper macros
//

/**
 * Throw an exception and write it to the log, together with the source code
 * location where it was thrown. This makes it MUCH easier to find out where
 * a problem occurred.
 *
 * Use this as a substitute for normal 'throw( exception )'.
 */
#define THROW( EXCEPTION ) \
    _throw_helper( ( EXCEPTION ), 0, __FILE__, __LINE__, __FUNCTION__ )

/**
 * Write a log notification that an exception has been caught.
 * This is not a substitute for a 'catch' statement. Rather, use it inside
 * a 'catch' block.
 *
 * Example:
 *
 *     try
 *     {
 *	   ...do something...
 *	   THROW( Exception( "Catastrophic failure" ) );
 *	   ...
 *     }
 *     catch( const Exception &exception )
 *     {
 *	   CAUGHT( exception );
 *	   ...clean up to prevent memory leaks etc. ...
 *	   RETHROW( exception ); // equivalent of   throw   without args
 *     }
 *
 * This will leave 3 lines for that exception in the log file: One for
 * THROW, one for CAUGHT, one for RETHROW. Each log line contains the
 * source file, the line number, and the function of the THROW or CAUGHT
 * or RETHROW calls.
 */
#define CAUGHT( EXCEPTION ) \
    _caught_helper( ( EXCEPTION ), 0, __FILE__, __LINE__, __FUNCTION__ )

/**
 * Write a log notification that an exception that has been caught is
 * being thrown again inside a 'catch' block. Use this as a substitute for
 * plain 'throw' without arguments. Unlike a plain 'throw', this macro does
 * have an argument.
 */
#define RETHROW( EXCEPTION ) \
    _rethrow_helper( ( EXCEPTION ), 0, __FILE__, __LINE__, __FUNCTION__ )

/**
 * Check the result of 'new' and throw exception if it returned 0.
 *
 * The do..while() loop is used because it syntactically allows to put a
 * semicolon (without nasty side effects) after the macro when it is used.
 */
#define CHECK_NEW( PTR )			\
    do						\
    {						\
	if ( ! (PTR) )				\
	{					\
	    THROW( OutOfMemoryException() );	\
	}					\
    } while( 0 )



/**
 * Check a pointer and throw an exception if it returned 0.
 */
#define CHECK_PTR( PTR )			\
    do						\
    {						\
	if ( ! (PTR) )				\
	{					\
	    THROW( NullPointerException() );	\
	}					\
    } while( 0 )


/**
 * Check the result of a dynamic_cast and throw an exception if it returned 0.
 */
#define CHECK_DYNAMIC_CAST( PTR, EXPECTED_TYPE )		\
    do								\
    {								\
	if ( ! (PTR) )						\
	{							\
	    THROW( DynamicCastException( EXPECTED_TYPE) );	\
	}							\
    } while( 0 )


/**
 * Check the magic number of an object and throw an exception if it returned false.
 */
#define CHECK_MAGIC( PTR )					\
    do								\
    {								\
	if ( ! (PTR) )						\
	{							\
	    THROW( NullPointerException() );			\
	}							\
								\
	if ( ! PTR->checkMagicNumber() )			\
	{							\
	    THROW( BadMagicNumberException( PTR ) );		\
	}							\
    } while( 0 )


/**
 * Check if an index is in range:
 * VALID_MIN <= INDEX <= VALID_MAX
 *
 * Throws InvalidWidgetException if out of range.
 **/
#define CHECK_INDEX_MSG( INDEX, VALID_MIN, VALID_MAX, MSG )	\
    do								\
    {								\
	if ( (INDEX) < (VALID_MIN) ||				\
	     (INDEX) > (VALID_MAX) )				\
	{							\
	    THROW( IndexOutOfRangeException( (INDEX), (VALID_MIN), (VALID_MAX), (MSG) ) ); \
	}							\
    } while( 0 )


#define CHECK_INDEX( INDEX, VALID_MIN, VALID_MAX )		\
    CHECK_INDEX_MSG( (INDEX), (VALID_MIN), (VALID_MAX), "")


//
// Helper functions. Do not use directly; use the corresponding macros instead.
//

template<class EX_t>
void _throw_helper( const EX_t	  &exception,
		    Logger *	   logger,
		    const QString &srcFile,
		    int		   srcLine,
		    const QString &srcFunction )
{
    exception.setSrcLocation( srcFile, srcLine, srcFunction );

    Logger::log( logger, srcFile, srcLine, srcFunction, LogSeverityWarning )
	<< "THROW "
	<< exception.className() << ": "
	<< exception.what()
	<< endl;

    throw( exception );
}


template<class EX_t>
void _caught_helper( const EX_t	   &exception,
		    Logger *	   logger,
		     const QString &srcFile,
		     int	    srcLine,
		     const QString &srcFunction )
{
    Logger::log( logger, srcFile, srcLine, srcFunction, LogSeverityWarning )
	<< "CAUGHT "
	<< exception.className() << ": "
	<< exception.what()
	<< endl;
}


template<class EX_t>
void _rethrow_helper( const EX_t    &exception,
		      Logger *	     logger,
		      const QString &srcFile,
		      int	     srcLine,
		      const QString &srcFunction )
{
    exception.setSrcLocation( srcFile, srcLine, srcFunction );

    Logger::log( logger, srcFile, srcLine, srcFunction, LogSeverityWarning )
	<< "RETHROW "
	<< exception.className() << ": "
	<< exception.what()
	<< endl;

    throw;
}



#endif // Exception_h
