#include "Trackball.h"

#include <gl/glew.h>
#include "Common/math.h"
#include "SceneParam.h"

#pragma warning(disable:4996)
namespace sn3DCore
{
	Transform::Transform() 
	{
		track.SetIdentity();
		radius=1.0f;
		center=Point3f(0,0,0);
	}
	Trackball::Trackball(): current_button(0), 
		current_mode(NULL), inactive_mode(NULL),
		dragging(false), spinnable(true), spinning(false), 
		history_size(10)
	{
		setDefaultMapping ();
	}

	Trackball::~Trackball() 
	{
		std::map<int, TrackMode*>::iterator it;
		for(it = modes.begin(); it != modes.end(); it++)
		{
			if ((*it).second)
				delete (*it).second;
		}
		if(inactive_mode!=NULL)
			delete inactive_mode;
	}

	void Trackball::setDefaultMapping () 
	{
		inactive_mode = new InactiveMode ();
		modes.clear ();
		modes[0] = NULL;
		modes[BUTTON_LEFT] = new SphereMode ();
		modes[BUTTON_LEFT | KEY_CTRL] = new PanMode ();
		modes[BUTTON_MIDDLE] = new PanMode ();
		modes[BUTTON_LEFT | KEY_SHIFT] = new ScaleMode ();
		modes[WHEEL] = new ScaleMode ();
		SetCurrentAction ();
	}

	void Trackball::SetIdentity() {
		track.SetIdentity();
		Reset();
	}
	void Trackball::SetPosition(const Point3f &c, int /* millisec */) 
	{
		center = c;
	}

	void Trackball::SetRadius(float r)
	{
		radius = r;
	}
	void Trackball::GetView()
	{
		camera.GetView();
	}

	void Trackball::DrawPostApply() 
	{ 
		if(current_mode !=NULL)
		{
			current_mode->Draw(this);
		}
		else
		{
			assert(inactive_mode != NULL);
			inactive_mode->Draw(this);
		}
	}

	void Trackball::Apply () 
	{
		camera.GetView();
		glTranslatef (center[0],center[1],center[2]);
		glMultMatrix (track.Matrix ());
		glTranslatef (-center[0],-center[1],-center[2]);
	}
	void Trackball::ApplyRotate()
	{
		Matrix44<float> r;
		track.rot.ToMatrix(r);
		glTranslatef (center[0],center[1],center[2]);
		glMultMatrix (r);
		glTranslatef (-center[0],-center[1],-center[2]);
	}

	void Trackball::ApplyScale()
	{
		Matrix44<float> s = Matrix44<float>().SetScale(track.sca, track.sca, track.sca);
		glTranslatef (center[0],center[1],center[2]);
		glMultMatrix (s);
		glTranslatef (-center[0],-center[1],-center[2]);

	}
	void Trackball::ApplyTranslate()
	{
		Matrix44<float> t = Matrix44<float>().SetTranslate(track.tra[0], track.tra[1], track.tra[2]);
		glMultMatrix (t);
	}

	void Trackball::Apply(bool ToDraw) 
	{ 
		Apply();
		if(ToDraw)
		{
			DrawPostApply();
		}
	}
	void Trackball::ApplyInverse() 
	{ 
		glTranslatef(center[0],center[1],center[2]);
		glMultMatrix(track.InverseMatrix());
		glTranslatef(-center[0],-center[1],-center[2]);
	}

	void Trackball::Scale(const float s)
	{
		track.sca*=s;

		float fScaleMin = SceneParam::Inst().ScaleMin;
		float fScaleMax = SceneParam::Inst().ScaleMax;

		// 添加缩放限制
		if (track.sca < fScaleMin)
			track.sca = fScaleMin;
		if (track.sca > fScaleMax)
			track.sca = fScaleMax;
	}

	void Trackball::Translate(Point3f tr)
	{
		Matrix44f m;  
		track.rot.ToMatrix(m); 
		track.tra = last_track.tra + Inverse(m)*tr/track.sca;
	}

