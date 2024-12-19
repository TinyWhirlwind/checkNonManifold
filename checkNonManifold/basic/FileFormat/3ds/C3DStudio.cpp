#include "stdafx.h"
#include <string.h>
#include"C3DStudio.h"
#pragma warning (disable : 4996)
#pragma warning (disable : 4715)
C3DStudio::C3DStudio()
{
  fp = NULL;
  newLight = NULL;
  newCamera = NULL;
  newSurface = NULL;
}

C3DStudio::~C3DStudio()
{
	
}


bool C3DStudio::Read3DSFile(std::string & file_name,BaseImporter * _bi)
{
	chunk3ds	chunk;
	unsigned long int size;
	unsigned long int position;

	m_importer = _bi;

	if ((fp = fopen(file_name.c_str(), "rb")) != NULL) 
	{
	  position = ftell(fp);	
	  fseek(fp, 0, SEEK_END);
	  size = ftell(fp); //  计算文件大小
	  fseek(fp, 0, SEEK_SET);

	  if (!Is3DSFile()) 
	  {
	   fclose(fp);
	   return false;
	  }		

	  while (position < size ) 
	  {
	    if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
	    if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;
		
		switch (chunk.tag) 
		{
		 case MAIN3DS:
		 if(!ReadMain(position, chunk.len)) 
		 {	
		   fclose(fp);
		   return false;
		 }
		 break;
			
		 default:
		 fseek(fp, position + chunk.len, SEEK_SET);
		}	
		position = ftell(fp);
	  }
		
	  fclose(fp);
	  return true; 
	}
	return false;
}
bool C3DStudio::Is3DSFile()
{
   chunk3ds	chunk;
   long		pos = ftell(fp);

   if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
   if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;

   fseek(fp, pos, SEEK_SET);
   return (chunk.tag == MAIN3DS);
}
bool C3DStudio::ReadMain(long Start, long Size)
{
   long			position = ftell(fp);
   chunk3ds		chunk;
   unsigned long	version;

   while (position < Start + Size) 
   {
	if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
	if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;
		
	switch (chunk.tag) 
	{
	  case M3D_VERSION:
	  if(fread(&version, sizeof(version), 1, fp) != 1)return false;
	  break;
	  case EDIT3DS:
		if (!ReadEdit(position, chunk.len)) return false;
	  break;
		
	  default:	
	  fseek(fp, position + chunk.len, SEEK_SET);
		
	 }
		
	position = ftell(fp);
	}
	return true;
}
bool C3DStudio::ReadEdit(long Start, long Size)
{
	long			position = ftell(fp);
	chunk3ds		chunk;

	while (position < Start + Size) 
	{
	 if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
	 if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;

	 switch (chunk.tag) 
	 {	
	  case MESH_VERSION:
	  if(fread(&m_Version, sizeof(unsigned long), 1, fp) != 1)return false;
	  break;
		
	  case EDIT_MATERIAL:
	  newSurface= new Surface;
	  if (!ReadMaterial(position, chunk.len)) return false;
	  m_surfacelist.push_back(newSurface);
	  break;
		
	  case MASTER_SCALE:
	  if(fread(& m_Scale, sizeof(float), 1, fp) != 1)return false;
	  break;
		
	  case EDIT_OBJECT:
	  if (!ReadObject(position, chunk.len)) return false;
	  break;
		
	  case EDIT_BACKGR:
	  if (!ReadColor(backcolor)) return false;
	  break;

	  case EDIT_AMBIENT:
	  if (!ReadColor(ambient)) return false;
	  break;

	  default:
	  fseek(fp, position + chunk.len, SEEK_SET);
	  }
	  position = ftell(fp);
	}
	return true;
}
bool C3DStudio::ReadObject(long Start, long Size)
{
	chunk3ds	chunk;
	long		position;

	char *	Name = NULL;
 
    if(!(Name=ReadAndAllocString3ds(256))) return false;
   
	position = ftell(fp);

	while (position < Start+Size) 
	{
	 if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
	 if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;

	   switch (chunk.tag) 
	   {	
		case OBJ_TRIMESH:
		
		if(!ReadTriMesh( position, chunk.len)) return false;
		
		break;
		
		case OBJ_LIGHT:
		 newLight =new Light;
		 if (!ReadLight( position, chunk.len)) return false;			
		 m_lightlist.push_back(newLight);
		 break;	
		
		case OBJ_CAMERA:
		 newCamera=new Camera;
		 if(!ReadCamera(position, chunk.len)) return false;
		 m_cameralist.push_back(newCamera);
		 break;		
		
		default:
			fseek(fp, position + chunk.len, SEEK_SET);
		}
		position = ftell(fp);
	}
	return true;
}
bool C3DStudio::ReadTriMesh(long Start, long Size)
{
	chunk3ds	chunk;
	long		position = ftell(fp);

	while (position < Start + Size ) 
	{
	 if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
	 if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;

	 switch (chunk.tag) 
	 {	
	   case TRI_MAPPINGCOORS:
	   if (!ReadTexArray()) return false;
       break;
		
	   case TRI_VERTEX:
	   if(!ReadVertexArray()) return false;
	   break;
		
	   case TRI_FACE:
	   if (!ReadFaceArray()) return false;
	   break;

	   case TRI_MATERIAL :
	   if (!ReadMeshMaterial()) return false;
	   break;
	   
	   default:		
	   fseek(fp, position + chunk.len, SEEK_SET);
	  }	
	 position = ftell(fp);
	}
	return true;
	
}
bool C3DStudio::ReadVertexArray() 
{	
  unsigned short int vn;
  if(fread(&vn, sizeof(vn), 1, fp) != 1)return false;
 
  for (int i = 0; i <(int)vn; i++)  
  {
    Point3f point;
	if(fread(&point.X(), sizeof(float), 1, fp) != 1)return false;
	if(fread(&point.Y(), sizeof(float), 1, fp) != 1)return false;
	if(fread(&point.Z(), sizeof(float), 1, fp) != 1)return false;
  
	m_importer->AddVertex(point); //增加顶点
  }
  return true;
}
bool C3DStudio::ReadFaceArray() 
{
	unsigned short int flag;
	unsigned short int fn;
	if(fread(&fn, sizeof(fn), 1, fp) != 1)return false;
	
   for (int i = 0; i < (int)fn; i++) 
   {
	unsigned short v0,v1,v2;
	if(fread(&v0, sizeof(unsigned short), 1, fp) != 1)return false;
	if(fread(&v1, sizeof(unsigned short), 1, fp) != 1)return false;
	if(fread(&v2, sizeof(unsigned short), 1, fp) != 1)return false;
    if(fread(&flag, sizeof(unsigned short), 1, fp) != 1)return false;
	m_importer->AddFace((int)v0,(int)v1,(int)v2);//增加面
   }
	return true;
}
bool C3DStudio::ReadTexArray()
{
  unsigned short int tn;
  
  if(fread(&tn, sizeof(tn), 1, fp) != 1)return false;
     
  for (int i = 0; i < (int)tn; i++)
  {
    float u;
	float v;
    if(fread(&u, sizeof(u), 1, fp) != 1)return false;
	if(fread(&v, sizeof(v), 1, fp) != 1)return false;
  }
	
  return true;
}
bool C3DStudio::ReadMeshMaterial() 
{
    string name;

	char * Name;
	unsigned short facenum; 

    unsigned short faceindex;
	
	Name = ReadAndAllocString3ds(256);

	delete Name;
	if(fread(&facenum, sizeof(unsigned short), 1, fp) != 1)return false;

    for (int i = 0; i < facenum; i++) 
	{//读出面索引，无用信息
       if(fread(&faceindex, sizeof(unsigned short), 1, fp) != 1)return false;
    }

	std::list<Surface *>::iterator it=m_surfacelist.begin();
	for (;it!=m_surfacelist.end();++it) 
	{
		//if (name == (*it)->m_Name) newMesh->m_Surface=*it;
    }

	return true;
}
bool C3DStudio::ReadLight( long Start, long Size)
{
	chunk3ds	chunk;
	long		position;
	float tmp;
	if(fread(&newLight->m_Location.x, sizeof(float), 1, fp) != 1)return false;
	if(fread(&newLight->m_Location.y, sizeof(float), 1, fp) != 1)return false;
	if(fread(&newLight->m_Location.z, sizeof(float), 1, fp) != 1)return false;
	position = ftell(fp);
	
	while (position < Start+Size) 
	{
	   if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
	   if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;
		
	   switch (chunk.tag) 
	   {
		
		case DL_SPOTLIGHT:
			
		if (!ReadSpotLight(position, chunk.len)) return false;
		newLight->m_Type = 2;
		break;
		
		case COLOR_F:

		if(fread(&newLight->m_Color.x, sizeof(float), 1, fp) != 1)return false;
	    if(fread(&newLight->m_Color.y, sizeof(float), 1, fp) != 1)return false;
	    if(fread(&newLight->m_Color.z, sizeof(float), 1, fp) != 1)return false;
		
		break;

		case COLOR_24:
		if(fread(&tmp, sizeof(float), 1, fp) != 1)return false;	
		newLight->m_Color.x = (float) tmp / (float) 255.0;
		if(fread(&tmp, sizeof(float), 1, fp) != 1)return false;
		newLight->m_Color.y = (float) tmp / (float) 255.0;
	    if(fread(&tmp, sizeof(float), 1, fp) != 1)return false;
		newLight->m_Color.z = (float) tmp / (float) 255.0;	
	
		break;	
		
		case DL_OFF:
			newLight->m_Enable = false;
			break;
		
		case DL_ATTENUATE:
			newLight->m_AttnOn = true;
			break;	
			
	    case DL_INNER_RANGE:
			if(fread(&newLight->m_NearRange, sizeof(float), 1, fp) != 1)return false;
			break;
      
      	case DL_OUTER_RANGE:
			if(fread(&newLight->m_FarRange, sizeof(float), 1, fp) != 1)return false;
     		break;
     
		default:
			fseek(fp, position + chunk.len, SEEK_SET);
		}
		position = ftell(fp);
	}
	return true;
}


