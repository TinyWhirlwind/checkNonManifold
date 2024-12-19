#ifndef _SceneBaseObject_
#define _SceneBaseObject_

//#include <QString>
//#include <QVector>
//#include <QSet>
#include <set>
#include <vector>
#include <initializer_list>
#include <boost\weak_ptr.hpp>
#include <boost\shared_ptr.hpp>
#include <boost\enable_shared_from_this.hpp>

#include "Common\point3.h"
#include "Common\quaternion.h"
#include "Common\matrix44.h"

#define USE_MESSAGE_BOX
//#define USE_DEBUG_OUTPUT

#define SCENEBASEOBJECT_COMPAREFLOAT(v1, v2, acc, popup) sn3DCore::SceneBaseObject::compareFloat(v1, v2, acc,std::string( __FILE__) + std::string(" ") + std::string(__FUNCTION__) + std::string(" ") + std::to_string(__LINE__) ,popup);
#define SCENEBASEOBJECT_COMPAREPOINT(v1, v2, acc, popup) sn3DCore::SceneBaseObject::comparePoint3f(v1, v2, acc,std::string( __FILE__) + std::string(" ") + std::string(__FUNCTION__) + std::string(" ") + std::to_string(__LINE__) ,popup);
#define SCENEBASEOBJECT_COMPAREMATRIX(v1, v2, acc, popup) sn3DCore::SceneBaseObject::compareMatrix44f(v1, v2, acc,std::string( __FILE__) + std::string(" ") + std::string(__FUNCTION__) + std::string(" ") + std::to_string(__LINE__) ,popup);


namespace sn3DCore
{
	class SceneBaseObject : public boost::enable_shared_from_this<SceneBaseObject>
	{
	public:
		SceneBaseObject();
		SceneBaseObject(
			std::string sName,
			Point3f ptLocalPosition = Point3f(0.0f, 0.0f, 0.0f),
			Point3f ptLocalNormal = Point3f(0.0f, 0.0f, 1.0f),
			Point3f ptLocalHeading = Point3f(0.0f, 1.0f, 0.0f),
			Point3f ptLocalScale = Point3f(1.0f, 1.0f, 1.0f),
			std::set<std::string> sTags = std::set<std::string>());
		virtual ~SceneBaseObject();

	public:
		enum Attach_Transform_Type
		{
			// 维持世界坐标系下的变换
			KEEP_WORLD_TRANSFORM = 0,
			// 维持局部坐标系下的变换
			KEEP_LOCAL_TRANSFORM,
			// 重置变换
			RESET_ALL_TRANSFORM
		};

		private:
		// 唯一标识UUID
		std::string m_sUUID;
		// 对象名称
		std::string m_sObjectName;
		// 父对象
		boost::weak_ptr<SceneBaseObject> m_pParent;
		// 子对象列表
		std::vector<boost::shared_ptr<SceneBaseObject>> m_vChildren;

		// 位置
		Point3f m_ptPosition;
		// 缩放
		Point3f m_ptScale;
		// 旋转 - normal
		Point3f m_ptNormal;
		// 旋转 - heading
		Point3f m_ptHeading;
		// 旋转 - 四元数
		//sn3DCore::Quaternionf m_qRotate;
		// 变换矩阵 - 旋转
		Matrix44f m_mtRotateMatrix;
		// 变换矩阵
		Matrix44f m_mtMatrix;
		// 标签
		std::set<std::string> m_Tags;

	private:
		//======模型信息
		Point3f m_ptInitObjNormal = Point3f(0, 0, 1);	//Z向 (0,0,1)
		Point3f m_ptInitObjHeading = Point3f(0, 1, 0);	//Y向 (0,1,0)

	public:
		Point3f getInitNormal() { return m_ptInitObjNormal; }
		Point3f getInitHeading() { return m_ptInitObjHeading; }

	public:
		// 获得唯一标识符UUID 
		inline const std::string getUUID() const
		{
			return m_sUUID;
		}

		// 设置对象名称
		inline void setSceneObjectName(const std::string sName)
		{
			m_sObjectName = sName;
		}

		// 获得对象名称
		inline const std::string getSceneObjectName(void) const
		{
			return m_sObjectName;
		}

		// 查找标签
		inline bool findTag(std::string sTag)
		{
			return m_Tags.end() != m_Tags.find(sTag);
		}

		// 查找标签
		inline bool compareTags(std::set<std::string> sTags)
		{
			std::set<std::string>::iterator tagItor = sTags.begin();
			while (tagItor != sTags.end())
			{
				if (!findTag(*tagItor))
				{
					return false;
				}
				tagItor++;
			}
			return true;
		}

		// 获得标签
		inline std::set<std::string> getTags()
		{
			return m_Tags;
		}

