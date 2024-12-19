#include "SceneBaseObject.h"
#include "MatrixTools.h"
#include <QUuid> 
#include <QMessageBox>
#include <qDebug>

namespace sn3DCore
{
	SceneBaseObject::SceneBaseObject()
	{
		m_sUUID = QUuid::createUuid().toString().toStdString();
		initTransform();
	}

	SceneBaseObject::SceneBaseObject(std::string sName,
		Point3f ptLocalPosition, Point3f ptLocalNormal, Point3f ptLocalHeading, Point3f ptLocalScale,
		std::set<std::string> sTags)
	{
		m_sUUID = QUuid::createUuid().toString().toStdString();
		initTransform();
		setSceneObjectName(sName);
		setLocalPosition(ptLocalPosition);
		setLocalRotate(ptLocalNormal, ptLocalHeading);
		setLocalScale(ptLocalScale);
		addTags(sTags);
	}

	SceneBaseObject::~SceneBaseObject()
	{
		removeAllChildren();
	}

	int SceneBaseObject::getChildCount(bool bDeepSearch)
	{
		int nChildCount = m_vChildren.size();
		if (bDeepSearch)
		{
			std::vector<boost::shared_ptr<SceneBaseObject>>::iterator childItor = m_vChildren.begin();
			while (childItor != m_vChildren.end())
			{
				nChildCount += (*childItor)->getChildCount(true);
				childItor++;
			}
		}
		return nChildCount;
	}

	// 查找子对象是否存在,若存在返回序号,否则返回-1
	int SceneBaseObject::getChildIndex(const SceneBaseObject *pObject)
	{
		if (pObject == nullptr)
		{
			return -1;
		}
		int nChildrenCount = m_vChildren.size();
		for (int i = 0; i < nChildrenCount; ++i)
		{
			if (nullptr != m_vChildren.at(i))
			{
				if (m_vChildren.at(i).get() == pObject)
				{
					return i;
				}
			}
		}
		return -1;
	}

	// 查找子对象是否存在,若存在返回序号,否则返回-1
	//int SceneBaseObject::getChildIndex(const boost::weak_ptr<SceneBaseObject> pObject)
	//{
	//	if (pObject.expired())
	//	{
	//		return -1;
	//	}
	//	return getChildIndex(pObject.lock().get());
	//}

	// 查找子对象是否存在,若存在返回序号,否则返回-1
	int SceneBaseObject::getChildIndex(const boost::shared_ptr<SceneBaseObject> pObject)
	{
		if (nullptr == pObject)
		{
			return -1;
		}
		return getChildIndex(pObject.get());
	}

