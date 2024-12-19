#include"stdafx.h"
#include "ImporterStl.h"
#include"../MeshImporter.h"
#pragma warning(disable:4996)

namespace sn3DCore
{

	bool ImporterStl::read(const std::wstring& _filename,BaseImporter * & _bi)
	{
	   sn3DMeshModel * model = new sn3DMeshModel;
	   model->AddMesh();
       _bi = new MeshImporter(model);
 
	    FILE *fp;
		bool binary=false;
		fp = _wfopen(_filename.c_str(), L"r");
		if(fp == NULL) return false;

		fseek(fp, 0, SEEK_END);

		int file_size = ftell(fp);
		int facenum;

		/////////  检查文件类型  /////////

		fseek(fp, STL_LABEL_SIZE, SEEK_SET);
		fread(&facenum, sizeof(int), 1, fp);
		int expected_file_size = STL_LABEL_SIZE + 4 + (sizeof(short)+sizeof(STLFacet))*facenum ;
		if(file_size ==  expected_file_size) binary = true;

		unsigned char tmpbuf[128];
		fread(tmpbuf,sizeof(tmpbuf),1,fp);
		for(unsigned int i = 0; i < sizeof(tmpbuf); i++)
		{
			if(tmpbuf[i] > 127)
			{
				binary=true;
				break;
			}
		}
		fclose(fp);

		int flag;

		if(binary)flag =  OpenBinary(*_bi,_filename.c_str());
		else flag= OpenAscii(*_bi,_filename.c_str());

		//_bi->SetRedundant();	//放到外部调用
		_bi->Update();

		if(flag==0)
			return true;
		else return false;


	}
	bool ImporterStl::read(const std::wstring& _filename, BaseImporter& _bi)
	{
		FILE *fp;
		bool binary=false;
		fp = _wfopen(_filename.c_str(), L"r");
		if(fp == NULL) return false;

		fseek(fp, 0, SEEK_END);

		int file_size = ftell(fp);
		int facenum;

		/////////  检查文件类型  /////////

		fseek(fp, STL_LABEL_SIZE, SEEK_SET);
		fread(&facenum, sizeof(int), 1, fp);
		int expected_file_size = STL_LABEL_SIZE + 4 + (sizeof(short)+sizeof(STLFacet))*facenum ;
		if(file_size ==  expected_file_size) binary = true;

		unsigned char tmpbuf[128];
		fread(tmpbuf,sizeof(tmpbuf),1,fp);
		for(unsigned int i = 0; i < sizeof(tmpbuf); i++)
		{
			if(tmpbuf[i] > 127)
			{
				binary=true;
				break;
			}
		}
		fclose(fp);

		int flag;

		if(binary)flag =  OpenBinary(_bi,_filename.c_str());
		else flag= OpenAscii(_bi,_filename.c_str());

		//_bi.SetRedundant();	//放到外部调用
		_bi.Update();

		if(flag==0)
			return true;
		else return false;
	}
	int  ImporterStl::OpenBinary(BaseImporter& _bi, const TCHAR * filename)
	{
		FILE *fp;
		fp = _wfopen(filename, L"rb");
		if(fp == NULL)
		{
			return E_CANTOPEN;
		}

		int facenum;
		fseek(fp, STL_LABEL_SIZE, SEEK_SET);
		fread(&facenum, sizeof(int), 1, fp);

		_bi.ReserveFace(facenum);

		_bi.ReserveVertex(facenum*3);

		for(int i=0;i<facenum;++i)
		{
			short attr;
			Point3f norm;
			Point3f tri[3];
			fread(&norm,sizeof(Point3f),1,fp);
			fread(&tri,sizeof(Point3f),3,fp);
			fread(&attr,sizeof(short),1,fp);
			for(int k=0;k<3;++k)
			{
				_bi.AddVertex(tri[k]);
			}
			_bi.AddFace(i*3,i*3+1,i*3+2);

		}
		fclose(fp);
		return E_NOERROR;
	}
	int  ImporterStl::OpenAscii(BaseImporter& _bi, const TCHAR * filename)
	{
		FILE *fp;
		fp = _wfopen(filename, L"r");
		if(fp == NULL) return E_CANTOPEN;

		long currentPos = ftell(fp);
		fseek(fp,0L,SEEK_END);
		long fileLen = ftell(fp);
		fseek(fp,currentPos,SEEK_SET);

		while(getc(fp) != '\n');

		STLFacet f;
		int cnt=0;
		int ret;

		while(!feof(fp))
		{
			ret = fscanf(fp, "%*s %*s %f %f %f\n", &f.n.X(), &f.n.Y(), &f.n.Z());
			if(ret!=3) 
			{
				// we could be in the case of a multiple solid object, where after a endfaced instead of another facet we have to skip two lines:
				//     endloop
				//	 endfacet
				//endsolid     <- continue on ret==0 will skip this line
				//solid ascii  <- and this one.
				//   facet normal 0.000000e+000 7.700727e-001 -6.379562e-001
				continue; 
			}
			ret=fscanf(fp, "%*s %*s");
			ret=fscanf(fp, "%*s %f %f %f\n", &f.v[0].X(),  &f.v[0].Y(),  &f.v[0].Z());
			if(ret!=3) 
				return E_UNESPECTEDEOF;
			ret=fscanf(fp, "%*s %f %f %f\n", &f.v[1].X(),  &f.v[1].Y(),  &f.v[1].Z());
			if(ret!=3) 
				return E_UNESPECTEDEOF;
			ret=fscanf(fp, "%*s %f %f %f\n", &f.v[2].X(),  &f.v[2].Y(),  &f.v[2].Z());
			if(ret!=3) 
				return E_UNESPECTEDEOF;
			ret=fscanf(fp, "%*s"); // end loop
			ret=fscanf(fp, "%*s"); // end facet
			if(feof(fp)) break;


			for(int k=0;k<3;++k)
			{
				_bi.AddVertex(f.v[k]);
			}    

			_bi.AddFace(cnt,cnt+1,cnt+2);

			cnt++;
		}
		fclose(fp);
		return E_NOERROR;
	}
}