bool C3DStudio::ReadSpotLight(long Start, long Size)
{
	chunk3ds	chunk;
	long		position;
			
	if(fread(&newLight->m_Target.x, sizeof(float), 1, fp) != 1)return false;
    if(fread(&newLight->m_Target.y, sizeof(float), 1, fp) != 1)return false;
	if(fread(&newLight->m_Target.z, sizeof(float), 1, fp) != 1)return false;
   
	if(fread(&newLight->m_Hotspot, sizeof(float), 1, fp) != 1)return false;
	if(fread(&newLight->m_Falloff, sizeof(float), 1, fp) != 1)return false;

	position = ftell(fp);

	while (position < Start + Size) {
	if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
	if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;
		switch (chunk.tag) 
		{
		case 0:
			break;
		default:
			fseek(fp, position + chunk.len, SEEK_SET);
		}
		position = ftell(fp);
	}

	return true;
}
bool C3DStudio::ReadCamera( long Start, long Size)
{
	
	if(fread(&newCamera->m_Eye.x, sizeof(float), 1, fp) != 1)return false;
    if(fread(&newCamera->m_Eye.y, sizeof(float), 1, fp) != 1)return false;
	if(fread(&newCamera->m_Eye.z, sizeof(float), 1, fp) != 1)return false;

	if(fread(&newCamera->m_Target.x, sizeof(float), 1, fp) != 1)return false;
	if(fread(&newCamera->m_Target.y, sizeof(float), 1, fp) != 1)return false;
	if(fread(&newCamera->m_Target.z, sizeof(float), 1, fp) != 1)return false;
   
	if(fread(&newCamera->m_Roll, sizeof(float), 1, fp) != 1)return false;
	if(fread(&newCamera->m_Focal_length, sizeof(float), 1, fp) != 1)return false;

	newCamera->m_Fov = 2400.0f / newCamera->m_Focal_length;
  
  	long		position = ftell(fp);
	chunk3ds	chunk;
	
  	while (position < Start + Size) {
	if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
	if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;
		switch (chunk.tag) {
	  	case CAM_RANGES:
			if(fread(&newCamera->m_RangeNear, sizeof(float), 1, fp) != 1)return false;
			if(fread(&newCamera->m_RangeFar, sizeof(float), 1, fp) != 1)return false;
     		break; 		
  		default:
			fseek(fp, position + chunk.len, SEEK_SET);
		}
		position = ftell(fp);
	}
	
  	return true;
}


