#ifndef _IMPORTERPLY_
#define _IMPORTERPLY_

#include "../BaseReader.h"
#include "plylib.h"
#include "io_ply.h"
#include "io_mask.h"
#include "Common\color4.h"
//#include"../../../Common/color4.h"
#include <vector>

template <class TYPE>

int PlyType ()  { return 0;}
template <> inline int PlyType <float >()  { return ply::T_FLOAT; }
template <> inline int PlyType <double>()  { return ply::T_DOUBLE; }
template <> inline int PlyType <int   >()  { return ply::T_INT; } 
template <> inline int PlyType <short >()  { return ply::T_SHORT; }
template <> inline int PlyType <unsigned char >()  { return ply::T_UCHAR; }


class ImporterPly : public sn3DCore::BaseReader
{
public:

	typedef ply::PropDescriptor PropDescriptor ;

public:

	bool read(const std::wstring& _filename, BaseImporter*& _bi);
	//{
	//	sn3DMeshModel* model = new sn3DMeshModel;
	//	model->AddMesh();
	//	_bi = new MeshImporter(model);
	//	PlyInfo pi;

	//	read(_filename, *_bi, pi);

	//	_bi->Update();  // 更新模型信息

	//	return true;
	//}

	bool read(const std::wstring& _filename, BaseImporter& _bi);
	//{
	//	PlyInfo pi;

	//	read(_filename, _bi, pi);

	//	_bi.Update();  // 更新模型信息

	//	return true;
	//}

	std::wstring get_description() const { return L"Stanford/PLY"; }

	std::wstring get_extensions()  const { return L"ply"; }

	#define MAX_USER_DATA 256

	struct LoadPly_FaceAux
	{
		unsigned char size;
		int v[512];
		int flags;
		float q;
		float texcoord[32];
		unsigned char ntexcoord;
		int texcoordind;
		float colors[32];
		unsigned char ncolors;
	
		unsigned char r;
		unsigned char g;
		unsigned char b;

		unsigned char data[MAX_USER_DATA];  
	};

	struct LoadPly_TristripAux
	{
		int size;
		int *v;
		unsigned char data[MAX_USER_DATA];  
	};


	template<class S>
	struct LoadPly_VertAux
	{
		S p[3];
		S n[3];
		int flags;
		float q;
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char data[MAX_USER_DATA];  
	};
	struct LoadPly_Camera
	{
		float view_px;
		float view_py;
		float view_pz;
		float x_axisx;
		float x_axisy;
		float x_axisz;
		float y_axisx;
		float y_axisy;
		float y_axisz;
		float z_axisx;
		float z_axisy;
		float z_axisz;
		float focal;
		float scalex;
		float scaley;
		float centerx;
		float centery;
		int   viewportx;
		int   viewporty;
		float k1;
		float k2;
		float k3;
		float k4;
	};

