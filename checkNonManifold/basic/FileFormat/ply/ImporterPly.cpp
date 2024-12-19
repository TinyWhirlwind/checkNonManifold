#include "stdafx.h"

#include "ImporterPly.h"
#include"../MeshImporter.h"

bool ImporterPly::read(const std::wstring& _filename,BaseImporter * & _bi)
{
    sn3DMeshModel * model = new sn3DMeshModel;
    model->AddMesh();
    _bi = new MeshImporter(model);
    PlyInfo pi;

    read(_filename,*_bi,pi);

    _bi->Update();  // 更新模型信息

    return true;
}
bool ImporterPly::read(const std::wstring& _filename, BaseImporter& _bi)
{
    PlyInfo pi;

    read(_filename,_bi,pi);

    _bi.Update();  // 更新模型信息

    return true;
}
int  ImporterPly::read(const std::wstring& _filename,BaseImporter& _bi,PlyInfo &pi)
{
    LoadPly_FaceAux fa;
    LoadPly_VertAux<float> va;
    LoadPly_TristripAux tsa;
    ply::PlyFile pf;
    pi.mask = 0;
    bool multit = false; //是否有多纹理

    va.flags = 42;
    pi.status = ply::E_NOERROR;

    //auto ws2s = [](const std::wstring& ws) ->char*
    //{
    //    _bstr_t t = ws.c_str();
    //    char* pchar = (char*)t;
    //    std::string result = pchar;
    //    char* charTmp = new char;
    //    strcpy(charTmp, result.c_str());
    //    pchar = nullptr;
    //    delete pchar;
    //    return charTmp;
    //};

    // 分析文件
    if(pf.Open(_filename.c_str(), ply::PlyFile::MODE_READ) == -1)
    {
        pi.status = pf.GetError();
        return pi.status;
    }
    pi.header = pf.GetHeader();

    // 文件描述

    if( pf.AddToRead(VertDesc(0))==-1 ) { pi.status = PlyInfo::E_NO_VERTEX; return pi.status; }
    if( pf.AddToRead(VertDesc(1))==-1 ) { pi.status = PlyInfo::E_NO_VERTEX; return pi.status; }
    if( pf.AddToRead(VertDesc(2))==-1 ) { pi.status = PlyInfo::E_NO_VERTEX; return pi.status; }

	if (pf.AddToRead(FaceDesc(0)) == -1) 
	{
		if (pf.AddToRead(FaceDesc(9)) == -1) 
		{
			if (pf.AddToRead(FaceDesc(10)) == -1) 
			{
				if (pf.AddToRead(FaceDesc(11)) == -1) 
				{
					if (pf.AddToRead(FaceDesc(12)) == -1) 
					{
						if (pf.AddToRead(TristripDesc(0)) == -1)
						{
							pi.status = PlyInfo::E_NO_FACE;
							return pi.status;
						}
					}
				}
			}
		}
	}

    if (pf.AddToRead(VertDesc(3)) != -1)
    {
        pi.mask |= Mask::IOM_VERTFLAGS;
    }

    if (pf.AddToRead(VertDesc(12)) != -1
            && pf.AddToRead(VertDesc(13)) != -1
            && pf.AddToRead(VertDesc(14)) != -1)
    {
        pi.mask |= Mask::IOM_VERTNORMAL;
    }

    if( pf.AddToRead(VertDesc(5))!=-1 )
    {
        pf.AddToRead(VertDesc(6));
        pf.AddToRead(VertDesc(7));
        pi.mask |= Mask::IOM_VERTCOLOR;
    }

    if( pf.AddToRead(VertDesc(8))!=-1 )
    {
        pf.AddToRead(VertDesc(9));
        pf.AddToRead(VertDesc(10));
        pi.mask |= Mask::IOM_VERTCOLOR;
    }

    if (pf.AddToRead(FaceDesc(1)) != -1)
    {
        pi.mask |= Mask::IOM_FACEFLAGS;
    }

    if( pf.AddToRead(FaceDesc(6))!=-1 )
    {
        pf.AddToRead(FaceDesc(7));
        pf.AddToRead(FaceDesc(8));
        pi.mask |= Mask::IOM_FACECOLOR;
    }

    if( pf.AddToRead(FaceDesc(4))!=-1)
    {
        pi.mask |= Mask::IOM_WEDGCOLOR;
    }

    // 用户属性
    std::vector<PropDescriptor> VPV(pi.vdn);
    std::vector<PropDescriptor> FPV(pi.fdn);

    if(pi.vdn>0)  ////////  顶点
    {
        size_t totsz=0;
        for(int i=0;i<pi.vdn;i++)
        {
            VPV[i] = pi.VertexData[i];
            VPV[i].offset1=offsetof(LoadPly_VertAux<float>,data)+totsz;
            totsz+=pi.VertexData[i].memtypesize();
            if( pf.AddToRead(VPV[i])==-1 ) { pi.status = pf.GetError(); return pi.status; }
        }
        if(totsz > MAX_USER_DATA)
        {
            pi.status = ply::E_BADTYPE;
            return pi.status;
        }
    }

    if(pi.fdn>0)  ////////  面片
    {
        size_t totsz=0;
        for(int i=0;i<pi.fdn;i++)
        {
            FPV[i] = pi.FaceData[i];
            FPV[i].offset1=offsetof(LoadPly_FaceAux,data)+totsz;
            totsz+=pi.FaceData[i].memtypesize();
            if( pf.AddToRead(FPV[i])==-1 ) { pi.status = pf.GetError(); return pi.status; }
        }
        if(totsz > MAX_USER_DATA)
        {
            pi.status = ply::E_BADTYPE;
            return pi.status;
        }
    }

    //////////////  读取 /////////////////

    for(int i=0;i<int(pf.elements.size());i++)
    {
        int n = pf.ElemNumber(i);

        if( !strcmp( pf.ElemName(i),"vertex" ) )
        {
            int j;

            pf.SetCurElement(i);


            for(j=0;j<n;++j)
            {
                if( pf.Read( (void *)&(va) )==-1 )
                {
                    pi.status = PlyInfo::E_SHORTFILE;
                    return pi.status;
                }

                Point3f point;
				Color4f color = Color4f(0.0f,0.0f,0.0f,1.0f);
                point[0] = va.p[0];
                point[1] = va.p[1];
                point[2] = va.p[2];

                if( pi.mask & Mask::IOM_VERTFLAGS)
                {
                    //(*vi).Flag() = va.flags;
                }
                if( pi.mask & Mask::IOM_VERTNORMAL )
                {
                    //// 读取法线
                }
                if( pi.mask & Mask::IOM_VERTCOLOR )
                {
                    ////// 读取颜色
                   
					color[0] = va.r/255.0;
					color[1] = va.g/255.0;
					color[2] = va.b/255.0;
                    color[3] = 1.0;

                    //_bi.AddVertexColor(color);
                }

				_bi.AddVertex(point, color);
            }
        }
        else if( !strcmp( pf.ElemName(i),"face") && (n>0) )
        {
            int j;

            pf.SetCurElement(i);

            for(j=0;j<n;++j)
            {
                if( pf.Read(&fa)==-1 )
                {
                    pi.status = PlyInfo::E_SHORTFILE;
                    return pi.status;
                }
                if(fa.size!=3) //非三角网格
                {
                    pi.status = PlyInfo::E_NO_3VERTINFACE;
                    return pi.status;
                }

                if(pi.mask & Mask::IOM_FACEFLAGS)
                {
                    //// 标志位
                }

                if( pi.mask & Mask::IOM_FACECOLOR )
                {
                    ///// 面颜色
                }


                /// Now the temporary struct 'fa' is ready to be copied into the real face '*fi'
                /// This loop
                //for(k=0;k<3;++k)
                //{
                //if( fa.v[k]<0 || fa.v[k]>=(int)(m.vn) )
                //{
                //	pi.status = PlyInfo::E_BAD_VERT_INDEX;
                //	return pi.status;
                //}
                //(*fi).V(k) = index[ fa.v[k] ];
                //}
                _bi.AddFace(fa.v[0],fa.v[1],fa.v[2]); // 加入面

                //for(k=0;k<pi.fdn;k++)
                //memcpy((char *)(&(*fi)) + pi.FaceData[k].offset1,(char *)(&fa) + FPV[k].offset1, FPV[k].memtypesize());


                //	for(int qq=0;qq<fa.size-3;++qq)
                //	{
                //(*fi).V(0) = index[ fa.v[0] ];
                //for(k=1;k<3;++k)
                //	{
                //	if( fa.v[2+qq]<0 || fa.v[2+qq]>=(int)(m.vn) )
                //	{
                //		pi.status = PlyInfo::E_BAD_VERT_INDEX;
                //		return pi.status;
                //	}
                //(*fi).V(k) = index[ fa.v[1+qq+k] ];
                //	}

                //for(k=0;k<pi.fdn;k++)
                //	memcpy((char *)(&(*fi)) + pi.FaceData[k].offset1,
                //	(char *)(&fa) + FPV[k].offset1, FPV[k].memtypesize());
                //++fi;
                //	}

            }
        }
        else if( !strcmp( pf.ElemName(i),"tristrips") )
        {
            int j;
            pf.SetCurElement(i);
            for(j=0;j<n;++j)
            {
                int k;
                if( pf.Read(&tsa)==-1 )
                {
                    pi.status = PlyInfo::E_SHORTFILE;
                    return pi.status;
                }
                int remainder=0;

                for(k=0;k<tsa.size-2;++k)
                {
                    if(tsa.v[k+2]==-1)
                    {
                        k+=2;
                        if(k%2) remainder=0;
                        else remainder=1;
                        continue;
                    }

                    if((k+remainder)%2)
                        _bi.AddFace(tsa.v[k+1],tsa.v[k+0],tsa.v[k+2]);
                    else
                        _bi.AddFace(tsa.v[k+0],tsa.v[k+1],tsa.v[k+2]);

                }
            }
        }
        else
        {
            int n = pf.ElemNumber(i);
            pf.SetCurElement(i);
            for(int j=0;j<n;j++)
            {
                if( pf.Read(0)==-1)
                {
                    pi.status = PlyInfo::E_SHORTFILE;
                    return pi.status;
                }
            }
        }
    } /// 读取面

    // Parsing texture names

    //............................

    // vn and fn should be correct but if someone wrongly saved some deleted elements they can be wrong.
    //	m.vn = 0;
    //	VertexIterator vi;
    //	for(vi=m.m_Vertices.begin();vi!=m.m_Vertices.end();++vi)
    //		if( ! (*vi).IsD() )++m.vn;

    //	m.fn = 0;
    //	FaceIterator fi;
    //	for(fi=m.m_Faces.begin();fi!=m.m_Faces.end();++fi)
    //		if( ! (*fi).IsD()) ++m.fn;

    return 0;
}