		// 添加标签
		inline void addTag(std::string sTag)
		{
			m_Tags.insert(sTag);
		}

		// 添加标签
		inline void addTags(std::set<std::string> sTags)
		{
			std::set<std::string>::iterator tagItor = sTags.begin();
			while (tagItor != sTags.end())
			{
				addTag(*tagItor);
				tagItor++;
			}
		}

		// 清空标签
		inline void clearTags()
		{
			m_Tags.clear();
		}

		//删除标签
		inline void removeTag(std::string sTag)
		{
			std::set<std::string>::iterator tagItor = m_Tags.find(sTag);
			if (tagItor != m_Tags.end())
			{
				m_Tags.erase(tagItor);
			}
		}

		inline void removeTags(std::set<std::string> sTags)
		{
			std::set<std::string>::iterator tagItor = sTags.begin();
			while (tagItor != sTags.end())
			{
				removeTag(*tagItor);
				tagItor++;
			}
		}

		// 获得父对象
		inline boost::shared_ptr<SceneBaseObject> getParent(void)
		{
			return m_pParent.expired() ? nullptr : m_pParent.lock();
		}

		// 判断是否是父对象
		inline bool isMyParent(const SceneBaseObject* pObject)
		{
			if (nullptr != pObject || m_pParent.expired())
			{
				return false;
			}
			else
			{
				return (pObject->getUUID() == m_pParent.lock()->getUUID());
			}
		}

		inline bool isMyParent(const boost::weak_ptr<SceneBaseObject> pObject)
		{
			if (pObject.expired() || m_pParent.expired())
			{
				return false;
			}
			else
			{
				return pObject.lock() == m_pParent.lock();
			}
		}

		inline bool isMyParent(const boost::shared_ptr<SceneBaseObject> pObject)
		{
			if (nullptr == pObject || m_pParent.expired())
			{
				return false;
			}
			else
			{
				return pObject == m_pParent.lock();
			}
		}

		inline boost::shared_ptr<SceneBaseObject> getRoot()
		{
			return m_pParent.expired() ? shared_from_this() : m_pParent.lock()->getRoot();
		}

		// 获得子对象数量
		int getChildCount(bool bDeepSearch = true);

		// 查找子对象是否存在,若存在返回序号,否则返回-1
		int getChildIndex(const SceneBaseObject* pObject);
		//int getChildIndex(const boost::weak_ptr<SceneBaseObject> pObject);
		int getChildIndex(const boost::shared_ptr<SceneBaseObject> pObject);

		// 在所有子对象中查找
		bool isChild(const SceneBaseObject* pObject, bool bDeepSearch = true);
		//bool isChild(const boost::weak_ptr<SceneBaseObject> pObject, bool bDeepSearch = true);
		bool isChild(const boost::shared_ptr<SceneBaseObject> pObject, bool bDeepSearch = true);

		// 移除子对象
		bool removeChild(const int nIndex);
		//bool removeChild(const boost::weak_ptr<SceneBaseObject> pObject);
		bool removeChild(const boost::shared_ptr<SceneBaseObject> pObject);

		// 获得对象
		boost::shared_ptr<SceneBaseObject> getChildByUuid(std::string sUUID, bool bDeepSearch = true);
		// 获得第一个同名对象
		boost::shared_ptr<SceneBaseObject> getChildByName(std::string sName, bool bDeepSearch = true);
		// 获得所有同名对象
		std::vector<boost::shared_ptr<SceneBaseObject>> getAllChildrenByName(std::string sName, bool bDeepSearch = true);
		// 获得所有标签一致的对象
		std::vector<boost::shared_ptr<SceneBaseObject>> getAllChildrenByTag(std::string sTag, bool bDeepSearch = true);
		// 获得所有标签一致的对象
		std::vector<boost::shared_ptr<SceneBaseObject>> getAllChildrenByTags(std::set<std::string> sTags, bool bDeepSearch = true);

		// 从父对象上脱离
		void removeFromParent();

		// 移除所有子对象
		void removeAllChildren();

	public:
		// 将对象附着到另一个对象下
		//bool attachToObject(boost::weak_ptr<SceneBaseObject> pObject, Attach_Transform_Type eType = KEEP_LOCAL_TRANSFORM);
		bool attachToObject(boost::shared_ptr<SceneBaseObject> pObject, Attach_Transform_Type eType = KEEP_LOCAL_TRANSFORM);

		// 重置坐标位置
		void initTransform();

		// 获得/设置世界坐标系下的位置
		virtual Point3f getWorldPosition();
		virtual void setWorldPosition(Point3f ptPos);
		// 获得/设置世界坐标系下的normal
		virtual Point3f getWorldNormal();
		// 获得/设置世界坐标系下的heading
		virtual Point3f getWorldHeading();
		virtual void setWorldRotate(Point3f ptNormal, Point3f ptHeading);
		// 获得/设置世界坐标系下的旋转 - 四元数
		virtual Quaternionf getWorldRotateQuaternionf();
		virtual Matrix44f getWorldRotateMatrix44f();
		// 获得世界坐标系下的变换矩阵
		virtual Matrix44f getWorldMatrix44f();

