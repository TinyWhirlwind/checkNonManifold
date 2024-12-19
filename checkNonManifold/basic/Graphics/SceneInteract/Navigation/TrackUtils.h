#ifndef TRACKUTILS_H
#define TRACKUTILS_H

#include <assert.h>
#include <vector>
#include <cmath>

#include "Common/base.h"
#include "Common/similarity.h"
#include "Common/intersection3.h"
#include "Common/line3.h"
#include "Common/plane3.h"
#include "Common/math.h"
#include "Common/space1.h"

using namespace std;
namespace sn3DCore
{
    Plane3f GetViewPlane (const Camera &camera, const Point3f & center)
    {
        Point3f vp = camera.ViewPoint ();
        Plane3f pl;
        Point3f plnorm = vp - center;
        plnorm.Normalize ();
        pl.Set (plnorm, plnorm * center);
        return pl;
    }

    Ray3f line2ray(const Line3f &l)
	{
        Ray3f r(l.Origin(),l.Direction());
        r.Normalize();
        return r;
    }

    Point3f HitViewPlane (Trackball * tb, const Point3f & p)
    {
        Plane3f vp = GetViewPlane (tb->camera, tb->center);
        Line3fN ln = tb->camera.ViewLineFromWindow (Point3f (p[0], p[1], 0));
        Point3f PonVP;
        IntersectionLinePlane < float >(vp, ln, PonVP);
        return PonVP;
    }

    /*将窗口坐标投影到旋转双曲面上
        dato un punto in coordinate di schermo e.g. in pixel stile opengl
        calcola il punto di intersezione tra la viewline  che passa per
        viewpoint e per hitplane e l'iperboloide.
        l'iperboloide si assume essere quello di rotazione attorno alla
        retta viewpoint-center e di raggio rad
        si assume come sistema di riferimento quello con l'origine
        su center ecome x la retta center-viewpoint

        eq linea
        hitplane.y
        y = - ----------- * x + hitplane.y
        viewpoint.x

        eq hiperboloide di raggio r (e.g. che passa per (r/sqrt2,r/sqrt2)

        1
        y = --- * (r^2 /2.0)
        x

        hitplane.y
        ----------- * x^2 - hitplane.y *x + (r^2/2.0) == 0
        viewpoint.x
        </pre>

        @param center the center of the manipulator.
        @param radius the radius of the manipulator.
        @param viewpoint the view point.
        @param vp the view plane.
        @param hitplane the projection of the window coordinate point on the view plane.
        @param hit the projection of hitplane on the rotational hyperboloid relative to the manipulator.
        @return true if and only if hit is valid.
        */

    bool HitHyper (Point3f center, float radius, Point3f viewpoint, Plane3f vp,
                   Point3f hitplane, Point3f & hit)
    {
        float hitplaney = Distance (center, hitplane);
        float viewpointxx = Distance (center, viewpoint);

        float a = hitplaney / viewpointxx;
        float b = -hitplaney;
        float c = radius * radius / 2.0f;
        float delta = b * b - 4 * a * c;
        float x1, x2, xval, yval;

        if (delta > 0)
		{
            x1 = (-b - sqrt (delta)) / (2.0f * a);
            x2 = (-b + sqrt (delta)) / (2.0f * a);

            xval = x1;                  // always take the minimum value solution
            yval = c / xval;            //  alternatively it also could be the other part of the equation yval=-(hitplaney/viewpointx)*xval+hitplaney;
        }
        else 
		{
            return false;
        }
        // Computing the result in 3d space;
        Point3f dirRadial = hitplane - center;
        dirRadial.Normalize ();
        Point3f dirView = vp.Direction ();
        dirView.Normalize ();
        hit = center + dirRadial * yval + dirView * xval;
        return true;
    }

