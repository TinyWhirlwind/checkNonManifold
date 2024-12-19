#include"stdafx.h"
#include"ASCWriter.h"

#pragma warning(disable:4996)
namespace sn3DCore
{

	ASCWriter::ASCWriter()
	{

	}

	bool ASCWriter::write(const std::string& filename, BaseExporter& _be)
	{
		FILE * fp;

		fp = fopen(filename.c_str(),"wb");

		for (int j=0;j<(int)_be.n_vertices();j++)
		{
			Point3f point;
			point = _be.point(j);		
			fprintf(fp,"%f %f %f \n",point.X(),point.Y(),point.Z());
		}

		fclose(fp);

		return true;

	}
	bool ASCWriter::write(const std::string& filename, BaseExporter& _be, float *mat)
	{

		FILE * fp;

		fp = fopen(filename.c_str(),"wb");

		for (int j=0;j<(int)_be.n_vertices();j++)
		{
			Point3f point;
			Point3f p; 
			p = _be.point(j);		
			point.X() = mat[0]*p.X()+mat[1]*p.Y()+mat[2]*p.Z()+mat[3];
			point.X() = mat[4]*p.X()+mat[5]*p.Y()+mat[6]*p.Z()+mat[7];
			point.X() = mat[8]*p.X()+mat[9]*p.Y()+mat[10]*p.Z()+mat[11];

			fprintf(fp,"%f %f %f \n",point.X(),point.Y(),point.Z());
		}

		fclose(fp);

		return true;

	}
}