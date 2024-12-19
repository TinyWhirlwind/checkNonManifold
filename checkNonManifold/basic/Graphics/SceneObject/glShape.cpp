#include "glShape.h"

#include <Windows.h>
#include <QtCore/qDebug.h>

#include "gl/gl.h"
#include "gl/glu.h"
#include "gl/glut.h"
#include "Common\quaternion.h"
#include "AlgebraCompute.h"

using namespace sn3DCore;
int	glShape::m_straightArrowList = -1; // 显示列表
int	glShape::m_rotateArrowList = -1;
float glShape::m_cylinderRadius = 0.5;

void glShape::DrawSolidSphere(Point3f center, float radius, sn3DCore::Color4f color)
{
	int slices = 20;
	int stacks = 20;
	glPushAttrib(GL_TRANSFORM_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glColor4f(color[0], color[1], color[2], color[3]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslated(center.X(),center.Y(), center.Z());	//将Solid变换到实际位置
	glutSolidSphere(radius, slices, stacks);	//默认绘制的Solid中心为原点
	glPopMatrix();
	glPopAttrib();
}

void glShape::DrawTransparentPlane(Point3f center, Point3f normal,
	Point3f frontDir, float size, sn3DCore::Color4f color)
{
	glPushAttrib(GL_TRANSFORM_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_LIGHTING_BIT | GL_BLEND);	
	glEnable(GL_BLEND);
	glEnable(GL_COLOR_MATERIAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);
	float mat[16];
	float gl_mat[16];
	GetRotationMatrix(Point3f(0.0, 0.0, 1.0), Point3f(0.0, 1.0, 0.0),  normal, frontDir, mat);

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			gl_mat[i * 4 + j] = mat[j * 4 + i];
		}

	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glScalef(size, size, size);
	glMultMatrixf(gl_mat);
	glColor4f(color[0], color[1], color[2], color[3]);
	glBegin(GL_QUADS);
	glVertex3f(-1, -1, 0);
	glVertex3f(1, -1, 0);
	glVertex3f(1, 1, 0);
	glVertex3f(-1, 1, 0);
	glEnd();
	glPopMatrix();
	glDepthMask(GL_TRUE);
	glPopAttrib();
}

void glShape::DrawPlaneCircle(Point3f center, Point3f normal, float radius)
{
	Point3f curDir = Point3f(1, 0, 0); //控件原方向
	Point3f axis = curDir ^ normal;
	axis.Normalize();
	float co = curDir * normal;
	if (co >= 1) co = 1.0;
	else if (co <= -1) co = -1.0;
	float angle = 180 * acosf(co) / 3.1415927;
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glRotatef(angle, axis.X(), axis.Y(), axis.Z());
	glScalef(radius, radius, radius);
	int nside = 99;
	const double pi2 = 3.14159265 * 2.0;
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_LIGHTING_BIT | GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);
	glColor4f(1.0, 0.0, 0.0, 0.15);
	for (double i = 0; i < nside; i++)
	{
		glBegin(GL_TRIANGLES);
		glVertex3f(0, 0.0, 0.0);
		glVertex3f(0, cos(i * pi2 / nside)*1.5, sin(i * pi2 / nside)*1.5);
		glVertex3f(0, cos((i + 1)* pi2 / nside)*1.5, sin((i + 1) * pi2 / nside)*1.5);
		glEnd();
	}
	glDepthMask(GL_TRUE);
	glPopAttrib();
	glPopMatrix();
}