	static const  PropDescriptor &VertDesc(int i)  
	{
		static const PropDescriptor pv[15]={
	/*00*/ {"vertex", "x",         ply::T_FLOAT, PlyType<float>(),offsetof(LoadPly_VertAux<float>,p),0,0,0,0,0  ,0},
	/*01*/ {"vertex", "y",         ply::T_FLOAT, PlyType<float>(),offsetof(LoadPly_VertAux<float>,p) + sizeof(float),0,0,0,0,0  ,0},
	/*02*/ {"vertex", "z",         ply::T_FLOAT, PlyType<float>(),offsetof(LoadPly_VertAux<float>,p) + 2*sizeof(float),0,0,0,0,0  ,0},
	/*03*/ {"vertex", "flags",     ply::T_INT,   ply::T_INT,           offsetof(LoadPly_VertAux<float>,flags),0,0,0,0,0  ,0},
	/*04*/ {"vertex", "quality",   ply::T_FLOAT, ply::T_FLOAT,         offsetof(LoadPly_VertAux<float>,q),0,0,0,0,0  ,0},
	/*05*/ {"vertex", "red"  ,     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<float>,r),0,0,0,0,0  ,0},
	/*06*/ {"vertex", "green",     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<float>,g),0,0,0,0,0  ,0},
	/*07*/ {"vertex", "blue" ,     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<float>,b),0,0,0,0,0  ,0},
	/*08*/ {"vertex", "diffuse_red"  ,     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<float>,r),0,0,0,0,0  ,0},
	/*09*/ {"vertex", "diffuse_green",     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<float>,g),0,0,0,0,0  ,0},
	/*10*/ {"vertex", "diffuse_blue" ,     ply::T_UCHAR, ply::T_UCHAR,         offsetof(LoadPly_VertAux<float>,b),0,0,0,0,0  ,0},
	/*11*/ {"vertex", "confidence",ply::T_FLOAT, ply::T_FLOAT,         offsetof(LoadPly_VertAux<float>,q),0,0,0,0,0  ,0},
	/*12*/ {"vertex", "nx",         ply::T_FLOAT, PlyType<float>(),offsetof(LoadPly_VertAux<float>,n),0,0,0,0,0  ,0},
	/*13*/ {"vertex", "ny",         ply::T_FLOAT, PlyType<float>(),offsetof(LoadPly_VertAux<float>,n) + sizeof(float),0,0,0,0,0  ,0},
	/*14*/ {"vertex", "nz",         ply::T_FLOAT, PlyType<float>(),offsetof(LoadPly_VertAux<float>,n) + 2*sizeof(float),0,0,0,0,0  ,0},
		};
		return pv[i];
	}
	static const  PropDescriptor &FaceDesc(int i)  
	{		
		static const 	PropDescriptor qf[13]=
		{
			{"face", "vertex_indices", ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_UCHAR,ply::T_UCHAR,offsetof(LoadPly_FaceAux,size)   ,0},
			{"face", "flags",          ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,flags),     0,0,0,0,0  ,0},
			{"face", "quality",        ply::T_FLOAT, ply::T_FLOAT, offsetof(LoadPly_FaceAux,q),         0,0,0,0,0  ,0},
			{"face", "texcoord",       ply::T_FLOAT, ply::T_FLOAT, offsetof(LoadPly_FaceAux,texcoord),    1,0,ply::T_UCHAR,ply::T_UCHAR,offsetof(LoadPly_FaceAux,ntexcoord)   ,0},
			{"face", "color",          ply::T_FLOAT, ply::T_FLOAT, offsetof(LoadPly_FaceAux,colors),    1,0,ply::T_UCHAR,ply::T_UCHAR,offsetof(LoadPly_FaceAux,ncolors)   ,0},
			{"face", "texnumber",      ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,texcoordind), 0,0,0,0,0  ,0},
			{"face", "red"  ,          ply::T_UCHAR, ply::T_UCHAR, offsetof(LoadPly_FaceAux,r),         0,0,0,0,0  ,0},
			{"face", "green",          ply::T_UCHAR, ply::T_UCHAR, offsetof(LoadPly_FaceAux,g),         0,0,0,0,0  ,0},
			{"face", "blue" ,          ply::T_UCHAR, ply::T_UCHAR, offsetof(LoadPly_FaceAux,b),         0,0,0,0,0  ,0},
			{"face", "vertex_index",   ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_UCHAR,ply::T_CHAR,offsetof(LoadPly_FaceAux,size)  ,0},
			{"face", "vertex_indices", ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_CHAR, ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
			{"face", "vertex_index",   ply::T_INT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_CHAR, ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
			{"face", "vertex_indices", ply::T_UINT,   ply::T_INT,   offsetof(LoadPly_FaceAux,v),		     1,0,ply::T_UCHAR,ply::T_CHAR,offsetof(LoadPly_FaceAux,size)   ,0},
		};
		return qf[i];
	}
	static const  PropDescriptor &TristripDesc(int i)  
	{		
		static const 	PropDescriptor qf[1]=
		{
			{"tristrips","vertex_indices", ply::T_INT,  ply::T_INT,  offsetof(LoadPly_TristripAux,v),		  1,1,ply::T_INT,ply::T_INT,offsetof(LoadPly_TristripAux,size) ,0},				
		};
		return qf[i];
	}


	static const  PropDescriptor &CameraDesc(int i)  
	{		
		static const PropDescriptor cad[23] =
		{
			{"camera","view_px",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,view_px),0,0,0,0,0  ,0},
			{"camera","view_py",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,view_py),0,0,0,0,0  ,0},
			{"camera","view_pz",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,view_pz),0,0,0,0,0  ,0},
			{"camera","x_axisx",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,x_axisx),0,0,0,0,0  ,0},
			{"camera","x_axisy",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,x_axisy),0,0,0,0,0  ,0},
			{"camera","x_axisz",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,x_axisz),0,0,0,0,0  ,0},
			{"camera","y_axisx",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,y_axisx),0,0,0,0,0  ,0},
			{"camera","y_axisy",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,y_axisy),0,0,0,0,0  ,0},
			{"camera","y_axisz",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,y_axisz),0,0,0,0,0  ,0},
			{"camera","z_axisx",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,z_axisx),0,0,0,0,0  ,0},
			{"camera","z_axisy",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,z_axisy),0,0,0,0,0  ,0},
			{"camera","z_axisz",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,z_axisz),0,0,0,0,0  ,0},
			{"camera","focal"  ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,focal  ),0,0,0,0,0  ,0},
			{"camera","scalex" ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,scalex ),0,0,0,0,0  ,0},
			{"camera","scaley" ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,scaley ),0,0,0,0,0  ,0},
			{"camera","centerx",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,centerx),0,0,0,0,0  ,0},
			{"camera","centery",ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,centery),0,0,0,0,0  ,0},
			{"camera","viewportx",ply::T_INT,ply::T_INT  ,offsetof(LoadPly_Camera,viewportx),0,0,0,0,0  ,0},
			{"camera","viewporty",ply::T_INT,ply::T_INT  ,offsetof(LoadPly_Camera,viewporty),0,0,0,0,0  ,0},
			{"camera","k1"     ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,k1 ),0,0,0,0,0  ,0},
			{"camera","k2"     ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,k2 ),0,0,0,0,0  ,0},
			{"camera","k3"     ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,k3 ),0,0,0,0,0  ,0},
			{"camera","k4"     ,ply::T_FLOAT,ply::T_FLOAT,offsetof(LoadPly_Camera,k4 ),0,0,0,0,0  ,0}
		};
		return cad[i];
	}
	/// Standard call for knowing the meaning of an error code
	static const char *ErrorMsg(int error)
	{
	  static std::vector<std::string> ply_error_msg;
	  if(ply_error_msg.empty())
	  {
		ply_error_msg.resize(PlyInfo::E_MAXPLYINFOERRORS );
		ply_error_msg[ply::E_NOERROR				]="No errors";
		  ply_error_msg[ply::E_CANTOPEN				]="Can't open file";
		ply_error_msg[ply::E_NOTHEADER ]="Header not found";
		  ply_error_msg[ply::E_UNESPECTEDEOF	]="Eof in header";
		  ply_error_msg[ply::E_NOFORMAT				]="Format not found";
		  ply_error_msg[ply::E_SYNTAX				]="Syntax error on header";
		  ply_error_msg[ply::E_PROPOUTOFELEMENT]="Property without element";
		  ply_error_msg[ply::E_BADTYPENAME		]="Bad type name";
		  ply_error_msg[ply::E_ELEMNOTFOUND		]="Element not found";
		  ply_error_msg[ply::E_PROPNOTFOUND		]="Property not found";
		  ply_error_msg[ply::E_BADTYPE				]="Bad type on addtoread";
		  ply_error_msg[ply::E_INCOMPATIBLETYPE]="Incompatible type";
		  ply_error_msg[ply::E_BADCAST				]="Bad cast";

		ply_error_msg[PlyInfo::E_NO_VERTEX      ]="No vertex field found";
		ply_error_msg[PlyInfo::E_NO_FACE        ]="No face field found";
		  ply_error_msg[PlyInfo::E_SHORTFILE      ]="Unespected eof";
		  ply_error_msg[PlyInfo::E_NO_3VERTINFACE ]="Face with more than 3 vertices";
		  ply_error_msg[PlyInfo::E_BAD_VERT_INDEX ]="Bad vertex index in face";
		  ply_error_msg[PlyInfo::E_NO_6TCOORD     ]="Face with no 6 texture coordinates";
		  ply_error_msg[PlyInfo::E_DIFFER_COLORS  ]="Number of color differ from vertices";
	  }

	  if(error>PlyInfo::E_MAXPLYINFOERRORS || error<0) return "Unknown error";
	  else return ply_error_msg[error].c_str();
	};

	int read(const std::wstring& _filename,BaseImporter& _bi,PlyInfo &pi);
 
};
#endif
