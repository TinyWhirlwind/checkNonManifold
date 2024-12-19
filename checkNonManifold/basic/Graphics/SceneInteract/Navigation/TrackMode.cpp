#include "trackmode.h"

#include "GL/glew.h"
#include "trackball.h"
#include "trackutils.h"
#include "SceneParam.h"

using namespace std;
namespace sn3DCore
{
    void TrackMode::Apply (Trackball * , float ){}
    void TrackMode::Apply (Trackball * , Point3f ){}
    void TrackMode::Draw(Trackball * ){}
    void TrackMode::SetAction (){}
    void TrackMode::Reset (){}
    bool TrackMode::isSticky()
    {
        return false;
    }
    void TrackMode::Undo(){}

    /*绘制不活动trackball*/
    void InactiveMode::Draw(Trackball * tb)
	{
        DrawSphereIcon(tb,false);
    }

    // 旋转操作实现
    void SphereMode::Apply (Trackball * tb, Point3f new_point)
    {
        Point3f hitOld = HitSphere (tb, tb->last_point);
        Point3f hitNew = HitSphere (tb, new_point);

        if(hitNew != hitOld)
        {
            Point3f axis;
            Point3f center = tb->center;
            axis = (hitNew - center) ^ (hitOld - center);
            axis.Normalize();
            float phi = Distance (hitNew, hitOld) / (tb->radius);
            tb->track.rot = Quaternionf (-phi, axis) *tb->last_track.rot;
        }
    }

    void SphereMode::SetDirection(Trackball * tb,Point3f dir)
    {
        Point3f dir1;
        dir1.X() = 0;
        dir1.Y() = 0;
        dir1.Z() = 1;
        Point3f axis = dir ^ dir1;
        float phi = Distance(dir,dir1);
        tb->track.rot = tb->last_track.rot = Quaternionf (phi, axis);

    }

    void SphereMode::Draw(Trackball * tb)
    {
        DrawSphereIcon(tb,true );
    }

    /*平移操作实现*/
    void PanMode::Apply (Trackball * tb, Point3f new_point)
    {
        Point3f hitOld = HitViewPlane (tb, tb->last_point);
        Point3f hitNew = HitViewPlane (tb, new_point);
        Point3f p;
        p= tb->last_point;
        p.Z()=0;
        hitOld = tb->camera.UnProject(p);
        p=new_point;
        p.Z() = 0;
        hitNew = tb->camera.UnProject(p);
        tb->Translate (hitNew - hitOld);
    }

    void PanMode::Draw(Trackball * tb)
    {

    }

    /*缩放操作实现*/
    void ScaleMode::Apply (Trackball * tb, float WheelNotch)
    {
        tb->track.sca *= pow (1.2f, -WheelNotch);

        float fScaleMin = SceneParam::Inst().ScaleMin;
        float fScaleMax = SceneParam::Inst().ScaleMax;

        // 添加缩放限制
        if (tb->track.sca < fScaleMin)
            tb->track.sca = fScaleMin;
        if (tb->track.sca > fScaleMax)
            tb->track.sca = fScaleMax;
    }

    void ScaleMode::Apply (Trackball * tb, Point3f new_point)
    {
        tb->track.sca = tb->last_track.sca * pow (3.0f, -(getDeltaY(tb,new_point)));

        float fScaleMin = SceneParam::Inst().ScaleMin;
        float fScaleMax = SceneParam::Inst().ScaleMax;
        // 添加缩放限制
        if (tb->track.sca < fScaleMin)
            tb->track.sca = fScaleMin;
        if (tb->track.sca > fScaleMax)
            tb->track.sca = fScaleMax;
    }

    void ScaleMode::Draw(Trackball * tb)
	{
        DrawSphereIcon(tb,true );
        DrawUglyScaleMode(tb);
    }
}
