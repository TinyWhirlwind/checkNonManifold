#include "AxisIcon.h"

#include <windows.h>
#include <GL\glew.h>
#include "Common/Point3.h"

AxisIcon::AxisIcon()
{
	ambient[0] = 0.1f; ambient[1] = 0.1f; ambient[2] = 0.1f; ambient[3] = 1.0f;
	diffuse[0] = 0.8f; diffuse[1] = 0.8f; diffuse[2] = 0.8f; diffuse[3] = 1.0f;
	specular[0] = 0.3f; specular[1] = 0.3f; specular[2] = 0.3f; specular[3] = 1.0f;

	ambientFancyBack[0] = 0.0f;
	ambientFancyBack[1] = 0.0f;
	ambientFancyBack[2] = 0.0f;
	ambientFancyBack[3] = 1.0f;

	diffuseFancyBack[0] = 1.0f;
	diffuseFancyBack[1] = 0.7f;
	diffuseFancyBack[2] = 0.7f;
	diffuseFancyBack[3] = 1.0f;

	specularFancyBack[0] = 1.0f;
	specularFancyBack[1] = 1.0f;
	specularFancyBack[2] = 1.0f;
	specularFancyBack[3] = 1.0f;

	ambientFancyFront[0] = 0.0f;
	ambientFancyFront[1] = 0.0f;
	ambientFancyFront[2] = 0.0f;
	ambientFancyFront[3] = 1.0f;

	diffuseFancyFront[0] = 0.7f;
	diffuseFancyFront[1] = 0.7f;
	diffuseFancyFront[2] = 0.7f;
	diffuseFancyFront[3] = 1.0f;

	specularFancyFront[0] = 1.0f;
	specularFancyFront[1] = 1.0f;
	specularFancyFront[2] = 1.0f;
	specularFancyFront[3] = 1.0f;

	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 1.0;
	position[3] = 0.0;

}
void AxisIcon::Cylinder(int slices, float lenght, float width)
{
	Point3f p0;
	Point3f p1;
	glPushMatrix();
	glScaled(lenght, width, width);

	float step = 6.28f / (float)slices;
	float angle = 0;

	//绘制圆柱侧面
	glBegin(GL_TRIANGLE_STRIP);
	for (int b = 0; b <= slices; ++b)
	{
		if (b == slices) angle = 0.0;
		p0 = Point3f(0, sin(angle), cos(angle));
		p1 = p0; p1[0] = 1.f;
		glNormal3f(p0[0], p0[1], p0[2]);
		glVertex3d(p0[0], p0[1], p0[2]);
		glVertex3d(p1[0], p1[1], p1[2]);
		angle += step;
	}
	glEnd();

	//绘制圆柱底面
	angle = 0;
	p0 = Point3f(0, 0, 0);

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(-1, 0, 0);
	glVertex3d(p0[0], p0[1], p0[2]);
	for (int b = 0; b <= slices - 1; ++b)
	{
		glNormal3f(-1, 0, 0);
		p1 = Point3f(0, sin(angle), cos(angle));
		glVertex3d(p1[0], p1[1], p1[2]);
		angle += step;
	}
	glNormal3f(-1, 0, 0);
	glVertex3d(0, 0, 1);
	glEnd();

	//绘制圆柱顶面
	angle = 6.28;
	p0 = Point3f(1, 0, 0);

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(1, 0, 0);
	glVertex3d(p0[0], p0[1], p0[2]);
	for (int b = 0; b <= slices - 1; ++b)
	{
		glNormal3f(1, 0, 0);
		p1 = Point3f(1, sin(angle), cos(angle));
		glVertex3d(p1[0], p1[1], p1[2]);
		angle -= step; //逆时针
	}
	glNormal3f(1, 0, 0);
	glVertex3d(1, 0, 1);
	glEnd();
	glPopMatrix();
}

void AxisIcon::Cone(int slices, float lenght, float width)
{
	Point3f p0;//顶点或中心点
	Point3f P[2];
	Point3f N[2];

	glPushMatrix();
	glScaled(lenght, width, width); //对高度1底面半径为1的圆锥进行缩放

	//绘制侧面
	p0 = Point3f(1.0, 0, 0);
	N[0] = Point3f(1.f, sinf(0), cosf(0)); //没有归一化
	P[0] = Point3f(0, sinf(0), cosf(0));

	for (int b = 1; b <= slices; ++b)
	{
		float angle = -6.28f*(float)b / (float)slices;//逆时针
		if (b == slices) angle = 0;

		N[1] = Point3f(1.f, sinf(angle), cosf(angle));
		P[1] = Point3f(0, sinf(angle), cosf(angle));

		glBegin(GL_TRIANGLES);
		Point3f n = ((P[0] - p0) ^ (P[1] - p0)).Normalize();
		glNormal3f(n[0], n[1], n[2]);
		glVertex3f(p0[0], p0[1], p0[2]);
		glNormal3f(N[0][0], N[0][1], N[0][2]);
		glVertex3f(P[0][0], P[0][1], P[0][2]);
		glNormal3f(N[1][0], N[1][1], N[1][2]);
		glVertex3f(P[1][0], P[1][1], P[1][2]);
		glEnd();

		N[0] = N[1];
		P[0] = P[1];
	}

	//绘制底面
	p0 = Point3f(0, 0, 0);
	P[0] = Point3f(0, sinf(0), cosf(0));

	for (int b = 1; b <= slices; ++b)
	{
		float angle = -6.28f*(float)b / (float)slices;
		if (b == slices) angle = 0;

		Point3f n = Point3f(-1.f, 0, 0);
		P[1] = Point3f(0, sinf(angle), cosf(angle));

		glBegin(GL_TRIANGLES);
		glNormal3f(n[0], n[1], n[2]);
		glVertex3f(P[0][0], P[0][1], P[0][2]);
		glVertex3f(p0[0], p0[1], p0[2]);
		glVertex3f(P[1][0], P[1][1], P[1][2]);
		glEnd();

		P[0] = P[1];
	}
	glPopMatrix();
}

