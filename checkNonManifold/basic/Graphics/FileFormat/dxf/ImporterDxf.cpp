#include "stdafx.h"
#include "ImporterDxf.h"
#include"../PointsImporter.h"
#include <cctype>
#include <string.h>
#include <sstream>
#include<locale.h>
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif
using namespace std;
using namespace sn3DCore;

#pragma warning (disable : 4996)

ImporterDxf::ImporterDxf()
{ 
 
}
bool ImporterDxf::read(std::string& _filename,BaseImporter * & _bi)
{
  sn3DPointsModel * model = new sn3DPointsModel;
  model->AddPointCloud();
  _bi = new PointsImporter(model);

  setlocale(LC_ALL,"Chinese-simplified");//设置中文环境

  {
    std::string::size_type dot = _filename.find_last_of("\\/");

    path_ = (dot == std::string::npos)? "./": std::string(_filename.substr(0,dot+1));
  }

   bool result = true;

   FILE * fp;
	
	fp = fopen(_filename.c_str(),"rt");
	string str;
	str.resize(250);

	fscanf(fp,"%s",&str[0]);

	if(strcmp(str.c_str(),"0")!=0)
	{
	  fclose(fp);
	  return false;
	}

	fscanf(fp,"%s",&str[0]);
	if( strcmp(str.c_str(),"SECTION")!=0)
	{
		fclose(fp);
		return false;
	}
	
	fscanf(fp,"%s",&str[0]);
	if( strcmp(str.c_str(),"2")!=0)
	{
		fclose(fp);
		return false;
	}

	while(feof(fp)==0)
	{
     fscanf(fp,"%s",&str[0]);
	 if(strcmp(str.c_str(),"ENTITIES")==0)
	 {
      break;
	 }
	}
	if(feof(fp)!=0)
	{
     fclose(fp);
	 return false;
	}

	while( 1 )
	{
	  fscanf(fp,"%s",&str[0]);
	  if(strcmp(str.c_str(),"ENDSEC")==0) break;
	  else if(strcmp(str.c_str(),"POINT")==0)
	  {
	   Point3f point;
	   while(1)
	   {
		fscanf(fp,"%s",&str[0]);
		if(strcmp(str.c_str(),"0")==0)
		{
		  _bi->AddVertex(point);	
		  break;
		}
		else if(strcmp(str.c_str(),"10")==0)	
		{	
		 fscanf(fp,"%f",&point.X());
		}
		else if(strcmp(str.c_str(),"20")==0)	
		{	
		  fscanf(fp,"%f",&point.Y());
		}					
		else if(strcmp(str.c_str(),"30")==0)	
		{	
		  fscanf(fp,"%f",&point.Z());
		}
	}
	
	  }
	}

	fclose(fp);

  _bi->Update();

  setlocale(LC_ALL,"C");//还原

  return result;
	
}
bool ImporterDxf::read(std::string& _filename, BaseImporter& _bi)
{
  setlocale(LC_ALL,"Chinese-simplified");//设置中文环境

  {
    std::string::size_type dot = _filename.find_last_of("\\/");

    path_ = (dot == std::string::npos)? "./": std::string(_filename.substr(0,dot+1));
  }

   bool result = true;

	FILE * fp;
	
	fp = fopen(_filename.c_str(),"rt");
	string str;
	str.resize(250);

	fscanf(fp,"%s",&str[0]);

	if(strcmp(str.c_str(),"0")!=0)
	{
	  fclose(fp);
	  return false;
	}

	fscanf(fp,"%s",&str[0]);
	if( strcmp(str.c_str(),"SECTION")!=0)
	{
		fclose(fp);
		return false;
	}
	
	fscanf(fp,"%s",&str[0]);
	if( strcmp(str.c_str(),"2")!=0)
	{
		fclose(fp);
		return false;
	}

	while(feof(fp)==0)
	{
     fscanf(fp,"%s",&str[0]);
	 if(strcmp(str.c_str(),"ENTITIES")==0)
	 {
      break;
	 }
	}
	if(feof(fp)!=0)
	{
     fclose(fp);
	 return false;
	}

	while( 1 )
	{
	  fscanf(fp,"%s",&str[0]);
	  if(strcmp(str.c_str(),"ENDSEC")==0) break;
	  else if(strcmp(str.c_str(),"POINT")==0)
	  {
	   Point3f point;
	   while(1)
	   {
		fscanf(fp,"%s",&str[0]);
		if(strcmp(str.c_str(),"0")==0)
		{
		  _bi.AddVertex(point);	
		  break;
		}
		else if(strcmp(str.c_str(),"10")==0)	
		{	
		 fscanf(fp,"%f",&point.X());
		}
		else if(strcmp(str.c_str(),"20")==0)	
		{	
		  fscanf(fp,"%f",&point.Y());
		}					
		else if(strcmp(str.c_str(),"30")==0)	
		{	
		  fscanf(fp,"%f",&point.Z());
		}
	}
	
	  }
	}

	fclose(fp);

  _bi.Update();

  setlocale(LC_ALL,"C");//还原

  return result;
}