void glShape::DrawLineCircle(Point3f center, Point3f normal, float radius, float lineWidth)
{
	glPushAttrib(GL_TRANSFORM_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(lineWidth);
	glEnable(GL_BLEND);
	glEnable(GL_COLOR_MATERIAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthMask(GL_FALSE);
	Point3f curDir = Point3f(1, 0, 0); //控件原方向
	Point3f axis = curDir ^ normal;
	axis.Normalize();
	float co = curDir * normal;
	if (co >= 1)		co = 1.0;
	else if (co <= -1)	co = -1.0;

	// axis:  normal到curDir的旋转
	// angle: normal到curDir的旋转角度
	float angle = 180.f * acosf(co) / M_PI;

	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glRotatef(angle, axis.X(), axis.Y(), axis.Z());
	
	glScalef(radius, radius, radius);

	int nside = 99;
	const double pi2 = M_PI * 2.0;
	glBegin(GL_LINE_LOOP);
	for (double i = 0; i < nside; i++)
	{
		glNormal3d(0.0, cos(i * pi2 / nside), sin(i * pi2 / nside));
		glVertex3d(0.0, cos(i * pi2 / nside), sin(i * pi2 / nside));
	}
	glEnd();
	glPopMatrix();
	glPopAttrib();
}

void  glShape::DrawCylinderArrow(Point3f pos, Point3f dir, float len,float radius,float circleRatio, float cylinderlenRatio,  
	                                          int body_slice, int head_slice)
{
	//radius 圆柱半径
	glPushAttrib(GL_TRANSFORM_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_LINE_BIT | GL_LIGHTING_BIT | GL_BLEND);
	glShadeModel(GL_SMOOTH);
	Point3f curDir = Point3f(1, 0, 0); //控件原方向
	Point3f axis = curDir ^ dir;
	axis.Normalize(); 
	float co = curDir * dir;
	if (co >= 1) co = 1.0;
	else if (co <= -1) co = -1.0;
	float angle = 180.f * acosf(co) / M_PI;
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]); // 底面中心点
	glRotatef(angle, axis.X(), axis.Y(), axis.Z());
	glScalef(len, radius, radius); // 长度 & 半径

	if (glIsList(m_straightArrowList))
		glCallList(m_straightArrowList);
	else
	{
		m_straightArrowList = glGenLists(1);
		glNewList(m_straightArrowList, GL_COMPILE_AND_EXECUTE);

		Point3f p0;
		Point3f p1;
		Point3f P[2];
		Point3f N[2];

		// 绘制圆柱侧面
		int segment = 2;
		float cirAngle = 0;
		float step = 6.28f / (float)body_slice;
	
		float radius = m_cylinderRadius;
		float cylinderLen = 1.0*cylinderlenRatio;
		float curLen = 0.0;
		float nextLen = 0.0;
		for (int a = 0; a < segment; ++a)
		{
			cirAngle = 0;
			curLen = cylinderLen*((float)a / (float)segment);
			nextLen = cylinderLen*((float)(a+1) / (float)segment);
			glBegin(GL_TRIANGLE_STRIP);
			for (int b = 0; b <= body_slice; ++b)
			{
				if (b == body_slice) cirAngle = 0;

				p0 = Point3f(curLen, sin(cirAngle), cos(cirAngle));
				p1 = p0; 
				p1[0] = nextLen;
				glNormal3f(0, p0[1], p0[2]); //旋转后的法向量
				glVertex3d(p0[0], p0[1], p0[2]);
				glVertex3d(p1[0], p1[1], p1[2]);
				cirAngle += step;
			}
			glEnd();
		}

		//  绘制圆柱底面
		cirAngle = 0;
		P[0] = Point3f(0, sinf(0), cosf(0));
		glNormal3f(-1, 0, 0);

		for (int b = 1; b <= head_slice; ++b)
		{
			float cirAngle = 6.28f*(float)b / (float)head_slice; // 顺时针角度，法向朝-z
			if (b == head_slice) cirAngle = 0;
			P[1] = Point3f(0, sinf(cirAngle), cosf(cirAngle));
			glBegin(GL_TRIANGLES);
			glVertex3f(0, 0, 0);
			glVertex3f(P[0].X(), P[0].Y(), P[0].Z());
			glVertex3f(P[1].X(), P[1].Y(), P[1].Z());
			glEnd();
			P[0] = P[1];
		}

		// 绘制圆锥底面
		P[0] = Point3f(cylinderLen, circleRatio*sinf(0), circleRatio*cosf(0));
		glNormal3f(-1, 0, 0);
		for (int b = 1; b <= head_slice + 1; ++b)
		{
			float cirAngle = -6.28f*(float)b / (float)head_slice;
			if (b == head_slice) cirAngle = 0;
			P[1] = Point3f(cylinderLen, circleRatio*sinf(cirAngle), circleRatio*cosf(cirAngle));
			glBegin(GL_TRIANGLES);
			glVertex3f(P[0].X(), P[0].Y(), P[0].Z());
			glVertex3f(cylinderLen, 0, 0);
			glVertex3f(P[1].X(), P[1].Y(), P[1].Z());
			glEnd();
			P[0] = P[1];
		}

		// 绘制圆锥侧面
		p0 = Point3f(1.0, 0, 0); //圆锥原顶点

		N[0] = Point3f(1, sinf(0), cosf(0)); //没有归一化
		P[0] = Point3f(cylinderLen, circleRatio*sinf(0), circleRatio*cosf(0));

		for (int b = 1; b <= head_slice; ++b)
		{
			float cirAngle = -6.28f*(float)b / (float)head_slice;//逆时针
			if (b == head_slice) cirAngle = 0;

			N[1] = Point3f(1.f, sinf(cirAngle), cosf(cirAngle));
			P[1] = Point3f(cylinderLen, circleRatio*sinf(cirAngle), circleRatio*cosf(cirAngle));

			glBegin(GL_TRIANGLES);
			Point3f n = ((P[0] - p0) ^ (P[1] - p0)).Normalize();

			// 中心点
			glNormal3f(n.X(), n.Y(), n.Z());   ///////// 法线
			glVertex3f(p0[0], p0[1], p0[2]);  ////////顶点
			// 第一个点
			glNormal3f(N[0].X(), N[0].Y(), N[0].Z()); ///////// 法线
			glVertex3f(P[0].X(), P[0].Y(), P[0].Z()); /////////// 顶点
			// 第二个点
			glNormal3f(N[1].X(), N[1].Y(), N[1].Z()); ///////法线
			glVertex3f(P[1].X(), P[1].Y(), P[1].Z()); /////////顶点
			glEnd();

			N[0] = N[1];
			P[0] = P[1];
		}

		glEndList();
	}
	glPopMatrix();
	glPopAttrib();
}

