#include "stdafx.h"
#include "ImporterObj.h"
#include <cctype>
#include <string.h>
#include <sstream>
#include<locale.h>
#include<windows.h>
#include<gl\gl.h>
using namespace std;

#include "..\ImageConvert.h"
#include"../MeshImporter.h"
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

#pragma warning (disable : 4996)
using namespace sn3DCore;

ImporterObj::ImporterObj()
{ 

}

bool ImporterObj::read(const std::string& _filename,BaseImporter * & _bi)
{
    sn3DMeshModel * model = new sn3DMeshModel;
    model->AddMesh();
    _bi = new MeshImporter(model);

    setlocale(LC_ALL,"Chinese-simplified");//设置中文环境

    std::string::size_type dot = _filename.find_last_of("\\/");

    path_ = (dot == std::string::npos)? "./": std::string(_filename.substr(0,dot+1));

    FILE * fp;

    fp = fopen(_filename.c_str(),"rt");

    bool result = read(fp,*_bi);

    fclose(fp);

    _bi->Update();

    setlocale(LC_ALL,"C");//还原

    return result;

}

bool ImporterObj::read(const std::string& _filename, BaseImporter& _bi)
{
    setlocale(LC_ALL,"Chinese-simplified");//设置中文环境

    std::string::size_type dot = _filename.find_last_of("\\/");

    path_ = (dot == std::string::npos)? "./": std::string(_filename.substr(0,dot+1));

    FILE * fp;

    fp = fopen(_filename.c_str(),"rt");

    bool result = read(fp,_bi);

    fclose(fp);

    _bi.Update();

    setlocale(LC_ALL,"C");//还原

    return result;
}

bool ImporterObj::read_material(std::fstream& _in,string & str)
{
    std::string line;
    std::string keyWrd;
    std::string key;
    Material    mat;
    float       f1,f2,f3;
    bool        indef = false;
    mat.cleanup();

    while( _in && !_in.eof() )
    {
        std::getline(_in,line);
        if ( _in.bad() )
        {
            return false;
        }

        if ( line.empty() )
            continue;

        std::stringstream stream(line);

        stream >> keyWrd;

        if(line[0] == '#' )
        {
            if (indef && !key.empty() && mat.is_valid())
            {
                materials_[key] = mat;
                mat.cleanup();
            }
        }

        else if (keyWrd == "newmtl") // begin new material definition
        {
            stream >> key;
            indef = true;
        }

        else if (keyWrd == "Kd") // diffuse color
        {
            stream >> f1; stream >> f2; stream >> f3;

            if( !stream.fail() )
                mat.set_Kd(f1,f2,f3);
        }

        else if (keyWrd == "Ka") // ambient color
        {
            stream >> f1; stream >> f2; stream >> f3;

            if( !stream.fail() )
                mat.set_Ka(f1,f2,f3);
        }

        else if (keyWrd == "Ks") // specular color
        {
            stream >> f1; stream >> f2; stream >> f3;
            if( !stream.fail() )
                mat.set_Ks(f1,f2,f3);
        }
        else if (keyWrd == "map_Kd") // map images
        {
            stream >> mat.texture ;
        }
        else if (keyWrd == "Tr") // transparency value
        {
            stream >> f1;

            if( !stream.fail() )
                mat.set_Tr(f1);
        }
        else if (keyWrd == "d") // transparency value
        {
            stream >> f1;

            if( !stream.fail() )
                mat.set_Tr(f1);
        }

        if ( _in && indef && /*mat.is_valid() &&*/ !key.empty())
            materials_[key] = mat;
    }

    str = key; //记录名称

    return true;
}

bool ImporterObj::read(FILE * fp, BaseImporter& _bi)
{
    std::string keyWrd;
    keyWrd.resize(64);
    float    x, y, z, u, v;
    std::vector<int>     vIndices;//vertex index
    std::vector<Point2f> faceCoords;
    std::vector<Point2f> texcoords;
    std::string    matname;

    while(!feof(fp))
    {
        fscanf(fp,"%s",keyWrd.c_str());

        if(strcmp(keyWrd.c_str(),"mtllib")==0)  // material file
        {
            std::string matFile;
            matFile.resize(1024);
            fscanf(fp,"%s",matFile.c_str());

            matFile = path_ + matFile;
            std::fstream matStream( matFile.c_str(), std::ios_base::in );

            if(matStream)
            {
                if (!read_material( matStream,matname ))
                    matStream.close();
            }

            if (materials_.find(matname)!=materials_.end())
            {
                size_t found = materials_.find(matname)->second.texture.find('.');
                string ext( materials_.find(matname)->second.texture.substr(found+1,found+3));

                matname="";
            }
        }
        else if (strcmp(keyWrd.c_str(),"v")==0)
        {
            fscanf(fp,"%f",&x);
            fscanf(fp,"%f",&y);
            fscanf(fp,"%f",&z);
            _bi.AddVertex(Point3f(x,y,z));
        }
        else if (strcmp(keyWrd.c_str(),"vt")==0)// texture coord
        {
            fscanf(fp,"%f",&u);
            fscanf(fp,"%f",&v);
            if(v < 0) v = 1.0+v;
            texcoords.push_back(Point2f(u, v));
        }
        else if (strcmp(keyWrd.c_str(),"vn")==0)// normal
        {
            fscanf(fp,"%f",&x);
            fscanf(fp,"%f",&y);
            fscanf(fp,"%f",&z);
            _bi.AddVertexNormal(Point3f(x,y,z));
        }
        else if (strcmp(keyWrd.c_str(),"f")==0) // face
        {
            int component(0);
            int value;

            vIndices.clear();
            faceCoords.clear();
            std::string element;
            element.resize(64);

            std::string str;
            str.resize(2048);
            fgets((char*)str.c_str(),2048,fp);

            while(sscanf(str.c_str(),"%s",element.c_str())!=EOF)
            {
                size_t found1=strlen(element.c_str());
                do{
                    //get the component (vertex/texCoord/normal)
                    size_t found=element.find("/");
                    if( found != std::string::npos)
                    {
                        sscanf(element.c_str(),"%d",&value);
                        if (element.substr(0,found).empty())
                        {
                            element = element.substr(found+1);
                            ++component;
                            continue;
                        }
                        element = element.substr(found+1);
                    }
                    else
                    {
                        sscanf(element.c_str(),"%d",&value);
                        element="";
                    }

                    switch (component)
                    {
                    case 0: // vertex
                        if ( value < 0 )
                            value = _bi.n_vertices() + value + 1;
                        vIndices.push_back(value-1);
                        break;
                    case 1: // texture coord
                        if ( value < 0 )
                            value = _bi.n_vertices() + value + 1;
                        faceCoords.push_back(texcoords[value-1]);
                        break;
                    case 2: // normal
                        break;
                    }

                    ++component;
                }while(!element.empty());

                element.resize(64);
                component = 0;
                str = str.substr(found1+1);
            }

            size_t n_faces = _bi.n_faces();

            if(vIndices.size()>=3)
            {
                int size = vIndices.size();
                for(int i=0;i<size-2;i++)
                {
                    bool mult = false;
                    if(size>3)mult=true;
                    _bi.AddFace(vIndices[0],vIndices[i+1],vIndices[i+2],mult);
                }
            }
            if(faceCoords.size()>=3)
            {
                int size = vIndices.size();
                for(int i=0;i<size-2;i++)
                    _bi.AddFaceTexCoord(faceCoords[0],faceCoords[i+1],faceCoords[i+2]);
            }
        }
        else
        {
            char str[2048];
            fgets(str,2048,fp);
        }
    }
    return true;
}

