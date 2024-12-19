#include"stdafx.h"
#include"sn3DSceneManager.h"


namespace sn3DCore
{

	bool sn3DSceneManager::ShareDisplay(sn3DSceneGL * scene1,sn3DSceneGL * scene2)
	{

		return true;
	}
	sn3DSceneGL * sn3DSceneManager::GetScene(int id)
	{
		if((int)m_scenes.size()>0&&id>=0&&id<(int)m_scenes.size())
			return m_scenes[id];
		else return NULL;
	}
	void sn3DSceneManager::HideScene(int id)
	{

	}	
	void sn3DSceneManager::ShowScene(int id)
	{

	}
	void sn3DSceneManager::AddScene(sn3DSceneGL * scene)
	{
		m_scenes.push_back(scene);
	}
	void sn3DSceneManager::SetDrawMode(int id,DrawGL::DrawMode dm)
	{
		//m_scenes[id]->SetDrawMode(dm);
	}
	void sn3DSceneManager::CheckShowBox(int id)
	{
		m_scenes[id]->CheckShowBox();
	}
	/*void sn3DSceneManager::SetColorMode(int id,int flag)
	{
		m_scenes[id]->SetColorMode(flag);
	}*/

}