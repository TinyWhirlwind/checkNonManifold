#ifndef _TRACKMODE_
#define _TRACKMODE_

#include "Common/line3.h"
#include "Common/plane3.h"
#include "Common/segment3.h"
#include "Common/ray3.h"
#include "SceneRender/Camera.h"

namespace sn3DCore
{
	class Trackball;

	/* manipulator 基类 所有 manipulator必须继承此基类*/
	class TrackMode 
	{
	public:
		virtual ~TrackMode () {}
		/*默认鼠标拖动事件操作器 trackball :manipulator manager*/
		virtual void Apply (Trackball * trackball, Point3f new_point);
		/*默认鼠标滚轮事件操作器*/
		virtual void Apply (Trackball * trackball, float WheelNotch);
		/*操作器开始函数 */
		virtual void SetAction ();
		/*默认操作器重置函数，如果有状态的话*/
		virtual void Reset ();
		/*操作器名*/
		virtual const char *Name ()
		{
			return "TrackMode";
		};
		/*默认操作器绘制函数*/
		virtual void Draw (Trackball * trackball);
		/*是否可切换操作器
		操作管理器可以在鼠标操作其间在当前操作器与其它操作器之间进行切换
		对于有状态的操作器，应取消切换，保持状态一致性与Undo操作统一
		如果允许返回false */
		virtual bool isSticky();
		/*Undo函数,如果操作器有状态，应该支持Undo函数*/
		virtual void Undo();
	}; 

	/*不活动操作器,只用于绘制不活动trackball，并用于trackball mode切换*/
	class InactiveMode:public TrackMode 
	{
	public:
		/*返回操作器名称*/
		const char *Name ()
		{
			return "InactiveMode";
		};
		/*绘制*/
		void Draw (Trackball * trackball);
	};


	/*旋转操作器,有状态,由鼠标坐标决定其行为*/
	class SphereMode:public TrackMode 
	{
	public:
		/*用户拖动鼠标，使物体旋转*/
		void Apply (Trackball * trackball, Point3f new_point);

		void SetDirection(Trackball * tb,Point3f dir);

		void Draw (Trackball * trackball);
	};

	/*平移操作器,实现二维平移,无状态*/
	class PanMode:public TrackMode 
	{
	public:
		/*用户拖动鼠标，使物体平移*/
		void Apply (Trackball * trackball, Point3f new_point);
		/*返回操作器名字*/
		const char *Name () 
		{
			return "PanMode";
		};
		/*绘制操作器*/
		void Draw (Trackball * trackball);
	};

	/*缩放操作器,用户上下拖动鼠标或滚动鼠标滚轮,缩放物体,无状态*/
	class ScaleMode:public TrackMode 
	{
	public:
		/*返回操作器名称*/
		const char *Name () 
		{
			return "ScaleMode";
		};
		/*应用缩放，向上拖动鼠标，缩小物体；向下拖动鼠标，放大物体*/
		void Apply (Trackball * trackball, Point3f new_point);
		/*应用缩放，向上滚动滚轮，缩小物体；向下滚动滚轮，放大物体。*/
		void Apply (Trackball * trackball, float WheelNotch);
		/*绘制操作器*/
		void Draw (Trackball * trackball);
	};
}

#endif
