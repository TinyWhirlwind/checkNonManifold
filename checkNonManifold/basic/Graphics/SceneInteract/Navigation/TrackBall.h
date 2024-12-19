#ifndef _TRACKBALL_
#define _TRACKBALL_

#include <list>
#include <vector>
#include <map>

#include "Common/similarity.h"
#include "Common/color4.h"
#include "Trackmode.h"

namespace sn3DCore
{
	class Transform 
	{
	public:
		Transform();
		virtual ~Transform() {}

		Similarityf track;
		Point3f center; 
		float radius; 
	};

	class TrackMode;
	class Trackball: public Transform 
	{
	public:
		enum Button 
		{ 
			BUTTON_NONE   = 0x0000, //鼠标按钮未被按下
			BUTTON_LEFT   = 0x0001, //鼠标左按钮被按下
			BUTTON_MIDDLE = 0x0002, //鼠标中按钮被按下
			BUTTON_RIGHT  = 0x0004, //鼠标右按钮被按下
			WHEEL         = 0x0008, //鼠标滚轮激活
			KEY_SHIFT     = 0x0010, //Shift 键按下
			KEY_CTRL      = 0x0020, // Ctrl 键按下
			KEY_ALT       = 0x0040, // Alt 键按下
			HANDLE        = 0x0080  //应用定义状态激活
		};

	public:
		Trackball();
		virtual ~Trackball();

	public:
		void SetIdentity();
		void SetPosition(const Point3f &c, int millisec = 0);
		void SetRadius(float r);
		void SetScale(const float s) {radius=s;};
		void Translate(Point3f tr);//平移变换
		void Scale(const float f);//缩放变换
		void Apply ();//同时应用旋转、缩放、平移
		void ApplyRotate();//应用旋转
		void ApplyScale();//应用缩放
		void ApplyTranslate();//应用平移
		void GetView();
		void Apply(bool Draw);
		void DrawPostApply();
		void ApplyInverse();
		void Reset();

		// 鼠标操作支持函数
		void MouseDown(int x, int y, /*Button*/ int button);
		void MouseMove(int x, int y); 
		void MouseUp(int x, int y, /*Button */ int button); 
		void MouseWheel(float notch);
		void MouseWheel (float notch, /*Button */ int button);
		void ButtonUp(Button button);
		void ButtonDown(Button button);
		void Undo();
		void SetSpinnable(bool on);
		bool IsSpinnable();
		void SetSpinning(Quaternionf &spin);
		void StopSpinning();
		bool IsSpinning();
		void Back();
		void Forward();
		void Home();
		void HistorySize(int lenght);
		void ToAscii(char * st);
		bool SetFromAscii(const char * st);
		void SetCurrentAction();

	public:
		TrackMode *	current_mode;
		Camera		camera;
		Similarityf last_track;
		Point3f		last_point;

	protected:
		void setDefaultMapping ();
		int			current_button;
		TrackMode*	inactive_mode;
		Similarityf undo_track; 
		Similarityf last_view;
		bool	dragging;
		int		button_mask;
		Quaternionf spin;
		bool	spinnable;
		bool	spinning;
		std::map<int, TrackMode *> modes;
		std::list<Transform> history;
		int		history_size;
		friend class TrackMode;
	};
}

#endif
