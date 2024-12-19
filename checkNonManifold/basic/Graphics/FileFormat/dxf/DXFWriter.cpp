#include"stdafx.h"
#include"DXFWriter.h"

#pragma warning(disable:4996)
namespace sn3DCore
{

	DXFWriter::DXFWriter()
	{

	}

	bool DXFWriter::write(const std::string& filename, BaseExporter& _be)
	{

	 FILE *f1;
	 if ((f1 = fopen(filename.c_str(),"w")) == NULL)
	 {
		return false;
	 }

     int vertexNo = _be.n_vertices();

	 fprintf(f1,"0\nSECTION\n2\nHEADER\n");

	 fprintf(f1,"9\n$EXTMIN\n");
	 fprintf(f1,"10\n%f\n",0.0);
	 fprintf(f1,"20\n%f\n",0.0);
	 fprintf(f1,"30\n%f\n",0.0);		
	 fprintf(f1,"9\n$EXTMAX\n");
	 fprintf(f1,"10\n%f\n",0.0);
	 fprintf(f1,"20\n%f\n",0.0);
	 fprintf(f1,"30\n%f\n",0.0);	

	 fprintf(f1,"0\n");
		
	 fprintf(f1,"ENDSEC\n");	//empty header section			

	 //no table section,  no block section
	 std::string objectStr="Venus3D";

	 fprintf(f1,"0\nSECTION\n2\nENTITIES\n");		//start of entities section	
			
	 for(int i = 0; i < vertexNo; i++)
	 {
		Point3f point = _be.point(i);
		fprintf(f1,"0\nPOINT\n8\n%s\n",objectStr.c_str());		//object name		
		fprintf(f1,"5\n%d\n",i+1);
		fprintf(f1,"10\n%-7.2f\n20\n%-7.2f\n30\n%-7.2f\n",	//10-x, 20-y, 30-z	
				point[0],point[1],point[2]);
	  }

	fprintf(f1,"0\nENDSEC\n0\nEOF\n");				//end of entities section. end of file
	fclose(f1);

	return true;

	}

}