bool C3DStudio::ReadMaterial(long Start, long Size)
{
	long			position = ftell(fp);
	chunk3ds		chunk;
	char *	Name = NULL;

	while (position < Start + Size ) 
	{
	 if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
	 if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;
		switch (chunk.tag) 
		{
		 case MAT_NAME:
		 if (!(Name=ReadAndAllocString3ds(256))) return false;
		 newSurface->m_Name = Name;
		 delete Name;
		 break;
		
		case MAT_SHADING:	
			if(fread(&newSurface->m_ShadeModel, sizeof(short int), 1, fp) != 1)return false;
			break;

		case MAT_AMBIENT:
			if (!ReadColor(newSurface->m_Ambient)) return false;
			break;
		
		case MAT_DIFFUSE:
			if (!ReadColor(newSurface->m_Diffuse)) return false;
			newSurface->m_Reflected=newSurface->m_Diffuse/4;
			break;
		
		case MAT_SPECULAR:
			if (!ReadColor(newSurface->m_Specular)) return false;
			break;
		
		case MAT_SHININESS:
			if (!ReadPercentage(newSurface->m_Shininess)) return false;
			newSurface->m_Shininess;//*********************存储反射量
			newSurface->m_Shininess=newSurface->m_Shininess*300;
			break;

		case MAT_SHIN2PCT:
			if (!ReadPercentage(newSurface->m_Shin_strength)) return false;
			break;

		case MAT_TRANSPARENCY:
			float transparency;
			if (!ReadPercentage(transparency)) return false;
			newSurface->m_Transmitted=Point3f(transparency,transparency,transparency);//********************存储折射量
			break;
		case MAT_TEXMAP:
			
		ReadTextureMap(position, chunk.len,0);
			break;
		
		case MAT_BUMPMAP:
			
		ReadTextureMap(position, chunk.len,1);
			
			break;				
		
		case MAT_TWO_SIDE:
			newSurface->m_TwoSided = true;
			break;

		default:
			
			fseek(fp, position + chunk.len, SEEK_SET);
		}
		position = ftell(fp);
	}
	
	//surfacelist.push_back(newMat);
	return true;
}