    /*将窗口坐标投影到该操作器的球面上*/
    Point3f HitSphere (Trackball * tb, const Point3f & p)
    {
        //Line3fN vn = tb->camera.ViewLineFromModel (tb->center);
        Line3fN ln = tb->camera.ViewLineFromWindow (Point3f (p[0], p[1], 0));
        Point3f viewpoint = tb->camera.ViewPoint ();
        Plane3f vp = GetViewPlane (tb->camera, tb->center);
        Point3f hit, hitPlane, hitSphere, hitSphere1, hitSphere2, hitHyper;
        Point3f nearPoint,farPoint;
        nearPoint.X() = p[0];nearPoint.Y() = p[1]; nearPoint.Z() =0;
        farPoint.X() = p[0]; farPoint.Y() = p[1]; farPoint.Z() =1;
        nearPoint=tb->camera.UnProject(nearPoint);
        farPoint = tb->camera.UnProject(farPoint);
        ln.SetOrigin(nearPoint);
        ln.SetDirection(farPoint - nearPoint);
        IntersectionLinePlane < float >(vp, ln, hitPlane);

        Sphere3f sphere (tb->center, /*1.0*/tb->radius);
        bool resSp = IntersectionLineSphere < float >(sphere, ln, hitSphere1, hitSphere2);

        if (resSp == true) 
		{
            if (Distance (viewpoint, hitSphere1) < Distance (viewpoint, hitSphere2))
                hitSphere = hitSphere1;
            else
                hitSphere = hitSphere2;
        }

        /*float dl= */ Distance (ln, tb->center);
        bool resHp = HitHyper (tb->center, 1.0/*tb->radius*/, viewpoint, vp, hitPlane, hitHyper);

        // four cases

        // 1) Degenerate line tangent to both sphere and hyperboloid!
        if ((!resSp && !resHp)) 
		{
            hit = ClosestPoint (ln, tb->center);

            //printf("closest point to line %f\n",Distance(hit,tb->center));
            return hit;
        }
        if ((resSp && !resHp))
        {
            return hitSphere;           // 2) line cross only the sphere
        }
        if ((!resSp && resHp))
        {
            return hitHyper;            // 3) line cross only the hyperboloid

        }
        // 4) line cross both sphere and hyperboloid: choose according angle.
        float angleDeg = AXYMath::ToDeg (Angle ((viewpoint - tb->center), (hitSphere - tb->center)));


        if (angleDeg < 45)
            return hitSphere;
        else
            return hitHyper;
    }

    /*!
        @brief Calculates the minimal distance between 2 lines.

        P and Q are the lines, P_s and Q_t are set to be the closest points on these lines.

        it's returned the distance from P_s and Q_t, and a boolean value which is true
        if the lines are parallel enough.

        if P and Q are parallel P_s and Q_t aren't set.

        the formula is taken from pages 81-83 of
        <em>"Eric Lengyel - Mathematics for 3D Game Programming & Computer Graphics"</em>
        @param P the first line.
        @param Q the second line.
        @param P_s the point on P closest to Q.
        @param Q_t the point on Q closest to P.
        @return a  pair made with the distance from P_s to Q_t and a boolean value, true if and only if P and Q are almost parallel.
        */
    pair< float, bool > LineLineDistance(const Line3f & P,const Line3f & Q,Point3f & P_s, Point3f & Q_t)
	{
        Point3f p0 = P.Origin (), Vp = P.Direction ();
        Point3f q0 = Q.Origin (), Vq = Q.Direction ();
        float VPVP = Vp * Vp;
        float VQVQ = Vq * Vq;
        float VPVQ = Vp * Vq;
        const float det = ( VPVP * VQVQ ) - ( VPVQ * VPVQ );
        const float EPSILON = 0.00001f;
        if ( fabs(det) < EPSILON ) 
		{
            return  make_pair(Distance(P,q0), true);
        }
        float b1= (q0 - p0) * Vp;
        float b2= (p0 - q0) * Vq;
        float s = ( (VQVQ * b1) + (VPVQ * b2) ) / det;
        float t = ( (VPVQ * b1) + (VPVP * b2) ) / det;
        P_s = p0 + (Vp * s);
        Q_t = q0 + (Vq * t);
        return  make_pair(Distance(P_s,Q_t),false);
    }

