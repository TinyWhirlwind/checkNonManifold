#ifndef _SN3D_SCENE_MANAGER_
#define _SN3D_SCENE_MANAGER_

#include"../../sn3DInterface/Isn3DScene.h"

/* 场景管理类 */
namespace sn3DCore
{

	class sn3DSceneManager : public Isn3DScene
	{
	public:

		bool ShareDisplay(sn3DSceneGL * scene1,sn3DSceneGL * scene2); // 共享显示列表

		void AddScene(sn3DSceneGL * scene);                           // 加入场景

		sn3DSceneGL * GetScene(int id);                               // 获取场景接口     

		void HideScene(int id);                                       // 关闭场景显示

		void ShowScene(int id);                                       // 显示场景

		void SetDrawMode(int id,DrawGL::DrawMode dm);

		void CheckShowBox(int id);

		void CheckShowModelBox(int id);

		//void SetColorMode(int id,int flag);


	public:

		std::vector<sn3DSceneGL *> m_scenes;                         // 所有场景

	};
}


#endif