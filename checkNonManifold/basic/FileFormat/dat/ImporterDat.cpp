#include "stdafx.h"
#include "ImporterDat.h"
#include <cctype>
#include <string.h>
#include <sstream>
#include<locale.h>
#include<windows.h>
#include<gl\gl.h>
using namespace std;
//#include "..\..\..\..\sn3DPlatform\Include\sn3DImageLoad\sn3DImageData.h"
//#include "..\..\..\..\sn3DPlatform\Include\sn3DImageLoad\sn3DImageLoad.h"
#include "..\ImageConvert.h"
#include"../MeshImporter.h"
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

#pragma warning (disable : 4996)
using namespace sn3DCore;


ImporterDat::ImporterDat()
{ 
 
}
bool ImporterDat::read(std::string& _filename,BaseImporter * & _bi)
{
    sn3DMeshModel * model = new sn3DMeshModel;
    sn3DMeshData * mesh = model->AddMesh();
    _bi = new MeshImporter(model);

    setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
    //std::string::size_type dot = _filename.find_last_of("\\/");
    //path_ = (dot == std::string::npos)? "./": std::string(_filename.substr(0,dot+1));
  
    FILE * fp = fopen(_filename.c_str(),"rb");
    ////////////////////////////////////////////////////读文件修改这里
	  
	int index;
	Point3f point;
	//float x_max=-1e5;
	//float x_min=1e5;
		
	while (!feof(fp))
	{
	  fscanf(fp,"%d",&index);
	  fscanf(fp,"%f",&point.X());
	  fscanf(fp,"%f",&point.Y());
	  fscanf(fp,"%f",&point.Z());
   
	  mesh->AddVertex(point);
	}
	
	fclose(fp);
		
	int pos =  _filename.find('.')-4;
    std::string path = _filename.substr(0,pos);
	path += std::string("face.dat");

	fp = fopen(path.c_str(),"rb");
    ////////////////////////////////////////////////////读文件修改这里
		
	int v0,v1,v2;
	while (!feof(fp))
	{
	  fscanf(fp,"%d",&index);
	  fscanf(fp,"%d",&v0);
	  fscanf(fp,"%d",&v1);
	  fscanf(fp,"%d",&v2);

	  mesh->AddFace(v0-1,v1-1,v2-1);
	}

	///////////////////////////////////

    _bi->Update();

    setlocale(LC_ALL,"C");//还原

    return true;
}
//bool ImporterDat::read(std::string& _filename, BaseImporter& _bi)
//{
//  setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
//
//  std::string::size_type dot = _filename.find_last_of("\\/");
//
//  path_ = (dot == std::string::npos)? "./": std::string(_filename.substr(0,dot+1));
//  
//  FILE * fp;
//
//  fp = fopen(_filename.c_str(),"rt");
//  
//  bool result = read(fp,_bi);
//
//  fclose(fp);
//
//  _bi.Update();
//
//  setlocale(LC_ALL,"C");//还原
//
//  return result;
//}
//
//bool ImporterDat::read(FILE * fp, BaseImporter& _bi)
//{
//  std::string keyWrd;
//  keyWrd.resize(64);
//  float    x, y, z, u, v;
//  std::vector<int>     vIndices;//vertex index
//  std::vector<Point2f> faceCoords;
//  std::vector<Point2f> texcoords;
//  std::string    matname;
//
//  while(!feof(fp))
//  {
//	fscanf(fp,"%s",keyWrd.c_str());
//  
//	if(strcmp(keyWrd.c_str(),"mtllib")==0)  // material file
//	{
//      std::string matFile;
//	  matFile.resize(1024);
//	  fscanf(fp,"%s",matFile.c_str());
//
//      matFile = path_ + matFile;
//      std::fstream matStream( matFile.c_str(), std::ios_base::in );
//
//      if(matStream)
//	  {
//        if (!read_material( matStream,matname ))
//        matStream.close();
//      }
//
//      if (materials_.find(matname)!=materials_.end())
//	  {
//		size_t found = materials_.find(matname)->second.texture.find('.');
//		string ext( materials_.find(matname)->second.texture.substr(found+1,found+3));
//
//		//if(ext=="jpg"||ext=="JPG")
//		//{
//		// char str[300];
//		// for(int i=0;i<300;i++)str[i] = 0;
//		// sprintf(str,"%s",materials_.find(matname)->second.texture.c_str());
//		//
//		// unsigned char * data=0;
//		// int w,h;
//		// int dw,dh;
//
//		// string temp = path_ + str;
//		// sn3DImageLoad::sn3DImageData<unsigned char> imagedata;
//		// sn3DImageLoad::sn3DLoadJpeg(temp.c_str(),imagedata);
//
//		// if(imagedata.m_data!=0)
//		// {
//	 //     data=imagedata.m_data;
//	 //     w=imagedata.m_w;
//	 //     h=imagedata.m_h;
//		//
//  //        dw=1024;
//		//  dh=1024;  
//	 //     CImageConvert::ImageFlip(imagedata.m_data,w,h);	
//	 //     
//		//  data = new BYTE[dw*dh*3];
//		//  CImageConvert::ImageNearestScale(data,dw,dh,imagedata.m_data,w,h);
//
//		//  unsigned int texId;
//		//  glEnable(GL_TEXTURE_2D);
//		//  glGenTextures(1,&texId);  
//		//  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//		//  glBindTexture(GL_TEXTURE_2D,texId);
//		//  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
//		//  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
//		//  glTexImage2D(GL_TEXTURE_2D, 0, 3,dw,dh, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE,data);
//		//  DWORD yy = glGetError();
//		//  glDisable(GL_TEXTURE_2D);
//		//  _bi.SetTexId(texId);
//		//  _bi.m_imageChannel = imagedata.m_channel;
//		//  _bi.m_w = dw;
//		//  _bi.m_h = dh;
//
//		//  CImageConvert::ImageFlip(data,dw,dh); // bmp format	
//		//  _bi.m_imageData =  data;
//		//  }
//		//}
//		matname="";
//	  }
//
//	}
//	else if (strcmp(keyWrd.c_str(),"v")==0)
//    {
//	  fscanf(fp,"%f",&x);
//	  fscanf(fp,"%f",&y);
//	  fscanf(fp,"%f",&z);
//      _bi.AddVertex(Point3f(x,y,z));
//    }
//	else if (strcmp(keyWrd.c_str(),"vt")==0)// texture coord
//    {
//	  fscanf(fp,"%f",&u);
//	  fscanf(fp,"%f",&v);
//      if(v < 0) v = 1.0+v;
//      texcoords.push_back(Point2f(u, v));
//    }
//	else if (strcmp(keyWrd.c_str(),"vn")==0)// normal
//    {
//	  fscanf(fp,"%f",&x);
//	  fscanf(fp,"%f",&y);
//	  fscanf(fp,"%f",&z);
//	  _bi.AddVertexNormal(Point3f(x,y,z));
//    }
//	else if (strcmp(keyWrd.c_str(),"f")==0) // face
//	{
//      int component(0);
//      int value;
//
//      vIndices.clear();
//	  faceCoords.clear();
//	  std::string element;
//	  element.resize(64);
//	  
//	  std::string str;
//	  str.resize(2048);
//	  fgets((char*)str.c_str(),2048,fp);
//
//	  while(sscanf(str.c_str(),"%s",element.c_str())!=EOF)
//	  {
//		size_t found1=strlen(element.c_str());
//	   do{//get the component (vertex/texCoord/normal)
//          size_t found=element.find("/");
//          if( found != std::string::npos)
//		  {
//			sscanf(element.c_str(),"%d",&value);
//            if (element.substr(0,found).empty()) 
//			{
//              element = element.substr(found+1);
//              ++component;
//              continue;
//            }
//            element = element.substr(found+1);
//          }
//		  else 
//		  {
//			sscanf(element.c_str(),"%d",&value);
//            element="";
//          }
//
//          switch (component)
//          {
//            case 0: // vertex
//            if ( value < 0 )
//			value = _bi.n_vertices() + value + 1;  
//            vIndices.push_back(value-1);
//            break;
//            case 1: // texture coord
//            if ( value < 0 ) 
//			value = _bi.n_vertices() + value + 1;
//            faceCoords.push_back(texcoords[value-1]);
//            break;
//            case 2: // normal
//            break;
//          }
//
//          ++component;
//        }while(!element.empty());
//
//	   element.resize(64);
//       component = 0;	
//	   str = str.substr(found1+1);
//	  }
//
//      size_t n_faces = _bi.n_faces();
//	  
//	  if(vIndices.size()>=3)
//	  {
//	   int size = vIndices.size();
//	   for(int i=0;i<size-2;i++)
//	   {
//	    bool mult = false;
//		if(size>3)mult=true;
//		_bi.AddFace(vIndices[0],vIndices[i+1],vIndices[i+2],mult);
//	   }
//	  }
//	  if(faceCoords.size()>=3)
//	  {
//	   int size = vIndices.size();
//	   for(int i=0;i<size-2;i++)
//	   _bi.AddFaceTexCoord(faceCoords[0],faceCoords[i+1],faceCoords[i+2]);
//	  }
//	}
//	else
//	{
//	 char str[2048];
//     fgets(str,2048,fp);
//	}
//  }
//  return true;
//}