		// 获得局部坐标系下的位置
		virtual inline Point3f getLocalPosition()
		{
			return m_ptPosition;
		}
		// 设置局部坐标系下的位置
		virtual inline void setLocalPosition(Point3f ptPos, bool bUpdateMatrix = true)
		{
			m_ptPosition = ptPos;
			if (bUpdateMatrix)
			{
				updateALLMatrix44f();
			}
		}
		virtual inline void setLocalPosition(float fX, float fY, float fZ, bool bUpdateMatrix = true)
		{
			setLocalPosition(Point3f(fX, fY, fZ), bUpdateMatrix);
		}

		// 获得局部坐标系下的缩放
		virtual inline Point3f getLocalScale()
		{
			return m_ptScale;
		}

		// 设置局部坐标系下的缩放
		virtual inline void setLocalScale(Point3f ptScale, bool bUpdateMatrix = true)
		{
			m_ptScale = ptScale;
			if (bUpdateMatrix)
			{
				updateALLMatrix44f();
			}
		}

		virtual inline void setLocalScale(float fX, float fY, float fZ)
		{
			setLocalScale(Point3f(fX, fY, fZ));
		}

		virtual inline void setLocalScale(float fScale)
		{
			setLocalScale(Point3f(fScale, fScale, fScale));
		}

		// 获得局部坐标系下的normal
		virtual inline Point3f getLocalNormal()
		{
			return m_ptNormal;
		}

		// 获得局部坐标系下的heading
		virtual inline Point3f getLocalHeading()
		{
			return m_ptHeading;
		}

		// 设置局部坐标系下的旋转
		virtual void setLocalRotate(Point3f ptNormal, Point3f ptHeading, bool bUpdate = true);
		inline void setLocalNormal(Point3f ptNormal, bool bUpdate = true)
		{
			setLocalRotate(ptNormal, getLocalHeading(), bUpdate);
		}
		virtual inline void setLocalHeading(Point3f ptHeading, bool bUpdate = true)
		{
			setLocalRotate(getLocalNormal(), ptHeading, bUpdate);
		}

		virtual inline void initLocalRotate()
		{
			setLocalRotate(Point3f(0.0f, 0.0f, 1.0f), Point3f(0.0f, 1.0f, 0.0f));
		}

		virtual void setLocalPositionAndRotate(Point3f ptRotCenter, Quaternionf qRot, Point3f ptTrans);

		// 获得局部坐标系下的旋转 - 四元数
		virtual inline Quaternionf getLocalRotateQuaternionf()
		{
			Quaternionf q;
			//q.FromMatrix(getLocalRotateMatrix());
			Matrix44f _Mat = getLocalRotateMatrix44f();
			q.FromMatrix(_Mat);
			return q;
		}

		virtual inline Matrix44f getLocalScaleMatrix44f()
		{
			Matrix44f S; S.SetScale(getLocalScale());
			return S;
		}

		// 获得局部坐标系下的旋转 - 旋转矩阵
		virtual inline Matrix44f getLocalRotateMatrix44f()
		{
			return m_mtRotateMatrix;
		}
		// 获得局部坐标系下的旋转 - 平移矩阵
		virtual inline Matrix44f getLocalTranformMatrix44f()
		{
			Matrix44f T;
			T.SetTranslate(getLocalPosition());
			return T;
		}

		virtual inline void setLocalRotateMatrix44f(Matrix44f rotMat, bool bUpdate = true);


		// 获得局部坐标系下的变换矩阵
		virtual inline Matrix44f getLocalMatrix44f()
		{
			return m_mtMatrix;
		}


		virtual inline void setLocalMatrix44f(Matrix44f mat, bool bUpdate = true);


		// 更新局部矩阵
		virtual void updateLocalMatrix44f();
		// 更新所有矩阵
		virtual void updateALLMatrix44f();

		// 获得自身父亲全局变换矩阵
		virtual Matrix44f getParentWorldMatrix44f();
		// 获得自身父亲局部变换矩阵
		virtual Matrix44f getParentLocalMatrix44f();
		// 获得自身父亲的旋转矩阵
		virtual Matrix44f getParentWorldRotateMatrix44f();
		// 获得自身父亲的旋转矩阵
		virtual Matrix44f getParentLocalRotateMatrix44f();

	public:
		// 从世界坐标系下的坐标转换到局部
		virtual Point3f getlocalPointFromWorld(Point3f ptPoint);