	void Trackball::ToAscii(char* result)
	{
		float * f = (float*) &track;
		sprintf(result, "trackball(%f,%f,%f,%f,%f,%f,%f,%f,%f)", 
			f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8] );
	}

	bool Trackball::SetFromAscii(const char * st)
	{
		float * f = (float*) &track;
		int res=  sscanf(st, "trackball(%f,%f,%f,%f,%f,%f,%f,%f,%f)", 
			f+0,f+1,f+2,f+3,f+4,f+5,f+6,f+7,f+8 );

		return (res==9);

	}
	void Trackball::Reset()
	{
		track.SetIdentity();
		undo_track = track;
		std::map<int, TrackMode *>::iterator i;
		for(i = modes.begin(); i != modes.end(); i++){
			TrackMode * mode=(*i).second;
			if(mode!=NULL)
				mode->Reset();
		}
		assert(inactive_mode != NULL);
		inactive_mode->Reset();
	}

	//输入事件处理接口
	/*void Trackball::MouseDown(int button) 
	{
		undo_track = track;
		current_button |= button;  
		SetCurrentAction();
		Hits.clear();
	}*/
	void Trackball::MouseDown(int x, int y, int button) 
	{
		undo_track = track;
		current_button |= button;  
		SetCurrentAction();
		last_point = Point3f((float)x, (float)y, 0);
		//Hits.clear();
		//Hits.swap(std::vector<Point3f>());
	}

	void Trackball::MouseMove(int x, int y) 
	{  
		if(current_mode == NULL) return;  
		if(last_point[2] == -1) { //changed mode in the middle of moving
			last_point = Point3f((float)x, (float)y, 0);
			return;
		}
		undo_track = track;
		
		current_mode->Apply(this, Point3f(float(x), float(y), 0));
        
	} 

	void Trackball::MouseUp(int /* x */, int /* y */, int button) 
	{ 
		undo_track = track;
		current_button &= (~button);
		SetCurrentAction();
	} 

	//假设1.0为滚轮的一步
	void Trackball::MouseWheel(float notch) 
	{
		if (current_button & 0x0020)return;
		undo_track = track;
		int buttons = current_button;
		current_button = WHEEL | (buttons&(KEY_SHIFT|KEY_CTRL|KEY_ALT));
		SetCurrentAction();
		if (current_mode == NULL)
		{
			ScaleMode scalemode;
			scalemode.Apply (this, notch);
		} 
		else
		{
			current_mode->Apply(this, notch);
		}
		current_button = buttons;
		SetCurrentAction();
	}

	void Trackball::MouseWheel(float notch, int button)
	{
		if (current_button & 0x0020)return;
		undo_track = track;
		current_button |= button;
		SetCurrentAction();
		if (current_mode == NULL) 
		{
			ScaleMode scalemode;
			scalemode.Apply (this, notch);
		} 
		else 
		{
			current_mode->Apply (this, notch);
		}
		current_button &= (~button);
		SetCurrentAction ();
	}

	void Trackball::ButtonDown(Trackball::Button button) 
	{
		bool old_sticky=false, new_sticky=false;
		assert (modes.count (0));
		if ( ( modes.count (current_button) ) && ( modes[current_button] != NULL ) ) 
		{
			old_sticky = modes[current_button]->isSticky();
		}
		current_button |= button;  
		if ( ( modes.count (current_button) ) && ( modes[current_button] != NULL ) ) 
		{
			new_sticky = modes[current_button]->isSticky();
		}
		if ( old_sticky || new_sticky)
			return;
		SetCurrentAction();
	}

	void Trackball::ButtonUp(Trackball::Button button) 
	{ 
		bool old_sticky=false, new_sticky=false;
		assert ( modes.count (0) );
		if ( ( modes.count (current_button) ) && ( modes[current_button] != NULL ) ) 
		{
			old_sticky = modes[current_button]->isSticky();
		}
		current_button &= (~button);  
		if ( ( modes.count (current_button) ) && ( modes[current_button] != NULL ) ) 
		{
			new_sticky = modes[current_button]->isSticky();
		}
		if ( old_sticky || new_sticky)
			return;
		SetCurrentAction();
	}

	void Trackball::Undo(){
		track = undo_track;
		if(current_mode != NULL)
			current_mode->Undo();
	}
	//自旋接口
	void Trackball::SetSpinnable(bool /* on*/ )	{}
	bool Trackball::IsSpinnable()				{return spinnable;}  
	void Trackball::SetSpinning(Quaternionf &/* spin*/){}
	void Trackball::StopSpinning()				{}
	bool Trackball::IsSpinning()				{ return spinning;}  

	//导航接口
	void Trackball::Back()						{}
	void Trackball::Forward()					{}
	void Trackball::Home()						{}
	void Trackball::HistorySize(int /* length */){}

	void Trackball::SetCurrentAction ()
	{
		assert (modes.count (0));
		if (!modes.count (current_button)) 
		{
			current_mode = NULL;
		} 
		else 
		{
			current_mode = modes[current_button];
			if(current_mode != NULL)
				current_mode->SetAction();
		}
		last_point = Point3f (0, 0, -1);
		last_track = track;
	}
}