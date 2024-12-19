#include "stdafx.h"
#include "Importer3ds.h"
#include <cctype>
#include <string.h>
#include <sstream>
#include<locale.h>
#include"../MeshImporter.h"
using namespace std;




using namespace sn3DCore;
Importer3ds::Importer3ds()
{ 
 
}
bool Importer3ds::read(std::string& _filename,BaseImporter * & _bi)
{
	sn3DMeshModel * model = new sn3DMeshModel;
	model->AddMesh();
	_bi = new MeshImporter(model);
	  setlocale(LC_ALL,"Chinese-simplified");//设置中文环境

  std::fstream in( _filename.c_str(), std::ios_base::in );//open file

  if (!in)
  {
    //MessageBoxA(NULL, "cannot not open file ","",MB_OK);
    return false;
  }

   std::string::size_type dot = _filename.find_last_of("\\/");

   path_ = (dot == std::string::npos)? "./": std::string(_filename.substr(0,dot+1));
  
   bool result = m_reader.Read3DSFile(_filename,_bi);

  _bi->Update();

  in.close();//close file

  setlocale(LC_ALL,"C");//还原

	return true;
}
bool Importer3ds::read(std::string& _filename, BaseImporter& _bi)
{
  setlocale(LC_ALL,"Chinese-simplified");//设置中文环境

  std::fstream in( _filename.c_str(), std::ios_base::in );//open file

  if (!in)
  {
    //MessageBoxA(NULL, "cannot not open file ","",MB_OK);
    return false;
  }

   std::string::size_type dot = _filename.find_last_of("\\/");

   path_ = (dot == std::string::npos)? "./": std::string(_filename.substr(0,dot+1));
  
   bool result = m_reader.Read3DSFile(_filename,&_bi);

  _bi.Update();

  in.close();//close file

  setlocale(LC_ALL,"C");//还原

  return result;
}
