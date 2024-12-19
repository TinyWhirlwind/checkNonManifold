#ifndef _PLYLIB_ 
#define _PLYLIB_ 

#include <memory.h>
#include <vector>
#include <string>
#include <assert.h>

namespace ply 
{

enum PlyTypes 
{
	T_NOTYPE,
	T_CHAR,
	T_SHORT,
	T_INT,
	T_UCHAR,
	T_USHORT,
	T_UINT,
	T_FLOAT,
	T_DOUBLE,
	T_MAXTYPE
};

enum PlyError 
{
	E_NOERROR,				// 0
	
	E_CANTOPEN,				// 1
	E_NOTHEADER,			// 2
	E_UNESPECTEDEOF,		// 3
	E_NOFORMAT,				// 4
	E_SYNTAX,				// 5
	E_PROPOUTOFELEMENT,		// 6
	E_BADTYPENAME,			// 7
	
	E_ELEMNOTFOUND,			// 8
	E_PROPNOTFOUND,			// 9
	E_BADTYPE,				// 10
	E_INCOMPATIBLETYPE,		// 11
	E_BADCAST,				// 12
	E_MAXPLYERRORS
};

enum PlyFormat
{
	F_UNSPECIFIED,
	F_ASCII,
	F_BINLITTLE,
	F_BINBIG
};


typedef FILE * GZFILE;


class PropDescriptor
{
public:
	char * elemname;	// name of element
	char * propname;	// Name of Property
	int	stotype1;		// Type of item to file
	int memtype1;		// type of element in memory
	size_t offset1;		// Offset value in memory
	int islist;			// 1 if the list, 0 otherwise
	int alloclist;		// 1 if allocates list, 0 if pre-allocata
	int stotype2;		// Type in the number of elements of the list to file
	int memtype2;		// Type in the number of elements of the list in memory
	size_t offset2;		// Offset value memory

	int format;			// duplication of the format
	
	size_t			stotypesize() const; // to know how large a file descriptor
	size_t			memtypesize() const; // to know how large a given descriptor in memory
	const char *memtypename() const; 
	const char *stotypename() const;
};

// Reading Callback (used to copy a data prop)
typedef bool (* readelemcb) ( GZFILE fp, void * mem, PropDescriptor * p );

class PlyProperty
{
public:
	inline PlyProperty()
	{
		tipo		= 0;
		islist		= 0;
		tipoindex	= 0;
		bestored	= 0;
	}

	inline PlyProperty( const char * na, int ti, int isl, int t2 )
	{
		assert(na);
		assert(ti>0);
		assert(ti<T_MAXTYPE);
		assert( t2>0 || (t2==0 && isl==0) );
		assert(t2<T_MAXTYPE);

		name		= std::string(na);
		tipo		= ti;
		islist		= isl;
		tipoindex	= t2;
		bestored	= 0;
	}

	std::string name;	
	int    tipo;			
	int    islist;				
	int    tipoindex;			

	int	   bestored;		

	PropDescriptor desc;		

	readelemcb	cb;			
};


class PlyElement
{
public:
	
	inline PlyElement()
	{
		number	= 0;
	}

	inline PlyElement( const char * na, int nu )
	{
		assert(na);
		assert(nu>=0);

		name    = std::string(na);
		number	= nu;
	}

	inline void SetName( const char * na )
	{
		name = std::string(na);
	}

	inline void SetNumbert( int nu )
	{
		assert(nu>0);
		number = nu;
	}

	void AddProp( const char * na, int ti, int isl, int t2 );

	int AddToRead(
		const char * propname,
		int	stotype1,
		int memtype1,
		size_t offset1,
		int islist,
		int alloclist,
		int stotype2,
		int memtype2,
		size_t offset2
	);

	PlyProperty * FindProp( const char * name );

	std::string name;			

	int    number;				

    std::vector<PlyProperty> props;	
};


class PlyFile
{
private:
	void compile( PlyElement * e );
	void compile( PlyProperty * p );

public:
	
	//Ways of opening
	enum {
		MODE_READ,
		MODE_WRITE
	};

	 PlyFile();
	~PlyFile();

		// Opens a file ply
	int Open( const char * filename, int mode );
	int Open(const wchar_t* filename, int mode);
		// Closes a file and the memory disalloca
	void Destroy();
		// Return the last error code of
	inline int GetError() const { return error; }
		// Definition of reading 
	int AddToRead(
		const char * elemname,
		const char * propname,
		int	stotype1,
		int memtype1,
		size_t offset1,
		int islist,
		int alloclist,
		int stotype2,
		int memtype2,
		size_t offset2
	);
		
	inline int AddToRead( const PropDescriptor & p )
	{
		return AddToRead(p.elemname,p.propname,p.stotype1,
			p.memtype1,p.offset1,p.islist,p.alloclist,p.stotype2,
			p.memtype2,p.offset2
		);
	}

	const char * ElemName( int i );

	// Return the number of objects of one type of element
	int ElemNumber( int i ) const;
		
	// Sets the current item type to do reading	
	inline void SetCurElement( int i )
	{
	  if(i<0 || i>=int(elements.size())) cure = 0;
	  else
	  {
		cure = &(elements[i]);
		compile(cure);
	  }
	}

	//Reading of an element
	int Read( void * mem );

	std::vector<PlyElement> elements;	// Vector elements
	std::vector<std::string> comments;
	static const char * typenames[9];
	static const char * newtypenames[9];

	inline const char * GetHeader() const { return header; }

protected:

	GZFILE gzfp;

	float  version;				// Version of the file
	int    error;				// Current error
	int    format;				// File format 
	char   header[4096];		// Text of header

	PlyElement * cure;			// Element to read

	
	// Callback reading: namely ReadBin or ReadAcii
	int (* ReadCB)( GZFILE fp, const PlyProperty * r, void * mem, int fmt );

	int OpenRead( const char * filename );
	int OpenWrite( const char * filename );
	int OpenRead(const wchar_t * filename);
	int OpenWrite(const wchar_t * filename);
	int OpenRead();
	
	PlyElement * AddElement( const char * name, int number );
	int FindType( const char * name ) const;
	PlyElement * FindElement( const char * name );
};



  } // end namespace ply

#endif
