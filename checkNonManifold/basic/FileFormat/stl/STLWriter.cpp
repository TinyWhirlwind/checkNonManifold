#include"stdafx.h"
#include"STLWriter.h"

#pragma warning(disable:4996)
namespace sn3DCore
{
	STLWriter::STLWriter()
	{

	}

	bool STLWriter::write(const std::wstring& filename, BaseExporter& _be)
	{

		FILE * fp;

		fp = _wfopen(filename.c_str(),L"wb");
		if (!fp)
			return false;

		int faceNum = _be.n_faces();

		fseek(fp, 80, SEEK_SET);

		fwrite(&faceNum, sizeof(int), 1, fp);


		std::vector<int> vIndices;

		for(int k=0;k<faceNum;k++)
		{
			short attr=0;
			float norm[3];
			float position[3];

			norm[0] = 0.0;
			norm[1] = 0.0;
			norm[2] = 0.0;

			fwrite(norm,sizeof(float),3,fp);

			_be.GetVertices(k,vIndices);

			position[0] = _be.point(vIndices[0]).X();
			position[1] = _be.point(vIndices[0]).Y();
			position[2] = _be.point(vIndices[0]).Z();

			fwrite(position,sizeof(float),3,fp);

			position[0] = _be.point(vIndices[1]).X();
			position[1] = _be.point(vIndices[1]).Y();
			position[2] = _be.point(vIndices[1]).Z();

			fwrite(position,sizeof(float),3,fp);

			position[0] = _be.point(vIndices[2]).X();
			position[1] = _be.point(vIndices[2]).Y();
			position[2] = _be.point(vIndices[2]).Z();

			fwrite(position,sizeof(float),3,fp);

			fwrite(&attr,2,1,fp); 
		}

		fclose(fp);

		return true;

	}
	bool STLWriter::write(const std::wstring& filename, BaseExporter& _be,float * mat)
	{

		FILE * fp;

		fp = _wfopen(filename.c_str(),L"wb");

		int faceNum = _be.n_faces();

		fseek(fp, 80, SEEK_SET);

		fwrite(&faceNum, sizeof(int), 1, fp);


		std::vector<int> vIndices;

		for(int k=0;k<faceNum;k++)
		{
			short attr=0;
			float norm[3];
			float position[3];
			float point[3];

			norm[0] = 0.0;
			norm[1] = 0.0;
			norm[2] = 0.0;

			fwrite(norm,sizeof(float),3,fp);

			_be.GetVertices(k,vIndices);

			position[0] = _be.point(vIndices[0]).X();
			position[1] = _be.point(vIndices[0]).Y();
			position[2] = _be.point(vIndices[0]).Z();

			point[0] = mat[0]*position[0]+mat[1]*position[1]+mat[2]*position[2] +mat[3];
			point[1] = mat[4]*position[0]+mat[5]*position[1]+mat[6]*position[2] +mat[7];
			point[2] = mat[8]*position[0]+mat[9]*position[1]+mat[10]*position[2]+mat[11];

			fwrite(point,sizeof(float),3,fp);

			position[0] = _be.point(vIndices[1]).X();
			position[1] = _be.point(vIndices[1]).Y();
			position[2] = _be.point(vIndices[1]).Z();

			point[0] = mat[0]*position[0]+mat[1]*position[1]+mat[2]*position[2] +mat[3];
			point[1] = mat[4]*position[0]+mat[5]*position[1]+mat[6]*position[2] +mat[7];
			point[2] = mat[8]*position[0]+mat[9]*position[1]+mat[10]*position[2]+mat[11];

			fwrite(point,sizeof(float),3,fp);

			position[0] = _be.point(vIndices[2]).X();
			position[1] = _be.point(vIndices[2]).Y();
			position[2] = _be.point(vIndices[2]).Z();

			point[0] = mat[0]*position[0]+mat[1]*position[1]+mat[2]*position[2] +mat[3];
			point[1] = mat[4]*position[0]+mat[5]*position[1]+mat[6]*position[2] +mat[7];
			point[2] = mat[8]*position[0]+mat[9]*position[1]+mat[10]*position[2]+mat[11];

			fwrite(point,sizeof(float),3,fp);

			fwrite(&attr,2,1,fp); 
		}

		fclose(fp);

		return true;
	}
}