#ifndef SUPERQUADRIC_H
#define SUPERQUADRIC_H

#ifndef PI
#define PI	(float)3.141592648
#endif

struct SuperQuadric {
  float a1, a2, a3;     /* Scaling factors for x, y, and z */
  float alpha;          /* For generating toroids. This is the inner radius */
  float n, e;           /* North-South/East-West Roundness/Squareness Factors */
  float u1, u2;         /* Initial and Final U values */
  float v1, v2;         /* Initial and Final V values */
  int u_segs;           /* Number of segments for U */
  int v_segs;           /* Number of segments for V */
  float s1, t1;         /* Initial s and t texture coordinates */
  float s2, t2;         /* Final S and T texture coordinates */
  int texture_flag;     /* Flag determining texture coordinate specification */
  int gl_list_id;       /* OpenGL Display List ID */
};

float sgnf ( float x );
float absf ( float x );
float sqC ( float v, float n );
float sqCT ( float v, float n, float alpha );
float sqS ( float v, float n );
void sqEllipsoid ( float a1, float a2, float a3, float u, float v, float n, float e, 
                   float *x, float  *y, float *z, float *nx, float *ny, float *nz );
void sqToroid ( float a1, float a2, float a3, float u, float v, float n, float e, float alpha,
                float *x, float *y, float *z, float *nx, float *ny, float *nz );
void sqSolidEllipsoid ( SuperQuadric *sq, int make_display_list, int gen_texture_coordinates );
void sqSolidToroid ( SuperQuadric *sq, int make_display_list, int gen_texture_coordinates );
float sqEllipsoidInsideOut ( SuperQuadric *sq , float x, float y, float z );
float sqToroidInsideOut ( SuperQuadric *sq , float x, float y, float z );

void sqSolidSphere ( float radius, int slices, int segments );
void sqSolidCylinder ( float radius, int slices, int segments );
void sqSolidStar ( float radius, int slices, int segments );
void sqSolidDoublePyramid ( float radius, int slices, int segments );
void sqSolidTorus ( float radius1, float radius2, int slices, int segments );
void sqSolidPineappleSlice ( float radius1, float radius2, int slices, int segments );
void sqSolidPillow ( float radius, int slices, int segments );
void sqSolidSquareTorus ( float radius1, float radius2, int slices, int segments );
void sqSolidPinchedTorus ( float radius1, float radius2, int slices, int segments );
void sqSolidRoundCube ( float radius, int slices, int segments );

#endif