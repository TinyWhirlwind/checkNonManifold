#include "stdafx.h"
#include "ImporterWrl.h"
#include <cctype>
#include <string.h>
#include <sstream>
#include<locale.h>
#include"../MeshImporter.h"
using namespace std;

#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

using namespace sn3DCore;
#pragma warning (disable : 4996)

ImporterWrl::ImporterWrl()
{ 
 
}
bool ImporterWrl::read(std::string& _filename,BaseImporter * & _bi)
{
  sn3DMeshModel * model = new sn3DMeshModel;
  model->AddMesh();
  _bi = new MeshImporter(model);

  setlocale(LC_ALL,"Chinese-simplified");//设置中文环境

  {
    std::string::size_type dot = _filename.find_last_of("\\/");

    path_ = (dot == std::string::npos)? "./": std::string(_filename.substr(0,dot+1));
  }

  bool result;

  float version=-1.0;
  FILE * fp = fopen(_filename.c_str(),"r");

  fscanf(fp,"#VRML V%f",&version);
 

  fclose(fp);

  if(version==1.0)
  {
  result = m_wrl1Reader.read(_filename, *_bi);
  
  }
  else if(version==2.0)
  {

  }

  _bi->Update();

  setlocale(LC_ALL,"C");//还原

  return result;


}
bool ImporterWrl::read(std::string& _filename, BaseImporter& _bi)
{
  setlocale(LC_ALL,"Chinese-simplified");//设置中文环境

  {
    std::string::size_type dot = _filename.find_last_of("\\/");

    path_ = (dot == std::string::npos)? "./": std::string(_filename.substr(0,dot+1));
  }

  bool result;

  float version=-1.0;
  FILE * fp = fopen(_filename.c_str(),"r");

  fscanf(fp,"#VRML V%f",&version);
 

  fclose(fp);

  if(version==1.0)
  {
  result = m_wrl1Reader.read(_filename, _bi);
  
  }
  else if(version==2.0)
  {

  }

  _bi.Update();

  setlocale(LC_ALL,"C");//还原

  return result;
}