    /*!
        @brief Calculates the minimal distance between a ray and a line.

        R is the ray and Q is the line, R_s and Q_t are set to be the closest points on
        the ray and the line.

        it's returned the distance from R_s and Q_t, and a boolean value which is true
        if the ray and the line are parallel enough.

        if R and Q are parallel R_s and Q_t aren't set.
        @param R the ray.
        @param Q the line.
        @param R_s the point on R closest to Q.
        @param Q_t the point on Q closest to R.
        @return a  pair made with the distance from R_s to Q_t and a boolean value, true if and only if P and Q are almost parallel.
        */
    pair< float, bool > RayLineDistance(const Ray3f & R,const Line3f & Q,Point3f & R_s, Point3f & Q_t)
	{
        Point3f r0 = R.Origin (), Vr = R.Direction ();
        Point3f q0 = Q.Origin (), Vq = Q.Direction ();
        float VRVR = Vr * Vr;
        float VQVQ = Vq * Vq;
        float VRVQ = Vr * Vq;
        const float det = ( VRVR * VQVQ ) - ( VRVQ * VRVQ );
        const float EPSILON = 0.00001f;
        if ( ( det >= 0.0f ? det : -det) < EPSILON ) 
		{
            return  make_pair(Distance(Q,r0), true);
        }
        float b1= (q0 - r0) * Vr;
        float b2= (r0 - q0) * Vq;
        float s = ( (VQVQ * b1) + (VRVQ * b2) ) / det;
        float t = ( (VRVQ * b1) + (VRVR * b2) ) / det;
        if(s<0)
		{
            R_s = r0;
            Q_t = ClosestPoint(Q,R_s);
        }
		else 
		{
            R_s = r0 + (Vr * s);
            Q_t = q0 + (Vq * t);
        }
        return  make_pair(Distance(R_s,Q_t),false);
    }

    /*!
        @brief Calculates the minimal distance between 2 segments.

        R e Q are the segments, R_s and Q_t are set to be the closest points on
        the segments.

        it's returned the distance from R_s and Q_t, and a boolean value which is true
        if the segments are parallel enough.
        @param R the first segment.
        @param Q the second segment.
        @param R_s the point on R closest to Q.
        @param Q_t the point on Q closest to R.
        @return a  pair made with the distance from R_s to Q_t and a boolean value, true if and only if P and Q are almost parallel.
        */
    pair< float, bool > SegmentSegmentDistance(const Segment3f & R, const Segment3f & Q, Point3f & R_s, Point3f & Q_t)
    {
        float R_len=Distance(R.P0(),R.P1());
        float Q_len=Distance(Q.P0(),Q.P1());
        float EPSILON_LENGTH =  fmax(R_len,Q_len)*0.0001f;
        if(R_len < EPSILON_LENGTH)
		{
            R_s=R.P0();
            Q_t=ClosestPoint(Q,R_s);
            return  make_pair(Distance(R_s,Q_t),true);
        }
        if( Q_len < EPSILON_LENGTH)
		{
            Q_t=Q.P0();
            R_s=ClosestPoint(R,Q_t);
            return  make_pair(Distance(R_s,Q_t),true);
        }
        Point3f r0 = R.P0(), Vr = (R.P1()-R.P0()).Normalize();
        Point3f q0 = Q.P0(), Vq = (Q.P1()-Q.P0()).Normalize();
        float VRVR = Vr * Vr;
        float VQVQ = Vq * Vq;
        float VRVQ = Vr * Vq;
        const float det = ( VRVR * VQVQ ) - ( VRVQ * VRVQ );
        const float EPSILON = 0.00001f;
        if ( ( det >= 0.0f ? det : -det) < EPSILON ) 
		{
            Line3f lR(R.P0(),R.P1());
            float qa=lR.Projection(Q.P0());
            float qb=lR.Projection(Q.P1());
            if( (qa<=0.0f) && qb<=(0.0f)){
                R_s=R.P0();
                Q_t=ClosestPoint(Q,R_s);
            } 
			else if ( (qa >= 1.0f) && (qb >= 1.0f) )
			{
                R_s=R.P1();
                Q_t=ClosestPoint(Q,R_s);
            } 
			else 
			{
                if( (qa >= 0.0f) && (qa <= 1.0f) )
				{
                    Q_t=Q.P0();
                    R_s=ClosestPoint(R,Q_t);
                } 
				else if((qb >= 0.0f) && (qb <= 1.0f) )
				{
                    Q_t=Q.P1();
                    R_s=ClosestPoint(R,Q_t);
                } 
				else 
				{
                    if( ((qa<=0.0f)&&(qb>=1.0f) ||((qb<=0.0f)&&(qa>=1.0f))))
					{
                        R_s=R.P0();
                        Q_t=ClosestPoint(Q,R_s);
                    }
					else
					{
                        assert(0);
                    }
                }
            }
            return  make_pair(Distance(R_s,Q_t),true);
        }
        float b1= (q0 - r0) * Vr;
        float b2= (r0 - q0) * Vq;
        float s = ( (VQVQ * b1) + (VRVQ * b2) ) / det;
        float t = ( (VRVQ * b1) + (VRVR * b2) ) / det;
        if( s < 0 )
		{
            R_s = R.P0();
        }
		else if ( s > R_len )
		{
            R_s = R.P1();
        } 
		else
		{
            R_s = r0 + (Vr * s);
        }
        if( t < 0)
		{
            Q_t = Q.P0();
        }
		else if ( t > Q_len )
		{
            Q_t = Q.P1();
        }
		else
		{
            Q_t = q0 + (Vq * t);
        }
        return  make_pair(Distance(R_s,Q_t),false);
    }