void glShape::DrawRotateArrow(Point3f center, Point3f upDir, Point3f frontDir, 
	                           float r, float halfAngle,int body_slice, int head_slice)
{
	float angle;// 弯曲角度,弯曲平面是xz平面,轴线是x轴

	float rotate0[3][3]; //法线旋转矩阵
	float rotate1[3][3];

	Point3f axisPoint0(0, 0, 0);//轴上顶点坐标
	Point3f axisPoint1(0, 0, 0);

	Point3f point; //待计算点
	Point3f p0;
	Point3f p1;
	Point3f P[2];
	Point3f N[2];

	// 绘制圆柱侧面
	float cylinderRadius = m_cylinderRadius;
	int segment = 6;
	float cirAngle = 0;
	float step = 6.28f / (float)body_slice;
	float mat[16];
	float gl_mat[16];
	GetRotationMatrix(Point3f(0.0, 1.0, 0.0), Point3f(0.0, 0.0, 1.0), upDir, frontDir, mat);

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			gl_mat[i * 4 + j] = mat[j * 4 + i];
		}
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glMultMatrixf(gl_mat);

	for (int a = -segment; a < segment; ++a)
	{
		angle = ((float)a / (float)segment) * 3.14 * halfAngle / 180.0; //当前轴线弯曲角度

		axisPoint0.X() = r * sinf(angle);
		axisPoint0.Z() = r * cosf(angle);

		rotate0[0][0] = cosf(angle);
		rotate0[0][1] = 0;
		rotate0[0][2] = sinf(angle);
		rotate0[1][0] = 0;
		rotate0[1][1] = 1;
		rotate0[1][2] = 0;
		rotate0[2][0] = -sinf(angle);
		rotate0[2][1] = 0;
		rotate0[2][2] = cosf(angle);

		angle = ((float)(a + 1) / (float)segment) * 3.14 * halfAngle / 180.0; //当前轴线弯曲角度

		axisPoint1.X() = r * sinf(angle);
		axisPoint1.Z() = r * cosf(angle);

		rotate1[0][0] = cosf(angle);
		rotate1[0][1] = 0;
		rotate1[0][2] = sinf(angle);
		rotate1[1][0] = 0;
		rotate1[1][1] = 1;
		rotate1[1][2] = 0;
		rotate1[2][0] = -sinf(angle);
		rotate1[2][1] = 0;
		rotate1[2][2] = cosf(angle);

		cirAngle = 0; // 旋绕一圈

		glBegin(GL_TRIANGLE_STRIP); // 每一段
		for (int b = 0; b <= body_slice; ++b)
		{
			if (b == body_slice) cirAngle = 0; // 先局部旋转，再平移

			float y = cylinderRadius*sin(cirAngle);
			float z = cylinderRadius*cos(cirAngle);

			p0.X() = z*rotate0[0][2];  //旋转
			p0.Y() = y;
			p0.Z() = z*rotate0[2][2];

			glNormal3f(p0[0], p0[1], p0[2]); //旋转后的法向量

			p0.X() += axisPoint0.X(); //移动到当前中心点
			p0.Z() += axisPoint0.Z();

			glVertex3d(p0[0], p0[1], p0[2]);

			p1.X() = z*rotate1[0][2];  //旋转
			p1.Y() = y;
			p1.Z() = z*rotate1[2][2];

			glNormal3f(p1[0], p1[1], p1[2]);//旋转后的法向量

			p1.X() += axisPoint1.X();
			p1.Z() += axisPoint1.Z();

			glVertex3d(p1[0], p1[1], p1[2]);

			cirAngle += step;
		}
		glEnd();
	}

	//////////////////////    绘制相邻圆锥底面
	cirAngle = 0;

	angle = (float)-1 * 3.14 * halfAngle / 180.0;//当前弯曲角度

	axisPoint0.X() = r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint0.Z() = r * cosf(angle);//向 -z方向弯曲

	rotate0[0][0] = cosf(angle);
	rotate0[0][1] = 0;
	rotate0[0][2] = sinf(angle);
	rotate0[1][0] = 0;
	rotate0[1][1] = 1;
	rotate0[1][2] = 0;
	rotate0[2][0] = -sinf(angle);
	rotate0[2][1] = 0;
	rotate0[2][2] = cosf(angle);

	P[0] = Point3f(-1, 2.4*cylinderRadius*sin(0), 2.4*cylinderRadius*cos(0));

	//== 指定法向
	point.X() = 1;  //相对于原中心点,即中心移动到原点
	point.Y() = 0;
	point.Z() = 0;

	p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2]; //旋转
	p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
	p1.Y() = 0;

	glNormal3f(p1.X(), p1.Y(), p1.Z());

	for (int b = 1; b <= head_slice; ++b)
	{
		float cirAngle = -6.28f*(float)b / (float)head_slice;
		if (b == head_slice) cirAngle = 0;

		P[1] = Point3f(-1, 2.4*cylinderRadius*sin(cirAngle), 2.4*cylinderRadius*cos(cirAngle));

		glBegin(GL_TRIANGLES);

		point.X() = 0;  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = P[0].Z();

		p0.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p0.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];

		p0.X() += axisPoint0.X(); //移动到当前中心点
		p0.Z() += axisPoint0.Z();
		p0.Y() = P[0][1];

		point.X() = 0;  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = P[1].Z();

		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];

		p1.X() += axisPoint0.X(); //移动到当前中心点
		p1.Z() += axisPoint0.Z();
		p1.Y() = P[1][1];

		glVertex3f(axisPoint0.X(), axisPoint0.Y(), axisPoint0.Z());
		glVertex3f(p0.X(), p0.Y(), p0.Z());
		glVertex3f(p1.X(), p1.Y(), p1.Z());

		glEnd();

		P[0] = P[1];
	}

	angle = (float)1 * 3.14 * halfAngle / 180.0;//当前弯曲角度
	axisPoint1.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint1.Z() = (float)r * cosf(angle);//向 -z方向弯曲

	rotate1[0][0] = cosf(angle);
	rotate1[0][1] = 0;
	rotate1[0][2] = sinf(angle);
	rotate1[1][0] = 0;
	rotate1[1][1] = 1;
	rotate1[1][2] = 0;
	rotate1[2][0] = -sinf(angle);
	rotate1[2][1] = 0;
	rotate1[2][2] = cosf(angle);

	P[0] = Point3f(1, 2.4*cylinderRadius*sin(0), 2.4*cylinderRadius*cos(0));
	point.X() = -1;  //相对于原中心点,即中心移动到原点
	point.Y() = 0;
	point.Z() = 0;
	p1.X() = point.X()*rotate1[0][0] + point.Y()*rotate1[0][1] + point.Z()*rotate1[0][2]; //旋转法线
	p1.Z() = point.X()*rotate1[2][0] + point.Y()*rotate1[2][1] + point.Z()*rotate1[2][2];
	p1.Y() = 0;

	glNormal3f(p1.X(), p1.Y(), p1.Z());

	for (int b = 1; b <= head_slice; ++b)
	{
		float cirAngle = -6.28f*(float)b / (float)head_slice;
		if (b == head_slice) cirAngle = 0;

		P[1] = Point3f(1, 2.4*cylinderRadius*sin(cirAngle), 2.4*cylinderRadius*cos(cirAngle));

		glBegin(GL_TRIANGLES);

		point.X() = 0;  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = P[0].Z();

		p0.X() = point.X()*rotate1[0][0] + point.Y()*rotate1[0][1] + point.Z()*rotate1[0][2];  //旋转
		p0.Z() = point.X()*rotate1[2][0] + point.Y()*rotate1[2][1] + point.Z()*rotate1[2][2];

		p0.X() += axisPoint1.X(); //移动到当前中心点
		p0.Z() += axisPoint1.Z();
		p0.Y() = P[0][1];

		point.X() = 0;  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = P[1].Z();

		p1.X() = point.X()*rotate1[0][0] + point.Y()*rotate1[0][1] + point.Z()*rotate1[0][2];  //旋转
		p1.Z() = point.X()*rotate1[2][0] + point.Y()*rotate1[2][1] + point.Z()*rotate1[2][2];

		p1.X() += axisPoint1.X(); //移动到当前中心点
		p1.Z() += axisPoint1.Z();
		p1.Y() = P[1][1];

		glVertex3f(p0.X(), p0.Y(), p0.Z());
		glVertex3f(axisPoint1.X(), axisPoint1.Y(), axisPoint1.Z());

		glVertex3f(p1.X(), p1.Y(), p1.Z());

		glEnd();

		P[0] = P[1];
	}

	/////////////////////////////////               绘制圆锥侧面
	float arrowAngle = 115.0 / r;
	angle = (float)-1 * 3.14 * halfAngle / 180.0 - 3.14 * arrowAngle / 180.0;//当前弯曲角度

	axisPoint1.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint1.Z() = (float)r * cosf(angle);//向 -z方向弯曲

	angle = (float)-1 * 3.14 * halfAngle / 180.0;//当前弯曲角度

	axisPoint0.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint0.Z() = (float)r * cosf(angle);//向 -z方向弯曲

	rotate0[0][0] = cosf(angle);
	rotate0[0][1] = 0;
	rotate0[0][2] = sinf(angle);
	rotate0[1][0] = 0;
	rotate0[1][1] = 1;
	rotate0[1][2] = 0;
	rotate0[2][0] = -sinf(angle);
	rotate0[2][1] = 0;
	rotate0[2][2] = cosf(angle);

	float coneLen = 0.6f;
	p0 = Point3f(-coneLen, 0, 0); //圆锥原顶点

	N[0] = Point3f(-1, sinf(0), cosf(0)); //没有归一化
	P[0] = Point3f(-1., 2.4*cylinderRadius*sin(0), 2.4*cylinderRadius*cos(0));

	for (int b = 1; b <= head_slice; ++b)
	{
		float cirAngle = -6.28f*(float)b / (float)head_slice;//逆时针
		if (b == head_slice) cirAngle = 0;

		N[1] = Point3f(-1.f, sinf(cirAngle), cosf(cirAngle));
		P[1] = Point3f(-1, 2.4*cylinderRadius*sin(cirAngle), 2.4*cylinderRadius*cos(cirAngle));

		glBegin(GL_TRIANGLES);

		Point3f n = ((P[1] - p0) ^ (P[0] - p0)).Normalize();

		// 第一个点
		point.X() = N[0].X();  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = N[0].Z();
		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
		p1.Y() = N[0].Y();
		glNormal3f(p1.X(), p1.Y(), p1.Z()); ///////// 法线
		point.X() = 0;  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = P[0].Z();
		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
		p1.X() += axisPoint0.X(); //移动到当前中心点
		p1.Z() += axisPoint0.Z();
		p1.Y() = P[0].Y();
		glVertex3f(p1.X(), p1.Y(), p1.Z()); /////////// 顶点


		// 中心点
		point.X() = n.X();  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = n.Z();
		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
		p1.Y() = n.Y();
		glNormal3f(p1.X(), p1.Y(), p1.Z());   ///////// 法线
		p1.X() = axisPoint1.X(); //移动到当前中心点
		p1.Z() = axisPoint1.Z();
		p1.Y() = p0.Y();
		glVertex3f(p1[0], p1[1], p1[2]);  ////////顶点


		// 第三个点
		point.X() = N[1].X();  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = N[1].Z();
		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
		p1.Y() = N[1].Y();
		glNormal3f(p1.X(), p1.Y(), p1.Z()); ///////法线
		point.X() = 0;  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = P[1].Z();
		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
		p1.X() += axisPoint0.X(); //移动到当前中心点
		p1.Z() += axisPoint0.Z();
		p1.Y() = P[1].Y();
		glVertex3f(p1.X(), p1.Y(), p1.Z()); /////////顶点

		glEnd();

		N[0] = N[1];
		P[0] = P[1];
	}

	angle = (float)1 * 3.14 * halfAngle / 180.0 + 3.14 * arrowAngle / 180.0;//当前弯曲角度
	axisPoint1.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint1.Z() = (float)r * cosf(angle);//向 -z方向弯曲

	angle = (float)1 * 3.14 * halfAngle / 180.0;//当前弯曲角度
	axisPoint0.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint0.Z() = (float)r * cosf(angle);//向 -z方向弯曲

	rotate0[0][0] = cosf(angle);
	rotate0[0][1] = 0;
	rotate0[0][2] = sinf(angle);
	rotate0[1][0] = 0;
	rotate0[1][1] = 1;
	rotate0[1][2] = 0;
	rotate0[2][0] = -sinf(angle);
	rotate0[2][1] = 0;
	rotate0[2][2] = cosf(angle);

	p0 = Point3f(coneLen, 0, 0); //圆锥原顶点

	N[0] = Point3f(1, sinf(0), cosf(0)); //没有归一化
	P[0] = Point3f(1., 2.4*cylinderRadius*sin(0), 2.4*cylinderRadius*cos(0));

	for (int b = 1; b <= head_slice; ++b)
	{
		float cirAngle = -6.28f*(float)b / (float)head_slice;//逆时针
		if (b == head_slice) cirAngle = 0;

		N[1] = Point3f(1.f, sinf(cirAngle), cosf(cirAngle));
		P[1] = Point3f(1, 2.4*cylinderRadius*sin(cirAngle), 2.4*cylinderRadius*cos(cirAngle));

		glBegin(GL_TRIANGLES);

		Point3f n = ((P[0] - p0) ^ (P[1] - p0)).Normalize();

		// 中心点
		point.X() = n.X();  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = n.Z();
		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
		p1.Y() = n.Y();
		glNormal3f(p1.X(), p1.Y(), p1.Z());   ///////// 法线
		p1.X() = axisPoint1.X(); //移动到当前中心点
		p1.Z() = axisPoint1.Z();
		p1.Y() = p0.Y();
		glVertex3f(p1[0], p1[1], p1[2]);  ////////顶点

		// 第一个点
		point.X() = N[0].X();  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = N[0].Z();
		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
		p1.Y() = N[0].Y();
		glNormal3f(p1.X(), p1.Y(), p1.Z()); ///////// 法线
		point.X() = 0;  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = P[0].Z();
		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
		p1.X() += axisPoint0.X(); //移动到当前中心点
		p1.Z() += axisPoint0.Z();
		p1.Y() = P[0].Y();
		glVertex3f(p1.X(), p1.Y(), p1.Z()); /////////// 顶点


		// 第二个点
		point.X() = N[1].X();  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = N[1].Z();
		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
		p1.Y() = N[1].Y();
		glNormal3f(p1.X(), p1.Y(), p1.Z()); ///////法线
		point.X() = 0;  //相对于原中心点,即中心移动到原点
		point.Y() = 0;
		point.Z() = P[1].Z();
		p1.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
		p1.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];
		p1.X() += axisPoint0.X(); //移动到当前中心点
		p1.Z() += axisPoint0.Z();
		p1.Y() = P[1].Y();
		glVertex3f(p1.X(), p1.Y(), p1.Z()); /////////顶点

		glEnd();

		N[0] = N[1];
		P[0] = P[1];
	}

	glPopMatrix();
}
void  glShape::DrawCylinderArrowWithFixedSize(Point3f pos, Point3f dir, float cylinderLen, float cylinderRadius, 
	                             float coneLen, float coneRadius,
	                             int body_slice, int head_slice)
{
	//radius 圆柱半径
	glPushAttrib(GL_TRANSFORM_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_LINE_BIT | GL_LIGHTING_BIT | GL_BLEND);
	glShadeModel(GL_SMOOTH);
	Point3f curDir = Point3f(1, 0, 0); //控件原方向
	Point3f axis = curDir ^ dir;
	axis.Normalize();
	float co = curDir * dir;
	if (co >= 1) co = 1.0;
	else if (co <= -1) co = -1.0;

	float angle = 180 * acosf(co) / M_PI;

	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]); // 底面中心点
	glRotatef(angle, axis.X(), axis.Y(), axis.Z());
	
	{
		Point3f p0;
		Point3f p1;
		Point3f P[2];
		Point3f N[2];

		// 绘制圆柱侧面
		int segment = 6;
		float cirAngle = 0;
		float step = 6.28f / (float)body_slice;

		float curLen = 0.0;
		float nextLen = 0.0;
		for (int a = 0; a < segment; ++a)
		{
			cirAngle = 0;
			curLen = cylinderLen*((float)a / (float)segment);
			nextLen = cylinderLen*((float)(a + 1) / (float)segment);
			glBegin(GL_TRIANGLE_STRIP);
			for (int b = 0; b <= body_slice; ++b)
			{
				if (b == body_slice) cirAngle = 0;

				p0 = Point3f(curLen, cylinderRadius*sin(cirAngle), cylinderRadius*cos(cirAngle));
				p1 = p0;
				p1[0] = nextLen;
				glNormal3f(0, p0[1], p0[2]); //旋转后的法向量
				glVertex3d(p0[0], p0[1], p0[2]);
				glVertex3d(p1[0], p1[1], p1[2]);
				cirAngle += step;
			}
			glEnd();
		}

		//  绘制圆柱底面
		cirAngle = 0;
		P[0] = Point3f(0, cylinderRadius*sinf(0), cylinderRadius*cosf(0));
		glNormal3f(-1, 0, 0);

		for (int b = 1; b <= head_slice; ++b)
		{
			float cirAngle = 6.28f*(float)b / (float)head_slice; // 顺时针角度，法向朝-z
			if (b == head_slice) cirAngle = 0;
			P[1] = Point3f(0, cylinderRadius*sinf(cirAngle), cylinderRadius*cosf(cirAngle));
			glBegin(GL_TRIANGLES);
			glVertex3f(0, 0, 0);
			glVertex3f(P[0].X(), P[0].Y(), P[0].Z());
			glVertex3f(P[1].X(), P[1].Y(), P[1].Z());
			glEnd();
			P[0] = P[1];
		}

		// 绘制圆锥底面
		P[0] = Point3f(cylinderLen, coneRadius*sinf(0), coneRadius*cosf(0));
		glNormal3f(-1, 0, 0);
		for (int b = 1; b <= head_slice + 1; ++b)
		{
			float cirAngle = -6.28f*(float)b / (float)head_slice;
			if (b == head_slice) cirAngle = 0;
			P[1] = Point3f(cylinderLen, coneRadius*sinf(cirAngle), coneRadius*cosf(cirAngle));
			glBegin(GL_TRIANGLES);
			glVertex3f(P[0].X(), P[0].Y(), P[0].Z());
			glVertex3f(cylinderLen, 0, 0);
			glVertex3f(P[1].X(), P[1].Y(), P[1].Z());
			glEnd();
			P[0] = P[1];
		}

		// 绘制圆锥侧面
		p0 = Point3f(cylinderLen + coneLen, 0, 0); //圆锥原顶点
		N[0] = Point3f(1, sinf(0), cosf(0)); //没有归一化
		P[0] = Point3f(cylinderLen, coneRadius*sinf(0), coneRadius*cosf(0));

		for (int b = 1; b <= head_slice; ++b)
		{
			float cirAngle = -6.28f*(float)b / (float)head_slice;//逆时针
			if (b == head_slice) cirAngle = 0;

			N[1] = Point3f(1.f, sinf(cirAngle), cosf(cirAngle));
			P[1] = Point3f(cylinderLen, coneRadius*sinf(cirAngle), coneRadius*cosf(cirAngle));

			glBegin(GL_TRIANGLES);
			Point3f n = ((P[0] - p0) ^ (P[1] - p0)).Normalize();

			// 中心点
			glNormal3f(n.X(), n.Y(), n.Z());   ///////// 法线
			glVertex3f(p0[0], p0[1], p0[2]);  ////////顶点
											  // 第一个点
			glNormal3f(N[0].X(), N[0].Y(), N[0].Z()); ///////// 法线
			glVertex3f(P[0].X(), P[0].Y(), P[0].Z()); /////////// 顶点
													  // 第二个点
			glNormal3f(N[1].X(), N[1].Y(), N[1].Z()); ///////法线
			glVertex3f(P[1].X(), P[1].Y(), P[1].Z()); /////////顶点
			glEnd();

			N[0] = N[1];
			P[0] = P[1];
		}
	}
	glPopMatrix();
	glPopAttrib();
}