	bool SceneBaseObject::isChild(const SceneBaseObject *pObject, bool bDeepSearch)
	{
		if (pObject == nullptr)
		{
			return false;
		}
		int nChildrenCount = m_vChildren.size();
		for (int i = 0; i < nChildrenCount; ++i)
		{
			if (nullptr != m_vChildren.at(i))
			{
				if (m_vChildren.at(i).get() == pObject)
				{
					return true;
				}
				else if (bDeepSearch)
				{
					if (m_vChildren.at(i)->isChild(pObject, bDeepSearch))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	//bool SceneBaseObject::isChild(const boost::weak_ptr<SceneBaseObject> pObject, bool bDeepSearch)
	//{
	//	if (pObject.expired())
	//	{
	//		return -1;
	//	}
	//	return isChild(pObject.lock().get(), bDeepSearch);
	//}

	bool SceneBaseObject::isChild(const boost::shared_ptr<SceneBaseObject> pObject, bool bDeepSearch)
	{
		if (nullptr == pObject)
		{
			return -1;
		}
		return isChild(pObject.get(), bDeepSearch);
	}

	// 移除子对象
	bool SceneBaseObject::removeChild(int nIndex)
	{
		if (nIndex >= 0 && nIndex < m_vChildren.size())
		{
			// 将父对象置空
			m_vChildren.at(nIndex)->setParent(nullptr);
			// 移除
			m_vChildren.erase(m_vChildren.begin() + nIndex);
			return true;
		}
		return false;
	}

	// 移除子对象
	//bool SceneBaseObject::removeChild(const boost::weak_ptr<SceneBaseObject> pObject)
	//{
	//	if (pObject.expired())
	//	{
	//		return false;
	//	}
	//	boost::shared_ptr<SceneBaseObject> pObShared_ptr = pObject.lock();
	//	return removeChild(pObShared_ptr);
	//}

	// 移除子对象
	bool SceneBaseObject::removeChild(const boost::shared_ptr<SceneBaseObject> pObject)
	{
		int nIndex = getChildIndex(pObject);
		return removeChild(nIndex);
	}

	// 获得对象
	boost::shared_ptr<SceneBaseObject> SceneBaseObject::getChildByUuid(std::string sUUID, bool bDeepSearch)
	{
		int nChildrenCount = m_vChildren.size();
		for (int i = 0; i < nChildrenCount; ++i)
		{
			if (nullptr != m_vChildren.at(i))
			{
				if (m_vChildren.at(i)->getUUID() == sUUID)
				{
					return m_vChildren.at(i);
				}
				else if (bDeepSearch)
				{
					boost::shared_ptr<SceneBaseObject> pObject = m_vChildren.at(i)->getChildByUuid(sUUID, bDeepSearch);
					if (nullptr != pObject)
					{
						return pObject;
					}
				}
			}
		}

		return nullptr;
	}

	// 获得第一个同名对象
	boost::shared_ptr<SceneBaseObject> SceneBaseObject::getChildByName(std::string sName, bool bDeepSearch)
	{
		int nChildrenCount = m_vChildren.size();
		for (int i = 0; i < nChildrenCount; ++i)
		{
			if (nullptr != m_vChildren.at(i))
			{
				if (m_vChildren.at(i)->getSceneObjectName() == sName)
				{
					return m_vChildren.at(i);
				}
				else if (bDeepSearch)
				{
					boost::shared_ptr<SceneBaseObject> pObject = m_vChildren.at(i)->getChildByName(sName, bDeepSearch);
					if (nullptr != pObject)
					{
						return pObject;
					}
				}
			}
		}

		return nullptr;
	}

	// 获得所有同名对象
	std::vector<boost::shared_ptr<SceneBaseObject>> SceneBaseObject::getAllChildrenByName(std::string sName, bool bDeepSearch)
	{
		std::vector<boost::shared_ptr<SceneBaseObject>> vObjectList;
		int nChildrenCount = m_vChildren.size();
		for (int i = 0; i < nChildrenCount; ++i)
		{
			if (nullptr != m_vChildren.at(i))
			{
				if (m_vChildren.at(i)->getSceneObjectName() == sName)
				{
					boost::shared_ptr<SceneBaseObject> object = m_vChildren.at(i);
					vObjectList.push_back(object);
				}
				else if (bDeepSearch)
				{
					std::vector<boost::shared_ptr<SceneBaseObject>> addVector = m_vChildren.at(i)->getAllChildrenByName(sName, bDeepSearch);
					vObjectList.insert(vObjectList.end(), addVector.begin(), addVector.end());
				}
			}
		}
		return vObjectList;
	}

	// 获得所有标签一致的对象
	std::vector<boost::shared_ptr<SceneBaseObject>> SceneBaseObject::getAllChildrenByTag(std::string sTag, bool bDeepSearch)
	{
		std::vector<boost::shared_ptr<SceneBaseObject>> vObjectList;
		int nChildrenCount = m_vChildren.size();
		for (int i = 0; i < nChildrenCount; ++i)
		{
			if (nullptr != m_vChildren.at(i))
			{
				if (m_vChildren.at(i)->findTag(sTag))
				{
					boost::shared_ptr<SceneBaseObject> object = m_vChildren.at(i);
					vObjectList.push_back(object);
				}
				else if (bDeepSearch)
				{
					std::vector<boost::shared_ptr<SceneBaseObject>> addVector = m_vChildren.at(i)->getAllChildrenByTag(sTag, bDeepSearch);
					vObjectList.insert(vObjectList.end(), addVector.begin(), addVector.end());
				}
			}
		}
		return vObjectList;
	}

	// 获得所有标签一致的对象
	std::vector<boost::shared_ptr<SceneBaseObject>> SceneBaseObject::getAllChildrenByTags(std::set<std::string> sTags, bool bDeepSearch)
	{
		std::vector<boost::shared_ptr<SceneBaseObject>> vObjectList;
		int nChildrenCount = m_vChildren.size();
		for (int i = 0; i < nChildrenCount; ++i)
		{
			if (nullptr != m_vChildren.at(i))
			{
				if (m_vChildren.at(i)->compareTags(sTags))
				{
					boost::shared_ptr<SceneBaseObject> object = m_vChildren.at(i);
					vObjectList.push_back(object);
				}
				else if (bDeepSearch)
				{
					std::vector<boost::shared_ptr<SceneBaseObject>> addVector = m_vChildren.at(i)->getAllChildrenByTags(sTags, bDeepSearch);
					vObjectList.insert(vObjectList.end(), addVector.begin(), addVector.end());
				}
			}
		}
		return vObjectList;
	}

	void SceneBaseObject::removeFromParent()
	{
		if (!m_pParent.expired())
		{
			m_pParent.lock()->removeChild(shared_from_this());
			setParent(nullptr);
		}
	}

	void SceneBaseObject::removeAllChildren()
	{
		int nChildrenCount = m_vChildren.size();
		for (int i = 0; i < nChildrenCount; ++i)
		{
			if (nullptr != m_vChildren[i])
			{
				m_vChildren[i]->removeFromParent();
			}
			else
			{
				m_vChildren.erase(m_vChildren.begin() + i);
			}
			i--;
			nChildrenCount--;
		}
	}

	//// 将对象附着到另一个对象下
	//bool SceneBaseObject::attachToObject(boost::weak_ptr<SceneBaseObject> pObject, Attach_Transform_Type eType)
	//{
	//	if (pObject.expired())
	//	{
	//		return false;
	//	}
	//	boost::shared_ptr<SceneBaseObject> pObShared_ptr = pObject.lock();
	//	return attachToObject(pObShared_ptr, eType);
	//}

	// 将对象附着到另一个对象下
	bool SceneBaseObject::attachToObject(boost::shared_ptr<SceneBaseObject> pObject, Attach_Transform_Type eType)
	{
		Point3f ptWorldPosition = getWorldPosition();
		Point3f ptWorldNormal = getWorldNormal();
		Point3f ptWorldHeading = getWorldHeading();
		if (nullptr != pObject)
		{
			switch (eType)
			{
			case KEEP_WORLD_TRANSFORM:
			{
				
				break;
			}
			case RESET_ALL_TRANSFORM:
			{
				initTransform();
				break;
			}
			default: // KEEP_LOCAL_TRANSFORM
			{
				break;
			}
			}

			pObject->addChild(shared_from_this());
			if (pObject != getParent())
			{
				if (!m_pParent.expired())
				{
					m_pParent.lock()->removeChild(shared_from_this());
					
				}
				setParent(pObject);
			}


			switch (eType)
			{
			case KEEP_WORLD_TRANSFORM:
			{
				setWorldPosition(ptWorldPosition);
				setWorldRotate(ptWorldNormal, ptWorldHeading);
				break;
			}
			default:
			{
				break;
			}
			}
		}
		return false;
	}

	// 重置坐标位置
	void SceneBaseObject::initTransform()
	{
		setLocalPosition(Point3f(0.0f, 0.0f, 0.0f) , false);
		setLocalScale(Point3f(1.0f, 1.0f, 1.0f), false);
		setLocalRotate(Point3f(0.0f, 0.0f, 1.0f), Point3f(0.0f, 1.0f, 0.0f), false);
		updateLocalMatrix44f();

		setModelPosition(Point3f(0.0f, 0.0f, 0.0f), false);
		setModelRotate(Point3f(0.0f, 0.0f, 1.0f), Point3f(0.0f, 1.0f, 0.0f), false);
		updateModelMatrix44f();
	}

	// 获得世界坐标系下的位置
	Point3f SceneBaseObject::getWorldPosition()
	{
		return getWorldMatrix44f() * Point3f(0, 0, 0);
	}

	// 设置世界坐标系下的位置
	void SceneBaseObject::setWorldPosition(Point3f ptPos)
	{
		setLocalPosition(Inverse(getParentWorldMatrix44f()) * ptPos);
	}

	// 获得世界坐标系下的normal
	Point3f SceneBaseObject::getWorldNormal()
	{
		return (getWorldRotateMatrix44f() * Point3f(0, 0, 1)).Normalize();
	}

	// 获得世界坐标系下的heading
	Point3f SceneBaseObject::getWorldHeading()
	{
		return (getWorldRotateMatrix44f() * Point3f(0, 1, 0)).Normalize();
	}

	// 设置世界坐标系下的旋转
	void SceneBaseObject::setWorldRotate(Point3f ptNormal, Point3f ptHeading)
	{
		//m_mtRotateMatrix = Inverse(getParentWorldRotateMatrix44f()) * CMatrixTools::GetRotateMatrix44f(ptNormal, ptHeading);
		//Point3f _ptNormal = (getLocalRotateMatrix() * (Point3f(0, 0, 1))).Normalize();
		//Point3f _ptHeading = (getLocalRotateMatrix() * (Point3f(0, 1, 0))).Normalize();
		//setLocalRotate(_ptNormal, _ptHeading, false);
		//updateLocalMatrix();
		setLocalRotateMatrix44f(Inverse(getParentWorldRotateMatrix44f()) * CMatrixTools::GetRotateMatrix44f(ptNormal, ptHeading), true);
	}

	// 获得世界坐标系下的旋转 - 四元数
	Quaternionf SceneBaseObject::getWorldRotateQuaternionf()
	{
		Quaternionf q;
		Matrix44f m = getWorldRotateMatrix44f();
		q.FromMatrix(m);
		return q;
	}

	// 获得世界坐标系下的旋转 - 矩阵
	Matrix44f SceneBaseObject::getWorldRotateMatrix44f()
	{
		return getParentWorldRotateMatrix44f() * getLocalRotateMatrix44f();
		//return CMatrixTools::GetRotateMatrix44f(getWorldNormal(), getWorldHeading());
	}

	// 获得世界坐标系下的变换矩阵
	Matrix44f SceneBaseObject::getWorldMatrix44f()
	{
		return getParentWorldMatrix44f() * getLocalMatrix44f();
	}

	// 设置局部坐标系下的旋转
	void SceneBaseObject::setLocalRotate(Point3f ptNormal, Point3f ptHeading, bool bUpdateMatrix)
	{
		m_ptNormal = ptNormal;
		m_ptHeading = ptHeading;
		if (bUpdateMatrix)
		{
			updateLocalMatrix44f();
			updateModelMatrix44f();
		}
	}

	// 设置局部坐标系下的平移与旋转
	void SceneBaseObject::setLocalPositionAndRotate(Point3f ptRotCenter, Quaternionf qRot, Point3f ptTrans)
	{
		Matrix44f matRot;	
		qRot.ToMatrix(matRot);
		Matrix44f matPrev; 
		matPrev.SetTranslate(-ptRotCenter);
		Matrix44f matPost; 
		matPost.SetTranslate(ptRotCenter);
		Matrix44f matTrans;
		matTrans.SetTranslate(ptTrans);
		Matrix44f result;
		result = matTrans * matPost * matRot * matPrev;	//先旋转再平移

		//Point3f _ptNormal = (matRot*(Point3f(0, 0, 1))).Normalize();
		//Point3f _ptHeading = (matRot*(Point3f(0, 1, 0))).Normalize();
		//setLocalRotate(_ptNormal, _ptHeading, false);
		//setLocalPosition(result * Point3f(0.0f, 0.0f, 0.0f), false);
		//setLocalRotateMatrix44f(matRot, false);
		//updateALLMatrix44f();
		setLocalMatrix44f(result);
	}

	inline void SceneBaseObject::setLocalRotateMatrix44f(Matrix44f rotMat, bool bUpdate)
	{
		m_mtRotateMatrix = rotMat;
		if (bUpdate)
		{
			Point3f _ptNormal = (getLocalRotateMatrix44f() * (Point3f(0, 0, 1))).Normalize();
			Point3f _ptHeading = (getLocalRotateMatrix44f() * (Point3f(0, 1, 0))).Normalize();
			Matrix44f T, R, S;
			T.SetTranslate(getLocalPosition());
			R = m_mtRotateMatrix;
			S.SetScale(getLocalScale());
			setLocalRotate(_ptNormal, _ptHeading, false);
			setLocalMatrix44f(T * R * S, false);
			updateModelMatrix44f();
		}
	}

	inline void SceneBaseObject::setLocalMatrix44f(Matrix44f mat, bool bUpdate)
	{
		// 会去除矩阵中缩放值
		m_mtMatrix = mat;

		if (bUpdate)
		{
			Point3f _ptCenter = getLocalMatrix44f() * (Point3f(0, 0, 0));
			Point3f _ptNormal = (getLocalMatrix44f() * Point3f(0, 0, 1) - _ptCenter).Normalize();
			Point3f _ptHeading = (getLocalMatrix44f() * Point3f(0, 1, 0) - _ptCenter).Normalize();
			setLocalPosition(_ptCenter, false);
			setLocalRotate(_ptNormal, _ptHeading, false);
			//setLocalRotateMatrix44f(CMatrixTools::GetRotateMatrix44f(getLocalNormal(), getLocalHeading()), false);
			//updateModelMatrix44f();
			updateALLMatrix44f();
		}
	}

	void SceneBaseObject::updateLocalMatrix44f()
	{
		Matrix44f T, R, S;
		T.SetTranslate(getLocalPosition());
		R = CMatrixTools::GetRotateMatrix44f(getLocalNormal(), getLocalHeading());
		S.SetScale(getLocalScale());
		setLocalRotateMatrix44f(R, false);
		setLocalMatrix44f(T * R * S, false);
	}

	// 更新局部矩阵
	void SceneBaseObject::updateALLMatrix44f()
	{
		updateLocalMatrix44f();
		updateModelMatrix44f();
	}

	// 获得自身父亲的变换矩阵
	Matrix44f SceneBaseObject::getParentWorldMatrix44f()
	{
		if (!m_pParent.expired())
		{
			return m_pParent.lock()->getParentWorldMatrix44f() * m_pParent.lock()->getLocalMatrix44f();
		}
		else
		{
			Matrix44f matrix;
			matrix.SetIdentity();
			return matrix;
		}
	}

	Matrix44f SceneBaseObject::getParentLocalMatrix44f()
	{
		if (!m_pParent.expired())
		{
			return m_pParent.lock()->getLocalMatrix44f();
		}
		else
		{
			Matrix44f matrix;
			matrix.SetIdentity();
			return matrix;
		}
	}

	// 获得自身父亲的旋转矩阵
	Matrix44f SceneBaseObject::getParentWorldRotateMatrix44f()
	{
		if (!m_pParent.expired())
		{
			return m_pParent.lock()->getParentWorldRotateMatrix44f() * m_pParent.lock()->getLocalRotateMatrix44f();
		}
		else
		{
			Matrix44f matrix;
			matrix.SetIdentity();
			return matrix;
		}
	}

	Matrix44f SceneBaseObject::getParentLocalRotateMatrix44f()
	{
		if (!m_pParent.expired())
		{
			return m_pParent.lock()->getLocalRotateMatrix44f();
		}
		else
		{
			Matrix44f matrix;
			matrix.SetIdentity();
			return matrix;
		}
	}

	Point3f SceneBaseObject::getlocalPointFromWorld(Point3f ptPoint)
	{
		return Inverse(getParentWorldMatrix44f()) * ptPoint;
	}

	// 获得模型局部坐标系下的变换矩阵
	Matrix44f SceneBaseObject::getModelWorldMatrix44f()
	{
		return getParentWorldMatrix44f() * getModelLocalMatrix44f();
	}

	Point3f SceneBaseObject::getModelWorldPosition()
	{
		return getModelWorldMatrix44f() * Point3f(0, 0, 0);
	}

	Matrix44f SceneBaseObject::getModelWorldRotateMatrix44f()
	{
		return getParentWorldRotateMatrix44f() * getLocalRotateMatrix44f() * getModelRotateMatrix44f();
	}

	Point3f SceneBaseObject::getModelWorldNormal()
	{
		return (getModelWorldRotateMatrix44f() * Point3f(0, 0, 1)).Normalize();
	}

	Point3f SceneBaseObject::getModelWorldHeading()
	{
		return (getModelWorldRotateMatrix44f() * Point3f(0, 1, 0)).Normalize();
	}

	 Point3f SceneBaseObject::getModelLocalPosition()
	{
		return getModelLocalMatrix44f() * Point3f(0, 0, 0);
	}

	 Point3f SceneBaseObject::getModelLocalNormal()
	 {
		 return (getLocalRotateMatrix44f() * getModelRotateMatrix44f() * Point3f(0, 0, 1)).Normalize();
	 }

	 Point3f SceneBaseObject::getModelLocalHeading()
	 {
		 return (getLocalRotateMatrix44f() * getModelRotateMatrix44f() * Point3f(0, 1, 0)).Normalize();
	 }

	 // 设置模型旋转
	void SceneBaseObject::setModelRotate(Point3f ptNormal, Point3f ptHeading, bool bUpdate)
	{
		m_ptModelNormal = ptNormal;
		m_ptModelHeading = ptHeading;
		if (bUpdate)
		{
			updateModelMatrix44f();
			//updateModelRotateMatrix44f(m_ptModelNormal, m_ptModelHeading);
		}
	}

	// 更新模型矩阵
	void SceneBaseObject::updateModelMatrix44f()
	{
		Matrix44f T, R, S;
		T.SetTranslate(getLocalPosition());
		R = getLocalRotateMatrix44f();
		S.SetScale(getLocalScale());
		Matrix44f Tm, Rm;
		Tm.SetTranslate(getModelPosition());
		Rm = CMatrixTools::GetRotateMatrix44f(getModelNormal(), getModelHeading());
		m_mtModelRotateMatrix = Rm;
		m_mtModelMatrix = T*R*Tm*Rm*S;
	}

	// 设置父对象
	void SceneBaseObject::setParent(const boost::shared_ptr<SceneBaseObject> pParent)
	{
		// 设置父对象
		m_pParent = pParent;
	}

	// 添加子对象
	void SceneBaseObject::addChild(const boost::shared_ptr<SceneBaseObject> pObject)
	{
		if (nullptr == pObject)
		{
			return;
		}

		if (m_vChildren.end() == find(m_vChildren.begin(), m_vChildren.end(), pObject))
		{
			// 添加子对象到队列里
			m_vChildren.push_back(pObject);
		}
	}

	void SceneBaseObject::addChildren(const std::vector<boost::shared_ptr<SceneBaseObject>> pObjects)
	{
		std::vector<boost::shared_ptr<SceneBaseObject>>::const_iterator childItor = pObjects.begin();
		while (childItor != pObjects.end())
		{
			addChild(*childItor);
			childItor++;
		}
	}
	bool SceneBaseObject::getRotateMatrix44fUnderSceneObj(const boost::shared_ptr<SceneBaseObject> SceneObj, Matrix44f& resMat44f)
	{
		if (SceneObj == nullptr)
		{
			return false;
		}
		boost::shared_ptr<SceneBaseObject> pRoot = nullptr;
		bool isInSameTree = SceneBaseObject::getCommonParent(shared_from_this(), SceneObj, pRoot);
		if (isInSameTree)
		{
			boost::shared_ptr<SceneBaseObject> pObj = shared_from_this();
			Matrix44f matObj1, matObj2;
			matObj1.SetIdentity();
			matObj2.SetIdentity();
			while (pObj != pRoot)
			{
				matObj1 = pObj->getLocalRotateMatrix44f() * matObj1;
				if (pObj->getParent())
				{
					pObj = pObj->getParent();
				}
				else
				{
					break;
				}
			}
			pObj = SceneObj;
			while (pObj != pRoot)
			{
				matObj2 = pObj->getLocalRotateMatrix44f() * matObj2;
				if (pObj->getParent())
				{
					pObj = pObj->getParent();
				}
				else
				{
					break;
				}
			}
			Invert(matObj2);
			resMat44f = matObj2 * matObj1;
		}
		return isInSameTree;
	}
	bool SceneBaseObject::getMatrix44fUnderSceneObj(const boost::shared_ptr<SceneBaseObject> SceneObj, Matrix44f& resMat44f)
	{
		boost::shared_ptr<SceneBaseObject> pRoot = nullptr;
		bool isInSameTree = SceneBaseObject::getCommonParent(shared_from_this(), SceneObj, pRoot);
		if (isInSameTree)
		{
			boost::shared_ptr<SceneBaseObject> pObj = shared_from_this();
			Matrix44f matObj1, matObj2;
			matObj1.SetIdentity();
			matObj2.SetIdentity();
			while (pObj != pRoot)
			{
				matObj1 = pObj->getLocalMatrix44f() * matObj1;
				if (pObj->getParent())
				{
					pObj = pObj->getParent();
				}
				else
				{
					break;
				}
			}
			pObj = SceneObj;
			while (pObj != pRoot)

			{
				matObj2 = pObj->getLocalMatrix44f() * matObj2;
				if (pObj->getParent())
				{
					pObj = pObj->getParent();
				}
				else
				{
					break;
				}
			}
			Invert(matObj2);
			resMat44f = matObj2 * matObj1;
		}
		return isInSameTree;
	}
	bool SceneBaseObject::getModelRotateMatrix44fUnderSceneObj(const boost::shared_ptr<SceneBaseObject> SceneObj, Matrix44f& resMat44f)
	{
		boost::shared_ptr<SceneBaseObject> pRoot = nullptr;
		bool isInSameTree = SceneBaseObject::getCommonParent(shared_from_this(), SceneObj, pRoot);
		if (isInSameTree)
		{
			boost::shared_ptr<SceneBaseObject> pObj;
			Matrix44f matObj1, matObj2;
			matObj1 = getModelRotateMatrix44f(); ;
			matObj2.SetIdentity();
			if (getParent() && pObj != pRoot)
			{
				pObj = getParent();
				while (pObj != pRoot)
				{
					matObj1 = pObj->getLocalRotateMatrix44f() * matObj1;
					if (pObj->getParent())
					{
						pObj = pObj->getParent();
					}
					else
					{
						break;
					}
				}
			}
			pObj = SceneObj;
			while (pObj != pRoot)

			{
				matObj2 = pObj->getLocalRotateMatrix44f() * matObj2;
				if (pObj->getParent())
				{
					pObj = pObj->getParent();
				}
				else
				{
					break;
				}
			}
			Invert(matObj2);
			resMat44f = matObj2 * matObj1;
		}
		return isInSameTree;
	}
	bool SceneBaseObject::getModelMatrix44fUnderSceneObj(const boost::shared_ptr<SceneBaseObject> SceneObj, Matrix44f& resMat44f)
	{
		boost::shared_ptr<SceneBaseObject> pRoot = nullptr;
		bool isInSameTree = SceneBaseObject::getCommonParent(shared_from_this(), SceneObj, pRoot);
		if (isInSameTree)
		{
			boost::shared_ptr<SceneBaseObject> pObj;
			Matrix44f matObj1, matObj2;
			matObj1 = getModelLocalMatrix44f(); ;
			matObj2.SetIdentity();
			if (getParent() && pObj != pRoot)
			{
				pObj = getParent();
				while (pObj != pRoot)
				{
					matObj1 = pObj->getLocalMatrix44f() * matObj1;
					if (pObj->getParent())
					{
						pObj = pObj->getParent();
					}
					else
					{
						break;
					}
				}
			}
			pObj = SceneObj;
			while (pObj != pRoot)

			{
				matObj2 = pObj->getLocalMatrix44f() * matObj2;
				if (pObj->getParent())
				{
					pObj = pObj->getParent();
				}
				else
				{
					break;
				}
			}
			Invert(matObj2);
			resMat44f = matObj2 * matObj1;
		}
		return isInSameTree;
	}
	
	bool SceneBaseObject::compareFloat(float fValue1, float fValue2, float fAccuracy, std::string errorStr, bool bPopUp)
	{
		if (abs(fValue1 - fValue2) > fAccuracy)
		{
#ifdef USE_DEBUG_OUTPUT
			if (bPopUp)
			{

				QString outStr;
				outStr += "-----------WARNING------------\n";
				outStr += (QString::fromStdString(errorStr) 
					+ " \nvalue1: " + QString::number(fValue1) 
					+ " \nvalue2: " + QString::number(fValue2)
					+ " \n" + QString::number(abs(fValue1 - fValue2)) + ">" + QString::number(fAccuracy));

#ifdef USE_MESSAGE_BOX
				QMessageBox::warning(nullptr, "Warning", outStr, QMessageBox::Ok);
#endif // USE_MESSAGE_BOX

			}

			qDebug() << "-----------------------------------------";
			qDebug() << QString::fromStdString(errorStr) << " value1: " << fValue1 << " value2: " << fValue2 << QString::number(abs(fValue1 - fValue2)) << ">" << fAccuracy;
			qDebug() << "-----------------------------------------";
#endif 			
			return false;
		}
		return true;
	}
	bool SceneBaseObject::comparePoint3f(Point3f fPoint1, Point3f fPoint2, float fAccuracy, std::string errorStr, bool bPopUp)
	{
		if ((fPoint1-fPoint2).Norm() > fAccuracy)
		{
#ifdef USE_DEBUG_OUTPUT
			if (bPopUp)
			{

				QString outStr;
				outStr += "-----------WARNING------------\n";
				outStr += (QString::fromStdString(errorStr)
					+ " \nvalue1: x: " + QString::number(fPoint1.x) + " y: " + QString::number(fPoint1.y) + " z: " + QString::number(fPoint1.z)
					+ " \nvalue2: x: " + QString::number(fPoint2.x) + " y: " + QString::number(fPoint2.y) + " z: " + QString::number(fPoint2.z)
					+ " \n" + QString::number((fPoint1 - fPoint2).Norm()) + " > " + QString::number(fAccuracy));

#ifdef USE_MESSAGE_BOX
				QMessageBox::warning(nullptr, "Warning", outStr, QMessageBox::Ok);
#endif // USE_MESSAGE_BOX

			}

			qDebug() << "-----------------------------------------";
			qDebug() << QString::fromStdString(errorStr) << " value1: x: " << fPoint1.x << " y: " << fPoint1.y << " z: " << fPoint1.z
														<< " value2: x: " << fPoint2.x << " y: " << fPoint2.y << " z: " << fPoint2.z
														<< QString::number((fPoint1 - fPoint2).Norm()) << " > " << QString::number(fAccuracy);
			qDebug() << "-----------------------------------------";
#endif		
			return false;
		}
		return true;
	}
	bool SceneBaseObject::compareMatrix44f(Matrix44f fMatrix1, Matrix44f fMatrix2, float fAccuracy, std::string errorStr, bool bPopUp)
	{
		bool check = false;
		for (int i = 0; i < 4; ++i)
		{
			if (check)
			{
				break;
			}
			for (int j = 0; j < 4; ++j)
			{
				if (abs(fMatrix1.ElementAt(i,j) - fMatrix2.ElementAt(i, j)) > fAccuracy)
				{
					check = true;
					break;
				}
			}
		}
		if (check)
		{
#ifdef USE_DEBUG_OUTPUT
			if (bPopUp)
			{

				QString outStr;
				outStr += "-----------WARNING------------\n";
				outStr += QString::fromStdString(errorStr);
				outStr += "\nMatrix1: ";
				for (int i = 0; i < 4; ++i)
				{
					outStr += "\n" + QString::number(fMatrix1.ElementAt(i, 0), 'f', 6) + " " 
						+ QString::number(fMatrix1.ElementAt(i, 1), 'f', 6) + " "
						+ QString::number(fMatrix1.ElementAt(i, 2), 'f', 6) + " "
						+ QString::number(fMatrix1.ElementAt(i, 3), 'f', 6);
				}
				outStr += "\nMatrix2: ";
				for (int i = 0; i < 4; ++i)
				{
					outStr += "\n" + QString::number(fMatrix2.ElementAt(i, 0), 'f', 6) + " "
						+ QString::number(fMatrix2.ElementAt(i, 1), 'f', 6) + " "
						+ QString::number(fMatrix2.ElementAt(i, 2), 'f', 6) + " "
						+ QString::number(fMatrix2.ElementAt(i, 3), 'f', 6);
				}
				outStr += " \nAcc: " + QString::number(fAccuracy);

#ifdef USE_MESSAGE_BOX
				QMessageBox::warning(nullptr, "Warning", outStr, QMessageBox::Ok);
#endif // USE_MESSAGE_BOX

			}

			qDebug() << "-----------------------------------------";
			qDebug() << QString::fromStdString(errorStr);
			qDebug() << "Matrix1: ";
			for (int i = 0; i < 4; ++i)
			{
				qDebug() << fMatrix1.ElementAt(i, 0) << " " << fMatrix1.ElementAt(i, 1) << " " << fMatrix1.ElementAt(i, 2) << " " << fMatrix1.ElementAt(i, 3);
			}
			qDebug() << "Matrix2: ";
			for (int i = 0; i < 4; ++i)
			{
				qDebug() << fMatrix2.ElementAt(i, 0) << " " << fMatrix2.ElementAt(i, 1) << " " << fMatrix2.ElementAt(i, 2) << " " << fMatrix2.ElementAt(i, 3);
			}
			qDebug() << "-----------------------------------------";
#endif			
			return false;
		}
		return true;
	}
	bool SceneBaseObject::getCommonParent(const boost::shared_ptr<SceneBaseObject> SceneObj1, const boost::shared_ptr<SceneBaseObject> SceneObj2, boost::shared_ptr<SceneBaseObject>& parent)
	{
		parent = nullptr;
		if (SceneObj1 == nullptr || SceneObj2 == nullptr)
		{
			return false;
		}
		std::list<boost::shared_ptr<SceneBaseObject>> pNodeList1, pNodeList2;
		pNodeList1.push_back(SceneObj1);
		pNodeList2.push_back(SceneObj2);

		while ((*pNodeList1.rbegin())->getParent())
		{
			pNodeList1.push_back((*pNodeList1.rbegin())->getParent());
		}
		while ((*pNodeList2.rbegin())->getParent())
		{
			pNodeList2.push_back((*pNodeList2.rbegin())->getParent());
		}


		while (!pNodeList1.empty() && !pNodeList2.empty() && *pNodeList1.rbegin() == *pNodeList2.rbegin())
		{
			parent = *pNodeList1.rbegin();
			pNodeList1.pop_back();
			pNodeList2.pop_back();
		}

		return parent != nullptr;
	}

}