bool C3DStudio::ReadTextureMap(long Start, long Size,int type)
{
	long			position = ftell(fp);
	chunk3ds		chunk;
	
	while (position < Start + Size) {
	 if(fread(&chunk.tag, sizeof(unsigned short), 1, fp) != 1)return false;
	 if(fread(&chunk.len, sizeof(unsigned long), 1, fp) != 1)return false;

		switch (chunk.tag) {
		
		case MAT_MAPNAME:
			switch(type)
			{
			case 0:newSurface->m_TextureName=ReadAndAllocString3ds(256);
                   break;
			case 1:newSurface->m_BumpName=ReadAndAllocString3ds(256);
			}
			break;

		default:
			fseek(fp, position + chunk.len, SEEK_SET);
		}
		position = ftell(fp);
	}
}



bool C3DStudio::ReadColor(Point3f &color)
{
	chunk3ds		chunk;
	long			position = ftell(fp);
	unsigned char	tmp;

	if(fread(&chunk.tag, sizeof(chunk.tag), 1, fp) != 1)return false;
	if(fread(&chunk.len, sizeof(chunk.len), 1, fp) != 1)return false;

	switch (chunk.tag) {
	case COLOR_F:
		if(fread(&color.x, sizeof(color.x), 1, fp) != 1)return false;
		if(fread(&color.y, sizeof(color.y), 1, fp) != 1)return false;
		if(fread(&color.z, sizeof(color.z), 1, fp) != 1)return false;		
		break;
	case COLOR_24:
	 if(fread(&tmp, sizeof(tmp), 1, fp) != 1)return false;		
		color.x = (float) tmp / (float) 255.0;
	if(fread(&tmp, sizeof(tmp), 1, fp) != 1)return false;		
		color.y = (float) tmp / (float) 255.0;
	if(fread(&tmp, sizeof(tmp), 1, fp) != 1)return false;		
		color.z = (float) tmp / (float) 255.0;
		break;
	default:
		fseek(fp, position + chunk.len, SEEK_SET);
		return false;
	}

	return true;
}


bool C3DStudio::ReadPercentage(float &value)
{
  chunk3ds	chunk;
	long	position = ftell(fp);

	if(fread(&chunk.tag, sizeof(chunk.tag), 1, fp) != 1)return false;
	if(fread(&chunk.len, sizeof(chunk.len), 1, fp) != 1)return false;
	
	if (chunk.tag == INT_PERCENTAGE) {
		short	svalue;
		 if(fread(&svalue, sizeof(svalue), 1, fp) != 1)return false;
		value = (float) svalue/ (float) 100.0;
		return true;
	} 
	else if (chunk.tag == FLOAT_PERCENTAGE) {
	 if(fread(&value, sizeof(value), 1, fp) != 1)return false;
		return true;
	} 
	fseek(fp, position + chunk.len, SEEK_SET);
	return false;	
}


char * C3DStudio::ReadAndAllocString3ds(unsigned int len)
{   
   char *str;
   char *temp = NULL;
   temp = new char[len+1];
   if(temp == NULL) return false;
   int c;
   int i;
   for (i=0;(c = fgetc(fp)) != EOF && c != '\0'; i++) 
   {
	 if (i < (int)len+1) {temp[i] = c;}
   }
	
   if (i < (int)len+1) {temp[i] = '\0';} 
   else {temp[len] = '\0';}

	str = new char[strlen(temp)+1];
	if (str == NULL) return false;

	strcpy(str, temp);
   	delete [] temp;
   	return str;	
}	


