void AxisIcon::Arrow(float body_lengtht, float body_width, float head_lenght, float head_width, int body_slice, int head_slice)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	Cylinder(body_slice, body_lengtht, body_width);//绘制圆柱
	glPopMatrix();
	glPushMatrix();
	glTranslatef(body_lengtht, 0, 0);
	Cone(head_slice, head_lenght, head_width);//绘制圆锥
	glPopMatrix();
	glPopAttrib();
}

void AxisIcon::MoveArrow(float scale, int body_slice, int head_slice)
{
	Point3f p0;
	Point3f p1;
	Point3f P[2];
	Point3f N[2];

	glScalef(scale, scale, scale);

	///////////////////////////        绘制圆柱侧面

	int segment = 50;
	float cirAngle = 0;
	float step = 6.28f / (float)body_slice;

	for (int a = -segment; a < segment; ++a)
	{
		cirAngle = 0;
		glBegin(GL_TRIANGLE_STRIP);
		for (int b = 0; b <= body_slice; ++b)
		{
			if (b == body_slice) cirAngle = 0;

			p0 = Point3f((float)a / (float)segment, 0.1*sin(cirAngle), 0.1*cos(cirAngle));
			p1 = p0; p1[0] = (float)(a + 1) / (float)segment;

			glNormal3f(0, p0[1], p0[2]); //旋转后的法向量
			glVertex3d(p0[0], p0[1], p0[2]);
			glVertex3d(p1[0], p1[1], p1[2]);

			cirAngle += step;
		}
		glEnd();
	}
	//////////////////////    绘制相邻圆锥底面

	cirAngle = 0;
	P[0] = Point3f(-1, 0.24*sinf(0), 0.24*cosf(0));
	glNormal3f(1, 0, 0);

	for (int b = 1; b <= head_slice; ++b)
	{
		float cirAngle = -6.28f*(float)b / (float)head_slice;
		if (b == head_slice) cirAngle = 0;

		P[1] = Point3f(-1, 0.24*sinf(cirAngle), 0.24*cosf(cirAngle));

		glBegin(GL_TRIANGLES);
		glVertex3f(-1, 0, 0);
		glVertex3f(P[0].X(), P[0].Y(), P[0].Z());
		glVertex3f(P[1].X(), P[1].Y(), P[1].Z());
		glEnd();

		P[0] = P[1];
	}

	P[0] = Point3f(1, 0.24*sinf(0), 0.24*cosf(0));
	glNormal3f(-1, 0, 0);

	for (int b = 1; b <= head_slice; ++b)
	{
		float cirAngle = -6.28f*(float)b / (float)head_slice;
		if (b == head_slice) cirAngle = 0;

		P[1] = Point3f(1, 0.24*sinf(cirAngle), 0.24*cosf(cirAngle));

		glBegin(GL_TRIANGLES);
		glVertex3f(P[0].X(), P[0].Y(), P[0].Z());
		glVertex3f(1, 0, 0);
		glVertex3f(P[1].X(), P[1].Y(), P[1].Z());
		glEnd();

		P[0] = P[1];
	}

	/////////////////////////////////               绘制圆锥侧面

	p0 = Point3f(-1.5, 0, 0); //圆锥原顶点

	N[0] = Point3f(-1, sinf(0), cosf(0)); //没有归一化
	P[0] = Point3f(-1., 0.24*sinf(0), 0.24*cosf(0));

	for (int b = 1; b <= head_slice; ++b)
	{
		float cirAngle = -6.28f*(float)b / (float)head_slice;//逆时针
		if (b == head_slice) cirAngle = 0;

		N[1] = Point3f(-1.f, sinf(cirAngle), cosf(cirAngle));
		P[1] = Point3f(-1, 0.24*sinf(cirAngle), 0.24*cosf(cirAngle));

		glBegin(GL_TRIANGLES);

		Point3f n = ((P[1] - p0) ^ (P[0] - p0)).Normalize();

		// 第一个点

		glNormal3f(N[0].X(), N[0].Y(), N[0].Z()); ///////// 法线
		glVertex3f(P[0].X(), P[0].Y(), P[0].Z()); /////////// 顶点

		// 中心点

		glNormal3f(n.X(), n.Y(), n.Z());   ///////// 法线
		glVertex3f(p0[0], p0[1], p0[2]);  ////////顶点

		// 第三个点

		glNormal3f(N[1].X(), N[1].Y(), N[1].Z()); ///////法线	   
		glVertex3f(P[1].X(), P[1].Y(), P[1].Z()); /////////顶点

		glEnd();

		N[0] = N[1];
		P[0] = P[1];
	}
	p0 = Point3f(1.5, 0, 0); //圆锥原顶点

	N[0] = Point3f(1, sinf(0), cosf(0)); //没有归一化
	P[0] = Point3f(1., 0.24*sinf(0), 0.24*cosf(0));

	for (int b = 1; b <= head_slice; ++b)
	{
		float cirAngle = -6.28f*(float)b / (float)head_slice;//逆时针
		if (b == head_slice) cirAngle = 0;

		N[1] = Point3f(1.f, sinf(cirAngle), cosf(cirAngle));
		P[1] = Point3f(1, 0.24*sinf(cirAngle), 0.24*cosf(cirAngle));

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

void AxisIcon::RotateArrow(float scale, int body_slice, int head_slice)
{
	float angle;// 弯曲角度,弯曲平面是xz平面,轴线是x轴
	float r = 1.5; //弯曲半径

	float rotate0[3][3];//法线旋转矩阵
	float rotate1[3][3];

	Point3f axisPoint0(0, 0, 0);//轴上顶点坐标
	Point3f axisPoint1(0, 0, 0);
	Point3f point; //待计算点

	Point3f p0;
	Point3f p1;

	Point3f P[2];
	Point3f N[2];

	glScalef(scale, scale, scale);

	///////////////////////////          绘制圆柱侧面

	int segment = 100;
	float cirAngle = 0;
	float step = 6.28f / (float)body_slice;

	for (int a = -segment; a < segment; ++a)
	{
		angle = ((float)a / (float)segment) / (float)r;//当前弯曲角度

		axisPoint0.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
		axisPoint0.Z() = (float)r * cosf(angle) - (float)r;//向 -z方向弯曲

		rotate0[0][0] = cosf(angle);
		rotate0[0][1] = 0;
		rotate0[0][2] = sinf(angle);
		rotate0[1][0] = 0;
		rotate0[1][1] = 1;
		rotate0[1][2] = 0;
		rotate0[2][0] = -sinf(angle);
		rotate0[2][1] = 0;
		rotate0[2][2] = cosf(angle);

		angle = ((float)(a + 1) / (float)segment) / (float)r;//当前弯曲角度

		axisPoint1.X() = (float)r * sinf(angle);
		axisPoint1.Z() = (float)r * cosf(angle) - (float)r;//向 -z方向弯曲

		rotate1[0][0] = cosf(angle);
		rotate1[0][1] = 0;
		rotate1[0][2] = sinf(angle);
		rotate1[1][0] = 0;
		rotate1[1][1] = 1;
		rotate1[1][2] = 0;
		rotate1[2][0] = -sinf(angle);
		rotate1[2][1] = 0;
		rotate1[2][2] = cosf(angle);

		cirAngle = 0;

		glBegin(GL_TRIANGLE_STRIP);
		for (int b = 0; b <= body_slice; ++b)
		{
			if (b == body_slice) cirAngle = 0;

			p0 = Point3f((float)a / (float)segment, 0.1*sin(cirAngle), 0.1*cos(cirAngle));
			p1 = p0; p1[0] = (float)(a + 1) / (float)segment;

			point.X() = 0;  //相对于原中心点,即中心移动到原点
			point.Y() = 0;
			point.Z() = p0.Z();

			p0.X() = point.X()*rotate0[0][0] + point.Y()*rotate0[0][1] + point.Z()*rotate0[0][2];  //旋转
			p0.Z() = point.X()*rotate0[2][0] + point.Y()*rotate0[2][1] + point.Z()*rotate0[2][2];

			glNormal3f(p0[0], p0[1], p0[2]); //旋转后的法向量

			p0.X() += axisPoint0.X(); //移动到当前中心点
			p0.Z() += axisPoint0.Z();

			glVertex3d(p0[0], p0[1], p0[2]);

			point.X() = 0;
			point.Y() = 0;
			point.Z() = p1.Z();

			p1.X() = point.X()*rotate1[0][0] + point.Y()*rotate1[0][1] + point.Z()*rotate1[0][2];
			p1.Z() = point.X()*rotate1[2][0] + point.Y()*rotate1[2][1] + point.Z()*rotate1[2][2];

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
	angle = (float)-1 / (float)r;//当前弯曲角度
	axisPoint0.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint0.Z() = (float)r * cosf(angle) - (float)r;//向 -z方向弯曲

	rotate0[0][0] = cosf(angle);
	rotate0[0][1] = 0;
	rotate0[0][2] = sinf(angle);
	rotate0[1][0] = 0;
	rotate0[1][1] = 1;
	rotate0[1][2] = 0;
	rotate0[2][0] = -sinf(angle);
	rotate0[2][1] = 0;
	rotate0[2][2] = cosf(angle);

	P[0] = Point3f(-1, 0.24*sinf(0), 0.24*cosf(0));

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

		P[1] = Point3f(-1, 0.24*sinf(cirAngle), 0.24*cosf(cirAngle));

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

	angle = (float)1 / (float)r;//当前弯曲角度

	axisPoint1.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint1.Z() = (float)r * cosf(angle) - (float)r;//向 -z方向弯曲

	rotate1[0][0] = cosf(angle);
	rotate1[0][1] = 0;
	rotate1[0][2] = sinf(angle);
	rotate1[1][0] = 0;
	rotate1[1][1] = 1;
	rotate1[1][2] = 0;
	rotate1[2][0] = -sinf(angle);
	rotate1[2][1] = 0;
	rotate1[2][2] = cosf(angle);

	P[0] = Point3f(1, 0.24*sinf(0), 0.24*cosf(0));

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

		P[1] = Point3f(1, 0.24*sinf(cirAngle), 0.24*cosf(cirAngle));

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

	angle = (float)-1.5 / (float)r;//当前弯曲角度

	axisPoint1.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint1.Z() = (float)r * cosf(angle) - (float)r;//向 -z方向弯曲

	angle = (float)-1 / (float)r;//当前弯曲角度

	axisPoint0.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint0.Z() = (float)r * cosf(angle) - (float)r;//向 -z方向弯曲

	rotate0[0][0] = cosf(angle);
	rotate0[0][1] = 0;
	rotate0[0][2] = sinf(angle);
	rotate0[1][0] = 0;
	rotate0[1][1] = 1;
	rotate0[1][2] = 0;
	rotate0[2][0] = -sinf(angle);
	rotate0[2][1] = 0;
	rotate0[2][2] = cosf(angle);

	p0 = Point3f(-1.5, 0, 0); //圆锥原顶点

	N[0] = Point3f(-1, sinf(0), cosf(0)); //没有归一化
	P[0] = Point3f(-1., 0.24*sinf(0), 0.24*cosf(0));

	for (int b = 1; b <= head_slice; ++b)
	{
		float cirAngle = -6.28f*(float)b / (float)head_slice;//逆时针
		if (b == head_slice) cirAngle = 0;

		N[1] = Point3f(-1.f, sinf(cirAngle), cosf(cirAngle));
		P[1] = Point3f(-1, 0.24*sinf(cirAngle), 0.24*cosf(cirAngle));

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

	angle = (float)1.5 / (float)r;//当前弯曲角度
	axisPoint1.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint1.Z() = (float)r * cosf(angle) - (float)r;//向 -z方向弯曲

	angle = (float)1 / (float)r;//当前弯曲角度
	axisPoint0.X() = (float)r * sinf(angle);    //计算在轴线上的点和旋转矩阵
	axisPoint0.Z() = (float)r * cosf(angle) - (float)r;//向 -z方向弯曲

	rotate0[0][0] = cosf(angle);
	rotate0[0][1] = 0;
	rotate0[0][2] = sinf(angle);
	rotate0[1][0] = 0;
	rotate0[1][1] = 1;
	rotate0[1][2] = 0;
	rotate0[2][0] = -sinf(angle);
	rotate0[2][1] = 0;
	rotate0[2][2] = cosf(angle);

	p0 = Point3f(1.5, 0, 0); //圆锥原顶点

	N[0] = Point3f(1, sinf(0), cosf(0)); //没有归一化
	P[0] = Point3f(1., 0.24*sinf(0), 0.24*cosf(0));

	for (int b = 1; b <= head_slice; ++b)
	{
		float cirAngle = -6.28f*(float)b / (float)head_slice;//逆时针
		if (b == head_slice) cirAngle = 0;

		N[1] = Point3f(1.f, sinf(cirAngle), cosf(cirAngle));
		P[1] = Point3f(1, 0.24*sinf(cirAngle), 0.24*cosf(cirAngle));

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
}

void AxisIcon::DrawX()
{
	float m_XVertices[168] = { -19.3521,-4.6853,33.2056,-3.1334,-4.6853,10.9040,-3.1334,5.3147,10.9040,
						  -19.3521, 5.3147, 33.2056,-0.1553,-4.6853,6.4696,-0.1553,5.3147,6.4696,
						  2.5416, -4.6853, 10.4478,2.5416, 5.3147, 10.4478,18.6604, -4.6853, 33.2056,
						  18.6604, 5.3147, 33.2056,29.0604, -4.6853, 33.2056,29.0604, 5.3147, 33.2056,
						  5.0744, -4.6853, -0.3866,5.0744, 5.3147, -0.3866,27.2604, -4.6853, -30.8944,
						  27.2604, 5.3147, -30.8944,17.9401, -4.6853, -30.8944,17.9401, 5.3147, -30.8944,
						  5.2822, -4.6853, -13.9726,5.2822, 5.3147, -13.9726,4.4586, -4.6853, -12.8950,
						  4.4586, 5.3147, -12.8950,3.6654, -4.6853, -11.8311,3.6654, 5.3147, -11.8311,
						  2.9026, -4.6853, -10.7808,2.9026, 5.3147, -10.7808,2.1703, -4.6853, -9.7442,
						  2.1703, 5.3147, -9.7442,1.4683, -4.6853, -8.7213,1.4683, 5.3147, -8.7213,
						  0.7968, -4.6853, -7.7120,0.7968, 5.3147, -7.7120,0.1557, -4.6853, -6.7163,
						  0.1557, 5.3147, -6.7163,-0.2930, -4.6853, -7.4932,-0.2930, 5.3147, -7.4932,
						  -0.8327, -4.6853, -8.3748,-0.8327, 5.3147, -8.3748,-1.4635, -4.6853, -9.3612,
						  -1.4635, 5.3147, -9.3612,-2.1854, -4.6853, -10.4523,-2.1854, 5.3147, -10.4523,
						  -2.9983, -4.6853, -11.6481,-2.9983, 5.3147, -11.6481,-3.9024, -4.6853, -12.9486,
						  -3.9024, 5.3147, -12.9486,-4.8974, -4.6853, -14.3538,-4.8974, 5.3147, -14.3538,
						  -16.7084, -4.6853, -30.8944,-16.7084, 5.3147, -30.8944,-26.7396, -4.6853, -30.8944,
						  -26.7396, 5.3147, -30.8944,-4.9006, -4.6853, -0.0585,-4.9006, 5.3147, -0.0585,
						  -29.5396, -4.6853, 33.2056,-29.5396, 5.3147, 33.2056 };
	int m_XFaces[324] = { 1, 2, 3, 1, 3, 4, 2, 5, 6, 2, 6, 3,
				   5, 7, 8, 5, 8, 6, 7, 9, 10, 7, 10, 8, 9, 11, 12,
				   9, 12, 10, 11, 13, 14, 11, 14, 12, 13, 15, 16,
				   13, 16, 14 ,15, 17, 18, 15, 18, 16, 17, 19, 20,
				   17, 20, 18, 19, 21, 22, 19, 22, 20, 21, 23, 24,
				   21, 24, 22, 23, 25, 26, 23, 26, 24, 25, 27, 28,
				   25, 28, 26, 27, 29, 30, 27, 30, 28, 29, 31, 32,
				   29, 32, 30, 31, 33, 34, 31, 34, 32, 33, 35, 36,
				   33, 36, 34, 35, 37, 38, 35, 38, 36, 37, 39, 40,
				   37, 40, 38, 39, 41, 42, 39, 42, 40, 41, 43, 44,
				   41, 44, 42, 43, 45, 46, 43, 46, 44, 45, 47, 48,
				   45, 48, 46, 47, 49, 50, 47, 50, 48, 49, 51, 52,
				   49, 52, 50, 51, 53, 54, 51, 54, 52, 53, 55, 56,
				   53, 56, 54, 55, 1, 4, 55, 4, 56, 53, 1, 55,
				   53, 2, 1, 53, 5, 2, 13, 17, 15, 13, 19, 17, 13, 21, 19,
				   13, 23, 21, 13, 25, 23, 13, 27, 25, 13, 29, 27,
				   9, 13, 11, 7, 13, 9, 5, 13, 7, 49, 53, 51,
				   47, 53, 49, 45, 53, 47, 43, 53, 45, 41, 53, 43,
				   39, 53, 41, 37, 53, 39, 13, 31, 29, 35, 53, 37,
				   33, 53, 35, 13, 33, 31, 53, 13, 5, 33, 13, 53,
				   54, 56, 4, 54, 4,3, 54, 3 ,6, 14, 16, 18, 14, 18, 20,
				   14, 20, 22, 14, 22, 24, 14, 24, 26, 14, 26, 28, 14, 28, 30,
				   10, 12, 14, 8, 10, 14, 6, 8, 14, 50, 52, 54, 48, 50, 54,
				   46, 48, 54, 44, 46, 54, 42, 44, 54, 40, 42, 54, 38, 40, 54,
				   14, 30, 32, 36, 38, 54, 34, 36, 54, 14, 32, 34, 54, 6, 14,
				   34, 54, 14 };


	GLfloat colorv[4];
	colorv[3] = 1.0;
	GLfloat colord[4];
	colord[3] = 1.0;
	GLfloat colors[4];
	colors[3] = 1.0;

	colorv[0] = 0.6; colorv[1] = 0.0; colorv[2] = 0.6;
	colord[0] = 0.6; colord[1] = 0.0; colord[2] = 0.6;
	colors[0] = 0.6; colors[1] = 0.0; colors[2] = 0.6;

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colors);


	glPushMatrix();

	//glRotatef(90,0.0,1.0,0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glScalef(0.008, 0.008, 0.008);
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < 108; i++)
	{
		Point3f p0, p1, p2;

		p0 = Point3f(m_XVertices[3 * (m_XFaces[3 * i] - 1)], m_XVertices[3 * (m_XFaces[3 * i] - 1) + 1], m_XVertices[3 * (m_XFaces[3 * i] - 1) + 2]);
		p1 = Point3f(m_XVertices[3 * (m_XFaces[3 * i + 1] - 1)], m_XVertices[3 * (m_XFaces[3 * i + 1] - 1) + 1], m_XVertices[3 * (m_XFaces[3 * i + 1] - 1) + 2]);
		p2 = Point3f(m_XVertices[3 * (m_XFaces[3 * i + 2] - 1)], m_XVertices[3 * (m_XFaces[3 * i + 2] - 1) + 1], m_XVertices[3 * (m_XFaces[3 * i + 2] - 1) + 2]);

		Point3f normal;

		Point3f e1, e2;
		e1 = p1 - p0;
		e2 = p2 - p1;
		normal = e1^e2;
		normal.Normalize();
		glNormal3f(normal.X(), normal.Y(), normal.Z());

		glVertex3f(p0.X(), p0.Y(), p0.Z());
		glVertex3f(p1.X(), p1.Y(), p1.Z());
		glVertex3f(p2.X(), p2.Y(), p2.Z());
	}
	glEnd();

	glPopMatrix();
}

void AxisIcon::DrawY()
{
	float m_YVertices[66] = { 3.9243, -4.5580, 27.5706,3.9243, -4.5580, 0.4175,3.9243, 5.4420, 0.4175,
							3.9243, 5.4420, 27.5706,29.5243, -4.5580, -36.5294,29.5243, 5.4420, -36.5294,
							19.6462, -4.5580, -36.5294,19.6462, 5.4420, -36.5294,7.2274, -4.5580, -17.6294,
							7.2274, 5.4420, -17.6294,0.1696, -4.5580, -6.3106,0.1696, 5.4420, -6.3106,
							-6.3726, -4.5580, -17.1481,-6.3726, 5.4420, -17.1481,-19.0710, -4.5580, -36.5294,
							-19.0710, 5.4420, -36.5294,-29.2757, -4.5580, -36.5294,-29.2757, 5.4420, -36.5294,
							-4.5757, -4.5580, 0.4175,-4.5757, 5.4420, 0.4175,-4.5757, -4.5580, 27.5706,
							-4.5757, 5.4420, 27.5706 };
	int m_YFaces[120] = { 1, 2, 3 ,1, 3, 4, 2, 5, 6 ,2, 6, 3 ,5, 7, 8 ,
					   5, 8, 6 ,7, 9, 10 ,7, 10, 8 ,9, 11, 12 ,9, 12, 10 ,
					   11, 13, 14 ,11, 14, 12 ,13, 15, 16 ,13, 16, 14 ,15, 17, 18 ,
					   15, 18, 16 ,17, 19, 20 ,17, 20, 18 ,19, 21, 22 ,19, 22, 20 ,
					   21, 1, 4 ,21, 4, 22 ,2, 7, 5 ,2, 9, 7 ,2, 11, 9 ,15, 19, 17,
					   13, 19, 15 ,11, 19, 13 ,2, 19, 11 ,1, 19, 2 ,1, 21, 19 ,
					   3, 6, 8 ,3, 8, 10 ,3, 10, 12 ,16, 18, 20 ,14, 16, 20 ,12, 14, 20 ,
					   3, 12, 20 ,4, 3, 20 ,4, 20, 22 };

	GLfloat colorv[4];
	colorv[3] = 1.0;
	GLfloat colord[4];
	colord[3] = 1.0;
	GLfloat colors[4];
	colors[3] = 1.0;

	colorv[0] = 0.6; colorv[1] = 0.0; colorv[2] = 0.6;
	colord[0] = 0.6; colord[1] = 0.0; colord[2] = 0.6;
	colors[0] = 0.6; colors[1] = 0.0; colors[2] = 0.6;

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colors);


	glPushMatrix();

	//glRotatef(-90,1.0,0.0,0.0);
	//glRotatef(90,0.0,1.0,0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glScalef(0.008, 0.008, 0.008);
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < 40; i++)
	{
		Point3f p0, p1, p2;

		p0 = Point3f(m_YVertices[3 * (m_YFaces[3 * i] - 1)], m_YVertices[3 * (m_YFaces[3 * i] - 1) + 1], m_YVertices[3 * (m_YFaces[3 * i] - 1) + 2]);
		p1 = Point3f(m_YVertices[3 * (m_YFaces[3 * i + 1] - 1)], m_YVertices[3 * (m_YFaces[3 * i + 1] - 1) + 1], m_YVertices[3 * (m_YFaces[3 * i + 1] - 1) + 2]);
		p2 = Point3f(m_YVertices[3 * (m_YFaces[3 * i + 2] - 1)], m_YVertices[3 * (m_YFaces[3 * i + 2] - 1) + 1], m_YVertices[3 * (m_YFaces[3 * i + 2] - 1) + 2]);

		Point3f normal;

		Point3f e1, e2;
		e1 = p1 - p0;
		e2 = p2 - p1;
		normal = e1^e2;
		normal.Normalize();
		glNormal3f(normal.X(), normal.Y(), normal.Z());

		glVertex3f(p0.X(), p0.Y(), p0.Z());
		glVertex3f(p1.X(), p1.Y(), p1.Z());
		glVertex3f(p2.X(), p2.Y(), p2.Z());
	}
	glEnd();

	glPopMatrix();
}

void AxisIcon::DrawZ()
{
	float m_ZVertices[108] = { 26.2979, -5.1742, 30.6629,26.2979, -5.1742, 23.0629,26.2979, 4.8258, 23.0629,
							26.2979, 4.8258, 30.6629,-14.6521, -5.1742, 23.0629,-14.6521, 4.8258, 23.0629,
							-10.8427, -5.1742, 18.5472,-10.8427, 4.8258, 18.5472,25.1979, -5.1742, -25.8371,
							25.1979, 4.8258, -25.8371,25.1979, -5.1742, -33.4371,25.1979, 4.8258, -33.4371,
							-20.7021, -5.1742, -33.4371,-20.7021, 4.8258, -33.4371,-20.7021, -5.1742, -25.8371,
							-20.7021, 4.8258, -25.8371,15.0682, -5.1742, -25.8371,15.0682, 4.8258, -25.8371,
							14.1633, -5.1742, -24.8871,14.1633, 4.8258, -24.8871,13.2459, -5.1742, -23.8894,
							13.2459, 4.8258, -23.8894,12.3160, -5.1742, -22.8443,12.3160, 4.8258, -22.8443,
							11.3736, -5.1742, -21.7517,11.3736, 4.8258, -21.7517,10.4186, -5.1742, -20.6115,
							10.4186, 4.8258, -20.6115,9.4512, -5.1742, -19.4239,9.4512, 4.8258, -19.4239,
							8.4713, -5.1742, -18.1887,8.4713, 4.8258, -18.1887,-24.4021, -5.1742, 22.7566,
							-24.4021, 4.8258, 22.7566,-24.4021, -5.1742, 30.6629,-24.4021, 4.8258, 30.6629 };
	int m_ZFaces[204] = { 1, 2, 3 ,1, 3, 4 ,2, 5, 6 ,2, 6, 3 ,5, 7, 8 ,5, 8, 6 ,7, 9, 10 ,
					   7, 10, 8 ,9, 11, 12 ,9, 12, 10 ,11, 13, 14 ,11, 14, 12 ,13, 15, 16 ,
					   13, 16, 14 ,15, 17, 18 ,15, 18, 16 ,17, 19, 20 ,17, 20, 18 ,19, 21, 22 ,
					   19, 22, 20 ,21, 23, 24 ,21, 24, 22 ,23, 25, 26 ,23, 26, 24 ,25, 27, 28 ,
					   25, 28, 26 ,27, 29, 30 ,27, 30, 28 ,29, 31, 32 ,29, 32, 30 ,31, 33, 34 ,
					   31, 34, 32 ,33, 35, 36 ,33, 36, 34 ,35, 1, 4 ,35, 4, 36 ,33, 1, 35 ,13, 17, 15 ,
					   11, 17, 13 ,9, 17, 11 ,7, 17, 9 ,7, 19, 17 ,7, 21, 19 ,7, 23, 21 ,7, 25, 23 ,
					   7, 27, 25 ,7, 29, 27 ,7, 31, 29 ,7, 33, 31 ,5, 33, 7 ,5, 1, 33 ,2, 1, 5 ,
					   34, 36, 4, 14, 16, 18, 12, 14, 18 ,10, 12, 18 ,8 ,10, 18 ,8, 18, 20 ,8, 20, 22 ,
					   8, 22, 24 ,8, 24, 26 ,8, 26, 28 ,8, 28, 30 ,8, 30, 32 ,8, 32, 34 ,6, 8, 34 ,
					   6, 34, 4 ,3, 6, 4 };


	GLfloat colorv[4];
	colorv[3] = 1.0;
	GLfloat colord[4];
	colord[3] = 1.0;
	GLfloat colors[4];
	colors[3] = 1.0;

	colorv[0] = 0.6; colorv[1] = 0.0; colorv[2] = 0.6;
	colord[0] = 0.6; colord[1] = 0.0; colord[2] = 0.6;
	colors[0] = 0.6; colors[1] = 0.0; colors[2] = 0.6;

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colors);


	glPushMatrix();

	//glRotatef(90,0.0,1.0,0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	glScalef(0.008, 0.008, 0.008);
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < 68; i++)
	{
		Point3f p0, p1, p2;

		p0 = Point3f(m_ZVertices[3 * (m_ZFaces[3 * i] - 1)], m_ZVertices[3 * (m_ZFaces[3 * i] - 1) + 1], m_ZVertices[3 * (m_ZFaces[3 * i] - 1) + 2]);
		p1 = Point3f(m_ZVertices[3 * (m_ZFaces[3 * i + 1] - 1)], m_ZVertices[3 * (m_ZFaces[3 * i + 1] - 1) + 1], m_ZVertices[3 * (m_ZFaces[3 * i + 1] - 1) + 2]);
		p2 = Point3f(m_ZVertices[3 * (m_ZFaces[3 * i + 2] - 1)], m_ZVertices[3 * (m_ZFaces[3 * i + 2] - 1) + 1], m_ZVertices[3 * (m_ZFaces[3 * i + 2] - 1) + 2]);

		Point3f normal;

		Point3f e1, e2;
		e1 = p1 - p0;
		e2 = p2 - p1;
		normal = e1^e2;
		normal.Normalize();
		glNormal3f(normal.X(), normal.Y(), normal.Z());

		glVertex3f(p0.X(), p0.Y(), p0.Z());
		glVertex3f(p1.X(), p1.Y(), p1.Z());
		glVertex3f(p2.X(), p2.Y(), p2.Z());
	}
	glEnd();

	glPopMatrix();
}