    /*!
        @brief Compute the point on a line closest to the ray projection of a window coordinate point.

        Given a window coordinate point, computes a ray starting from the manipulator
        camera eye and passing through the point's projection on the viewplane, then uses RayLineDistance()
        to get the closest point to ray on a given line.
        @see RayLineDistance(const Ray3f & R,const Line3f & Q,Point3f & R_s, Point3f & Q_t)
        @param tb the manipulator.
        @param axis the axis.
        @param point the window coordinate point.
        @return a  pair made with the point on axis closest to the ray projection of point and a boolean true if and only if the ray doesn't diverges respect to the axis.
        */

    pair< Point3f,bool > HitNearestPointOnAxis (Trackball * tb,Line3f axis, Point3f point)
    {
        Ray3fN ray = line2ray(tb->camera.ViewLineFromWindow (point));
        Point3f axis_p(0,0,0), ray_p(0,0,0);
        pair< float, bool > resp=RayLineDistance(ray,axis,ray_p,axis_p);
        if(resp.second || (ray_p == ray.Origin())){
            return  make_pair(Point3f(0,0,0),false);
        }
        return  make_pair(axis_p,true);
    }

    /*!
        @brief Project a line into a plane.

        Given a line and a plane, returns the line projection on the plane.

        The line returned is \e not normalized.
        @param ln the line.
        @param pl the plane.
        @return the (non normalized) line projected.
        */

    Line3f ProjectLineOnPlane(const Line3f & ln, const Plane3f & pl)
    {
        Point3f l0=ln.Origin();
        Point3f l1=l0+ln.Direction();
        Point3f p1,p2;
        p1=pl.Projection(l0);
        p2=pl.Projection(l1);
        Line3f res(p1,p2-p1);
        return res;
    }

    /*!
        @brief Computes a signed line-point distance.

        Given a line, a point and a positivity direction, computes the signed distance between the line and the point.
        @param line the line.
        @param pt the point.
        @param positive_dir the positivity direction.
        @return the signed distance.
        */
    float signedDistance(Line3f line,Point3f pt,Point3f positive_dir)
    {
        return Distance(line,pt) * ((((pt-ClosestPoint(line,pt)) * positive_dir) >= 0.0f )? 1.0f: -1.0f);
    }

    /*!
        @brief Computes the verical component of an user mouse drag.

        @param tb the manipulator.
        @param new_point the new mouse pointer coordinate.
        @return The verical component of the user mouse drag.
        */

    float getDeltaY(Trackball * tb, Point3f new_point)
    {
        float ScreenHeight = float (tb->camera.viewport[3] - tb->camera.viewport[1]);
        return (new_point[1] - tb->last_point[1]) / ScreenHeight;
    }

    /*!
        @brief Computes the intersection between a ray and a plane.

        @param pl the plane.
        @param ray the ray.
        @param po the intersection point.
        @return true if and only if there is intersection (po is valid).
        */
    template<class T>
    inline bool IntersectionRayPlane( const Plane3<T> & pl, const Ray3<T> & ray, Point3<T> &po){
        const T epsilon = T(1e-8);

        T k = pl.Direction() * ray.Direction(); // Compute 'k' factor
        if( (k > -epsilon) && (k < epsilon))
            return false;
        T r = (pl.Offset() - pl.Direction()*ray.Origin())/k;  // Compute ray distance
        if (r < 0)
            return false;
        po = ray.Origin() + ray.Direction()*r;
        return true;
    }

