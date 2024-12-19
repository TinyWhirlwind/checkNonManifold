#include"stdafx.h"
#include"ObjWriter.h"
#include<fstream>

namespace sn3DCore
{
    OBJWriter::OBJWriter()
    {

    }
    bool OBJWriter::write(const std::wstring&, BaseExporter&, float *)
    {
        return true;
    }
    bool OBJWriter::write(const std::wstring& _filename, BaseExporter& _be)
    {
        setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
        std::wfstream _out(_filename.c_str(), std::ios_base::out);

        if(!_out)
        {
            return false;
        }

        ///  写入文件头
        _out << "# " << _be.n_vertices() << " vertices, ";
        _out << _be.n_faces() << " faces" << std::endl;

        // 写入顶点
        for (int i=0, nV=_be.n_vertices(); i<nV; ++i)
        {
            Point3f point = _be.point(i);
            _out << "v " << point[0] <<" "<< point[1] <<" "<< point[2] << std::endl;
        }
        if(_be.n_normals()!=0)
            for (int i=0, nV=_be.n_vertices(); i<nV; ++i)
            {
                Point3f n = _be.VertexNormal(i);
                _out << "vn " << n[0] <<" "<< n[1] <<" "<< n[2] << std::endl;
            }
        if (_be.n_texcoords()!=0)
            for (int i=0, nV=_be.n_vertices(); i<nV; ++i)
            {
                Point2f t = _be.VertexTexcoord(i);
                _out << "vt " << t[0] <<" "<< t[1] << std::endl;
            }

        int lastMat = -1;

        // 写入面
        for (int i=0, nF=_be.n_faces(); i<nF; ++i)
        {
            _out << "f";

            if(!_be.IsMultPolygon(i))
            {
                std::vector<int> vIndices;
                _be.GetVertices(i,vIndices);
                for (int j=0; j< (int)vIndices.size(); ++j)
                {
                    int idx = vIndices[j] + 1;
                    _out << " " << idx;
                    if (_be.n_texcoords()!=0)
                        _out << "/" << idx;
                    if (_be.n_normals()!=0)
                    {
                        if(_be.n_texcoords()==0)
                            _out << "/";
                        _out << "/" << idx;
                    }
                }
            }
            else
            {
                bool flag = false;

                std::vector<int> pIndices;
                pIndices.resize(3);
                pIndices[0] = -1;
                pIndices[1] = -1;
                pIndices[2] = -1;

                do
                {
                    std::vector<int> vIndices;
                    _be.GetVertices(i,vIndices);

                    if(pIndices[0]==-1&&pIndices[1]==-1&&pIndices[2]==-1)
                    {
                        for (int j=0; j< (int)vIndices.size(); ++j)
                        {
                            int idx = vIndices[j] + 1;
                            _out << " " << idx;
                            if (_be.n_texcoords()!=0)
                                _out << "/" << idx;
                            if (_be.n_normals()!=0)
                            {
                                if(_be.n_texcoords()==0)
                                    _out << "/";
                                _out << "/" << idx;
                            }
                        }
                        pIndices[0]=vIndices[0];
                        pIndices[1]=vIndices[1];
                        pIndices[2]=vIndices[2];
                        flag = true;
                        i++;
                    }
                    else
                    {
                        if(_be.IsMultPolygon(i)&&pIndices[0]==vIndices[0]&&pIndices[2]==vIndices[1])
                        {
                            int idx = vIndices[2] + 1;
                            _out << " " << idx;
                            if (_be.n_texcoords()!=0)
                                _out << "/" << idx;
                            if (_be.n_normals()!=0)
                            {
                                if(_be.n_texcoords()==0)
                                    _out << "/";
                                _out << "/" << idx;
                            }
                            pIndices[0]=vIndices[0];
                            pIndices[1]=vIndices[1];
                            pIndices[2]=vIndices[2];
                            i++;
                            flag = true;
                        }
                        else
                        {
                            flag = false;
                            i--;
                        }
                    }
                }while(flag);
            }
            _out << std::endl;
        }
        _out.close();
        setlocale(LC_ALL,"C");//还原
        return true;
    }
}