void glShape::DrawStraightArrow(Point3f pos, Point3f dir, float scale, int body_slice, int head_slice)
{
	Point3f curDir = Point3f(1, 0, 0);
	Point3f axis = curDir ^ dir;
	axis.Normalize();
	float co = curDir * dir;
	if (co >= 1) co = 1.0;
	else if (co <= -1) co = -1.0;

	float angle = 180 * acosf(co) / M_PI;

	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	glRotatef(angle, axis.X(), axis.Y(), axis.Z());
	glScalef(scale, scale, scale);

	if (glIsList(m_straightArrowList))
		glCallList(m_straightArrowList);
	else
	{
		m_straightArrowList = glGenLists(1);
		glNewList(m_straightArrowList, GL_COMPILE_AND_EXECUTE);

		Point3f p0;
		Point3f p1;
		Point3f P[2];
		Point3f N[2];

		int segment = 6;
		float cirAngle = 0;
		float step = 6.28f / (float)body_slice;

		float ratio = 0.03;

		for (int a = 0; a < segment; ++a)
		{
			cirAngle = 0;

			glBegin(GL_TRIANGLE_STRIP);
			for (int b = 0; b <= body_slice; ++b)
			{
				if (b == body_slice) cirAngle = 0;

				p0 = Point3f((float)a / (float)segment, ratio*sin(cirAngle), ratio*cos(cirAngle));
				p1 = p0; p1[0] = (float)(a + 1) / (float)segment;

				glNormal3f(0, p0[1], p0[2]);
				glVertex3d(p0[0], p0[1], p0[2]);
				glVertex3d(p1[0], p1[1], p1[2]);

				cirAngle += step;
			}
			glEnd();
		}

		cirAngle = 0;

		P[0] = Point3f(1, ratio*0.24*sinf(0), ratio*0.24*cosf(0));
		glNormal3f(-1, 0, 0);

		for (int b = 1; b <= head_slice + 1; ++b)
		{
			float cirAngle = -6.28f*(float)b / (float)head_slice;
			if (b == head_slice) cirAngle = 0;

			P[1] = Point3f(1, ratio*2.4*sinf(cirAngle), ratio*2.4*cosf(cirAngle));

			glBegin(GL_TRIANGLES);

			glVertex3f(P[0].X(), P[0].Y(), P[0].Z());
			glVertex3f(1, 0, 0);
			glVertex3f(P[1].X(), P[1].Y(), P[1].Z());

			glEnd();

			P[0] = P[1];
		}

		/////////////////////////////////

		p0 = Point3f(1.3, 0, 0); 

		N[0] = Point3f(1, sinf(0), cosf(0));
		P[0] = Point3f(1., ratio*2.4*sinf(0), ratio*2.4*cosf(0));

		for (int b = 1; b <= head_slice; ++b)
		{
			float cirAngle = -6.28f*(float)b / (float)head_slice;
			if (b == head_slice) cirAngle = 0;

			N[1] = Point3f(1.f, sinf(cirAngle), cosf(cirAngle));
			P[1] = Point3f(1, ratio*2.4*sinf(cirAngle), ratio*2.4*cosf(cirAngle));

			glBegin(GL_TRIANGLES);

			Point3f n = ((P[0] - p0) ^ (P[1] - p0)).Normalize();

			// ���ĵ�

			glNormal3f(n.X(), n.Y(), n.Z());   
			glVertex3f(p0[0], p0[1], p0[2]);  
											 

			glNormal3f(N[0].X(), N[0].Y(), N[0].Z()); 
			glVertex3f(P[0].X(), P[0].Y(), P[0].Z()); 

													 

			glNormal3f(N[1].X(), N[1].Y(), N[1].Z()); 
			glVertex3f(P[1].X(), P[1].Y(), P[1].Z()); 

			glEnd();

			N[0] = N[1];
			P[0] = P[1];
		}

		glEndList();
	}
	glPopMatrix();
}