    /*!
        @brief Project a window coordinate point on a plane.

        Given a window coordinate point, computes a ray starting from the manipulator
        camera eye and passing through the point's projection on the viewplane, then uses IntersectionRayPlane()
        to get the ray intersection with a given plane.

        @see IntersectionRayPlane()
        @param tb the manipulator.
        @param point the window coordinate point.
        @param plane the plane.
        @return a  pair made with p's projection on the.plane and a boolean true if and only if the ray doesn't diverges respect to the plane.
        */

    pair< Point3f, bool > HitPlane (Trackball * tb, Point3f point, Plane3f plane)
    {
        Ray3fN ray = line2ray(tb->camera.ViewLineFromWindow (point));
        Point3f p(0,0,0);
        bool res = IntersectionRayPlane < float >(plane, ray, p);
        return  make_pair(p,res);
    }

    // drawing section
    // nota: non ho scritto io questa classe,
    // quindi la documentazione doxy potrebbe non essere accurata al 100%.
    /*!
        @brief Drawing hints for manipulators

        This class is an holder for drawing-related variables.

        It's mainly used for SphereMode and InactiveMode drawings.
        */

    class DrawingHint 
	{
    public:

        DrawingHint () 
		{
            CircleStep = 64;
            HideStill = false;
            DrawTrack = false;
            LineWidthStill = 0.5f;
            LineWidthMoving = 1.5f;
            color = Color4b::LightBlue;
        }
        /// The circles resolution.
        int CircleStep;
        /// currently not in use.
        bool HideStill;
        /// currently not in use.
        bool DrawTrack;
        /// circle color
        Color4b color;
        /// circle line width when inactive.
        float LineWidthStill;
        /// circle line width when active.
        float LineWidthMoving;
    };

    DrawingHint DH;
    void DrawPlaneHandle ()
    {
        float r = 1.0;
        float dr = r / 10.0f;

        glBegin (GL_LINE_STRIP);
        glVertex3f (+r + dr, +r, 0.0);
        glVertex3f (+r, +r + dr, 0.0);
        glVertex3f (+r - dr, +r, 0.0);
        glVertex3f (+r, +r - dr, 0.0);
        glVertex3f (+r + dr, +r, 0.0);
        glEnd ();
        glBegin (GL_LINE_STRIP);
        glVertex3f (-r + dr, -r, 0.0);
        glVertex3f (-r, -r + dr, 0.0);
        glVertex3f (-r - dr, -r, 0.0);
        glVertex3f (-r, -r - dr, 0.0);
        glVertex3f (-r + dr, -r, 0.0);
        glEnd ();
    }

    void DrawCircle ()
    {
        int nside = DH.CircleStep;
        const double pi2 = 3.14159265 * 2.0;
        glBegin (GL_LINE_LOOP);
        for (double i = 0; i < nside; i++) {
            glNormal3d (cos (i * pi2 / nside), sin (i * pi2 / nside), 0.0);
            glVertex3d (cos (i * pi2 / nside), sin (i * pi2 / nside), 0.0);
        }
        glEnd ();
    }

    void DrawSphereIcon (Trackball * tb,bool active)
    {
        glPushAttrib (GL_TRANSFORM_BIT |GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix ();
        glTranslate (tb->center);
        glMultMatrix (tb->track.InverseMatrix ());
        glScale (tb->radius);
        Matrix44f r;
        tb->track.rot.ToMatrix (r);
        glMultMatrix (r);

        float amb[4] = { .3f, .3f, .3f, 1.0f };
        float col[4] = { .5f, .5f, .8f, 1.0f };
        glEnable (GL_LINE_SMOOTH);
        if (active)
            glLineWidth (DH.LineWidthMoving);
        else
            glLineWidth (DH.LineWidthStill);
        glEnable (GL_LIGHTING);
        glEnable (GL_LIGHT0);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor (DH.color);
        glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, amb);
        glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, col);

