#include"stdafx.h"
#include"ImporterAsc.h"
#include"../PointsImporter.h"
#include<fstream>
#include <sstream>

namespace sn3DCore
{

ImporterAsc::ImporterAsc()
{

}

bool ImporterAsc::read(const std::wstring& _filename,BaseImporter * & _bi)
{
    sn3DPointsModel * model = new sn3DPointsModel;
	model->AddPointCloud();
    _bi = new PointsImporter(model);
       	
	float x,y,z;
	float r,g,b;
	std::string line;
	setlocale(LC_ALL,"Chinese-simplified");

	
	std::fstream in( _filename.c_str(), std::ios_base::in);	
	setlocale(LC_ALL,"C");	
	if (!in)
	{
	  return false;
	}
	int index;
	while( in && !in.eof() )
	{
	  std::getline(in,line);
	  std::stringstream stream(line);
	  stream >> x;	
	  stream >> y;	
	  stream >> z;
	  stream >> r;	
	  stream >> g;	
	  stream >> b;
	  _bi->AddVertex(Point3f(x,y,z));
	  _bi->AddVertexNormal(Point3f(r,g,b));
	}

	in.close();//close file
	_bi->Update();

	return true;
}
	
bool ImporterAsc::read(const std::wstring& _filename, BaseImporter& _bi)
	{
		float x,y,z;
		std::string line;
		setlocale(LC_ALL,"Chinese-simplified");

		std::fstream in( _filename.c_str(), std::ios_base::in );

		setlocale(LC_ALL,"C");

		if (!in)
		{
			return false;
		}

		while( in && !in.eof() )
		{
			std::getline(in,line);
			std::stringstream stream(line);

			stream >> x;
			stream >> y;
			stream >> z;
			_bi.AddVertex(Point3f(x,y,z));

		}
		in.close();//close file

		_bi.Update();

		return true;
	}
}