void glShape::GetRotationMatrix(Point3f up0, Point3f front0, Point3f up1, Point3f front1, float * mat)
{
	AlgebraCompute::GetRotationMatrix(up0, front0, up1, front1, mat);
	//Quaternionf rot0, rot1; // 两次旋转
	//float co;
	//Point3f axis;
	//float angle;
	//up0.Normalize();
	//front0.Normalize();
	//up1.Normalize();
	//front1.Normalize();
	//co = up0*up1;
	//if (co < -1.0)co = -1.0;
	//else if (co>1.0)co = 1.0;
	//angle = acos(co);
	//axis = up0 ^ up1;
	//if (axis.SquaredNorm() <= 0.00001f)
	//{
	//	Point3f tempAxis(up0.y, up0.x + 3, up0.z);
	//	tempAxis.Normalize();
	//	axis = tempAxis ^ up0;
	//}
	//axis.Normalize();

	//rot0.FromAxis(angle, axis);

	//Matrix44f matrix0, matrix1;
	//rot0.ToMatrix(matrix0);

	//Point3f front00;

	//front00[0] = front0[0] * matrix0[0][0] + front0[1] * matrix0[0][1] + front0[2] * matrix0[0][2];
	//front00[1] = front0[0] * matrix0[1][0] + front0[1] * matrix0[1][1] + front0[2] * matrix0[1][2];
	//front00[2] = front0[0] * matrix0[2][0] + front0[1] * matrix0[2][1] + front0[2] * matrix0[2][2];

	//front00.Normalize();
	//co = up1*front00;
	//co = front1*up1;

	//co = front00*front1;

	//if (co < -1.0)co = -1.0;
	//else if (co>1.0)co = 1.0;
	//angle = acos(co);
	//axis = front00 ^ front1; // 这里直接使用 up1 做轴线会出错
	//if (axis.SquaredNorm() <= 0.00001f)
	//{
	//	Point3f tempAxis(front1.y, front1.x + 3, front1.z);
	//	tempAxis.Normalize();
	//	axis = tempAxis ^ front1;
	//}
	//axis.Normalize();
	//rot1.FromAxis(angle, axis);

	//rot1.ToMatrix(matrix1);

	//Matrix44f matrix = matrix1 * matrix0;
	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		mat[i * 4 + j] = matrix[i][j];
	//	}
	//}
}