        DrawCircle ();
        glRotatef (90, 1, 0, 0);
        DrawCircle ();
        glRotatef (90, 0, 1, 0);
        DrawCircle ();
        glPopMatrix ();
        glPopAttrib ();
    }

    void prepare_attrib()
    {
        float amb[4] = { .3f, .3f, .3f, 1.0f };
        float col[4] = { .5f, .5f, .8f, 1.0f };
        glEnable (GL_LIGHTING);
        glEnable (GL_LIGHT0);
        glEnable (GL_LINE_SMOOTH);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, amb);
        glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, col);
    }

    /*!
        @brief Support function for the \e DrawUgly series of functions.

        Draw a coordinate vector, usually a letter, near the manipulator icon in a user readable oriantation.
        \warning this method is part of the \e DrawUgly series of functions, which is a \b TEMPORARY solution, used while waiting for the \e DrawBeautiful series...
        @param tb the manipulator.
        @param ugly_letter the coordinate vector.
        */

    void DrawUglyLetter(Trackball * tb, vector<Point3f> ugly_letter)
    {
        Point3f center=tb->camera.Project(tb->center);
        float offset=0;
        offset= fmax(offset,Distance(center,tb->camera.Project(tb->center+(Point3f(1,0,0) * tb->radius))));
        offset= fmax(offset,Distance(center,tb->camera.Project(tb->center+(Point3f(0,1,0) * tb->radius))));
        offset= fmax(offset,Distance(center,tb->camera.Project(tb->center+(Point3f(0,0,1) * tb->radius))));
        glPushMatrix();
        glPushAttrib (GL_ALL_ATTRIB_BITS);
        // go to world coords
        glTranslate (tb->center);
        glMultMatrix (tb->track.InverseMatrix ());
        glTranslate (-tb->center);
        prepare_attrib();
        glColor3f(1,1,1);
        glLineWidth(4.0);

        glBegin(GL_LINE_STRIP);
        for(unsigned int i=0;i<ugly_letter.size();i++){
            glVertex(tb->camera.UnProject(center+(ugly_letter[i] * offset * 0.25)
                                          +Point3f(-offset,-offset,0)));
        }
        glEnd();
        glPopAttrib ();
        glPopMatrix();
    }

    /*!
        @brief PanMode drawing function, member of the \e DrawUgly series.

        Draw a PanMode manipulator in an ugly way.
        \warning this method is part of the \e DrawUgly series of functions, which is a \b TEMPORARY solution, used while waiting for the \e DrawBeautiful series...
        @param tb the manipulator.
        */

    void DrawUglyPanMode(Trackball * tb)
    {
        vector<Point3f> ugly_p;
        ugly_p.push_back(Point3f(-1,-1,0));
        ugly_p.push_back(Point3f(-1,1,0));
        ugly_p.push_back(Point3f(1,1,0));
        ugly_p.push_back(Point3f(1,0,0));
        ugly_p.push_back(Point3f(-1,0,0));
        DrawUglyLetter(tb,ugly_p);
    }

    /*!
        @brief ZMode drawing function, member of the \e DrawUgly series.

        Draw a ZMode manipulator in an ugly way.
        \warning this method is part of the \e DrawUgly series of functions, which is a \b TEMPORARY solution, used while waiting for the \e DrawBeautiful series...
        @param tb the manipulator.
        */

    void DrawUglyZMode(Trackball * tb)
    {
        vector<Point3f> ugly_z;
        ugly_z.push_back(Point3f(-1,1,0));
        ugly_z.push_back(Point3f(1,1,0));
        ugly_z.push_back(Point3f(-1,-1,0));
        ugly_z.push_back(Point3f(1,-1,0));
        DrawUglyLetter(tb,ugly_z);
    }

    /*!
        @brief ScaleMode drawing function, member of the \e DrawUgly series.

        Draw a ScaleMode manipulator in an ugly way.
        \warning this method is part of the \e DrawUgly series of functions, which is a \b TEMPORARY solution, used while waiting for the \e DrawBeautiful series...
        @param tb the manipulator.
        */

    void DrawUglyScaleMode(Trackball * tb)
    {
        vector<Point3f> ugly_s;
        ugly_s.push_back(Point3f(1,1,0));
        ugly_s.push_back(Point3f(-1,1,0));
        ugly_s.push_back(Point3f(-1,0,0));
        ugly_s.push_back(Point3f(1,0,0));
        ugly_s.push_back(Point3f(1,-1,0));
        ugly_s.push_back(Point3f(-1,-1,0));
        DrawUglyLetter(tb,ugly_s);
    }
}
#endif
