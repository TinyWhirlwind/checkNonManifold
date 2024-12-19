#include"stdafx.h"
#include"DATWriter.h"

#pragma warning(disable:4996)
namespace sn3DCore
{
	DATWriter::DATWriter()
	{

	}

	bool DATWriter::write(const std::string& filename, BaseExporter& _be)
	{

	
	  FILE * fp = fopen(filename.c_str(),"wb");

////////////////////////////////////////////////////读文件修改这里

	  size_t vn = _be.n_vertices();
	  size_t fn = _be.n_faces();

	  for(size_t i=0;i<vn;i++)
	  {
		Point3f p =_be.point(i);
	    fprintf(fp,"%d   ",i+1);
		fprintf(fp,"%f   ",p[0]);
		fprintf(fp,"%f   ",p[1]);
		fprintf(fp,"%f   \n",p[2]);
	  }

	 /* std::vector<int> vs;
	  for(size_t i=0;i<fn;i++)
	  {
		_be.GetVertices(i,vs);
	    fprintf(fp,"%d",i+1);
		fprintf(fp,"  %d",vs[0]+1);
		fprintf(fp,"  %d",vs[1]+1);
		fprintf(fp,"  %d\n",vs[2]+1);

		vs.clear();
	  }
		*/
       fclose(fp);

		//int index;
		//int v0,v1,v2;
		//while (!feof(fp))
		//{
		//	fscanf(fp,"%d",&index);
		//	fscanf(fp,"%d",&v0);
		//	fscanf(fp,"%d",&v1);
		//	fscanf(fp,"%d",&v2);

		//	Face face;

		//	face.m_Index = index - 1;

		//	face.V(0) = mesh->V(v0-1);
		//	face.V(1) = mesh->V(v1-1);
		//	face.V(2) = mesh->V(v2-1);

		//	mesh->m_Faces.push_back(face);
		//}

		//mesh->fn = (unsigned int)mesh->m_Faces.size();


		
		return true;

	}
	bool DATWriter::write(const std::string& filename, BaseExporter& _be,float * mat)
	{

		FILE * fp;

		fp = fopen(filename.c_str(),"wb");

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