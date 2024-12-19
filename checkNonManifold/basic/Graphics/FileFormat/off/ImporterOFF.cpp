#include"stdafx.h"
#include"Importeroff.h"
#include"../MeshImporter.h"
#include<fstream>
#include <sstream>

namespace sn3DCore
{
ImporterOFF::ImporterOFF()
{

}
bool ImporterOFF::read(std::string& _filename,BaseImporter * & _bi)
{
    sn3DMeshModel * model = new sn3DMeshModel;
    model->AddMesh();
    _bi = new MeshImporter(model);
	   
	setlocale(LC_ALL,"Chinese-simplified");
	FILE* input = fopen(_filename.c_str(), "r");
    char off[10];
    int junkint;
    int NUMBFACES;
    int NUMBPTS;
    fscanf(input, "%s\n", off);
    fscanf(input, "%d %d %d\n", &NUMBPTS, &NUMBFACES, &junkint);
    //cout << NUMBPTS << " " << NUMBFACES << endl;
    float x, y, z;
    int   i, j, k;
    int numb = 0;
    while (numb < NUMBPTS)
	{
     fscanf(input, "%f %f %f\n", &x, &y, &z);

	 Point3f point;
	 point[0] = x;
	 point[1] = y;
	 point[2] = z;
	 _bi->AddVertex(point);
     numb++;
    }

    numb = 0;	  
    while (numb < NUMBFACES)
	{    
	 char junk[5];
     fscanf(input, "%s %d %d %d\n", junk, &i, &j, &k);    
	 _bi->AddFace(i,j,k);
	 numb++;
    }
    fclose(input);
	setlocale(LC_ALL,"C");
	   
	_bi->Update();

	return true;
	}
	
bool ImporterOFF::read(std::string& _filename, BaseImporter& _bi)
{
   
	setlocale(LC_ALL,"Chinese-simplified");
	FILE* input = fopen(_filename.c_str(), "r");
    char off[10];
    int junkint;
    int NUMBFACES;
    int NUMBPTS;
    fscanf(input, "%s\n", off);
    fscanf(input, "%d %d %d\n", &NUMBPTS, &NUMBFACES, &junkint);
    //cout << NUMBPTS << " " << NUMBFACES << endl;
    float x, y, z;
    int   i, j, k;
    int numb = 0;
    while (numb < NUMBPTS)
	{
     fscanf(input, "%f %f %f\n", &x, &y, &z);

	 Point3f point;
	 point[0] = x;
	 point[1] = y;
	 point[2] = z;
	 _bi.AddVertex(point);
     numb++;
    }

    numb = 0;	  
    while (numb < NUMBFACES)
	{    
	 char junk[5];
     fscanf(input, "%s %d %d %d\n", junk, &i, &j, &k);    
	 _bi.AddFace(i,j,k);
	 numb++;
    }
    fclose(input);
	setlocale(LC_ALL,"C");
	   
	_bi.Update();

	return true;
}
}