	private:
		// 模型中心点位置
		Point3f m_ptModelPosition;
		// 模型normal
		Point3f m_ptModelNormal;
		// 模型heading
		Point3f m_ptModelHeading;
		// 模型变换矩阵 - 旋转
		Matrix44f m_mtModelRotateMatrix;
		// 模型局部变换矩阵
		Matrix44f m_mtModelMatrix;

	public:
		// 获得模型世界坐标系下的变换矩阵
		virtual Matrix44f getModelWorldMatrix44f();

		// 获得模型世界坐标系下的位置
		virtual Point3f getModelWorldPosition();

		// 获得模型世界坐标系下的旋转矩阵
		virtual Matrix44f getModelWorldRotateMatrix44f();

		// 获得模型世界坐标系下的normal
		virtual Point3f getModelWorldNormal();

		// 获得模型世界坐标系下的heading
		virtual Point3f getModelWorldHeading();

		// 获得模型局部坐标系下的变换矩阵
		virtual inline Matrix44f getModelLocalMatrix44f()
		{
			return m_mtModelMatrix;
		}



		// 获得模型局部坐标系下的位置
		virtual Point3f getModelLocalPosition();

		// 获得模型局部坐标系下的normal 未验证正确性
		virtual Point3f getModelLocalNormal();

		// 获得模型局部坐标系下的heading 未验证正确性
		virtual Point3f getModelLocalHeading();

		virtual inline Point3f getModelPosition()
		{
			return m_ptModelPosition;
		}

		// 设置模型坐标系下的位置(不建议使用Model这个方法)
		virtual inline void setModelPosition(Point3f ptPos, bool bUpdate = true)
		{
			m_ptModelPosition = ptPos;
			if (bUpdate)
			{
				updateModelMatrix44f();
			}
		}
		virtual inline void setModelPosition(float fX, float fY, float fZ, bool bUpdate = true)
		{
			setModelPosition(Point3f(fX, fY, fZ), bUpdate);
		}

		// 获得模型坐标系下的normal
		virtual inline Point3f getModelNormal()
		{
			return m_ptModelNormal;
		}

		// 获得模型坐标系下的heading
		virtual inline Point3f getModelHeading()
		{
			return m_ptModelHeading;
		}

		// 获得模型坐标系下的旋转矩阵
		virtual inline Matrix44f getModelRotateMatrix44f()
		{
			return m_mtModelRotateMatrix;
		}

		// 设置局部坐标系下的旋转
		virtual void setModelRotate(Point3f ptNormal, Point3f ptHeading, bool bUpdate = true);
		inline void setModelNormal(Point3f ptNormal, bool bUpdate = true)
		{
			setModelRotate(ptNormal, getModelHeading(), bUpdate);
		}

		virtual inline void setModelHeading(Point3f ptHeading, bool bUpdate = true)
		{
			setModelRotate(getModelNormal(), ptHeading, bUpdate);
		}

		// 更新模型变换矩阵
		virtual void updateModelMatrix44f();

	private:
		// 设置父对象
		void setParent(const boost::shared_ptr<SceneBaseObject> pParent);
		// 添加子对象
		void addChild(const boost::shared_ptr<SceneBaseObject> pObject);
		void addChildren(const std::vector<boost::shared_ptr<SceneBaseObject>> pObjects);

	public:
		// 获得映射到某个坐标系下的矩阵
		// 如果当前对象与映射对象不在同一颗树中 则返回false
		bool getRotateMatrix44fUnderSceneObj(const boost::shared_ptr<SceneBaseObject> SceneObj, Matrix44f &resMat44f);
		bool getMatrix44fUnderSceneObj(const boost::shared_ptr<SceneBaseObject> SceneObj, Matrix44f& resMat44f);
		bool getModelRotateMatrix44fUnderSceneObj(const boost::shared_ptr<SceneBaseObject> SceneObj, Matrix44f& resMat44f);
		bool getModelMatrix44fUnderSceneObj(const boost::shared_ptr<SceneBaseObject> SceneObj, Matrix44f& resMat44f);
	public:
		static bool compareFloat(float fValue1, float fValue2, float fAccuracy = 0.001f, std::string errorStr = "", bool bPopUp = false);
		static bool comparePoint3f(Point3f fPoint1, Point3f fPoint2, float fAccuracy = 0.001f, std::string errorStr = "", bool bPopUp = false);
		static bool compareMatrix44f(Matrix44f fMatrix1, Matrix44f fMatrix2, float fAccuracy = 0.001f, std::string errorStr = "", bool bPopUp = false);
		static bool getCommonParent(const boost::shared_ptr<SceneBaseObject> SceneObj1, const boost::shared_ptr<SceneBaseObject> SceneObj2, boost::shared_ptr<SceneBaseObject>& parent);
		
};
}

#endif