void AxisIcon::SetRotate(Quaternionf ro)
{
	m_rotate = ro;
}

void AxisIcon::GLRender(int locx, int locy, int w, int h, float * mx)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);

	float ambientFancyBack[] = {0.f, 0.f, 0.f, 1.f};
	float diffuseFancyBack[] = {0.35f, 0.35f, 0.35f, 1.f};
	float specularFancyBack[] = {0.f, 0.f, 0.f, 1.f};

	float ambientFancyFront[] = {0.45f, 0.45f, 0.45f, 1.f};
	float diffuseFancyFront[] = {0.7f, 0.7f, 0.7f, 1.f};
	float specularFancyFront[] = {0.65f, 0.65f, 0.65f, 1.f};

	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientFancyFront);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseFancyFront);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularFancyFront);

	glDisable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientFancyBack);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseFancyBack);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularFancyBack);

	glEnable(GL_LIGHTING);
	GLfloat depthrange[2];
	glGetFloatv(GL_DEPTH_RANGE, depthrange);
	GLdouble projectionmatrix[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projectionmatrix);
	glEnable(GL_DEPTH_TEST);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glViewport(locx, locy, w, h);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-2.5, 2.5, -2.5, 2.5, -2.5, 2.5);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glLoadMatrixf(mx);

	GLfloat colorv[4];
	colorv[3] = 1.0;
	GLfloat colord[4];
	colord[3] = 1.0;
	GLfloat colors[4];
	colors[3] = 1.0;

	Matrix44<float> r;
	Quaternionf ro = m_rotate;
	ro[0] = -ro[0];
	ro.ToMatrix(r);
	Transpose(r);

	colorv[0] = 0.1; colorv[1] = 0.0; colorv[2] = 0.1;
	colord[0] = 0.8; colord[1] = 0.0; colord[2] = 0.8;
	colors[0] = 0.4; colors[1] = 0.0; colors[2] = 0.4;

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colors);

	glColor4f(1.0, 0.0, 1.0, 1.0); //
	glColor4f(1.0, 0.0, 0.0, 1.0); //x方向红色

	colorv[0] = 0.1; colorv[1] = 0.0; colorv[2] = 0.0;
	colord[0] = 0.8; colord[1] = 0.0; colord[2] = 0.0;
	colors[0] = 0.4; colors[1] = 0.0; colors[2] = 0.0;

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colors);

	glPushMatrix();
	Cylinder(50, 1, 0.1);
	glTranslatef(1, 0, 0);
	Cone(50, 1.0, 0.3);

	glTranslatef(1.2, 0.0, 0);
	glMultMatrixf(&r._a[0]);
	DrawX();

	glPopMatrix();

	glColor4f(0.0, 1.0, 0.0, 1.0); //y方向绿色

	colorv[0] = 0.0; colorv[1] = 0.1; colorv[2] = 0.0;
	colord[0] = 0.0; colord[1] = 0.8; colord[2] = 0.0;
	colors[0] = 0.0; colors[1] = 0.4; colors[2] = 0.0;

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colors);

	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	Cylinder(50, 1, 0.1);
	glTranslatef(1, 0, 0);
	Cone(50, 1.0, 0.3);
	glTranslatef(1.2, 0.0, 0);

	glRotatef(-90, 0, 0, 1);
	glMultMatrixf(&r._a[0]);
	DrawY();
	glPopMatrix();

	glColor4f(0.0, 0.0, 1.0, 1.0); //z方向蓝色

	colorv[0] = 0.0; colorv[1] = 0.0; colorv[2] = 0.1;
	colord[0] = 0.0; colord[1] = 0.0; colord[2] = 0.8;
	colors[0] = 0.0; colors[1] = 0.0; colors[2] = 0.4;

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colors);

	glPushMatrix();
	glRotatef(90, 0, -1, 0);
	Cylinder(50, 1, 0.1);
	glTranslatef(1, 0, 0);
	Cone(50, 1.0, 0.3);
	glTranslatef(1.2, 0.0, 0);
	glRotatef(-90, 0, -1, 0);
	glMultMatrixf(&r._a[0]);
	DrawZ();
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);//复原
}
