#ifndef _SN3D_INTERACTOR_
#define _SN3D_INTERACTOR_


/*      sn3DInteractor类

该类定义了场景交互类的接口,主要包括: 

1.鼠标消息

2.键盘消息              */


#include <windows.h>
#include"sn3DSceneGL.h"
//#include"../../sn3DInterface/Isn3DCommandInteractor.h"


namespace sn3DCore
{
	class sn3DInteractor //: public Isn3DCommandInteractor
	{
	 public:
		
	  virtual	void SetSelectRadius(int selectRadius)		=0;  //  设置选择画刷半径
		
	};
}
#endif