#define	MAIN3DS	                    0x4D4D
#define	MATMAGIC	                0x3DFF 
#define	M3D_VERSION	                0x0002
/*0x4D4D  包含0x3D3D主编辑块(EDIT3DS)和关键臻块忽略  */
#define	EDIT3DS		                0x3D3D     
#define		MESH_VERSION            0x3D3E
#define		MASTER_SCALE            0x0100
#define		EDIT_VIEW_P1            0x7012
#define		EDIT_VIEW_P2            0x7011
#define		EDIT_VIEW_P3            0x7020
#define		EDIT_VIEW1              0x7001     
#define		BIT_MAP				    0x1100
#define		USE_BIT_MAP             0x1101
#define		USE_SOLID_BGND          0x1201
#define		V_GRADIENT              0x1300
#define		USE_V_GRADIENT          0x1301
#define		EDIT_BACKGR             0x1200
#define		EDIT_AMBIENT		    0x2100

/*0x3D3D 包含 物体块0x4000和材质块 0xAFFF  */
#define		DEFAULT_VIEW		    0x3000

#ifdef VIEW_FRONT
	#undef VIEW_FRONT
#endif
#ifdef VIEW_BACK
	#undef VIEW_BACK
#endif
#ifdef VIEW_LEFT
	#undef VIEW_LEFT
#endif
#ifdef VIEW_RIGHT
	#undef VIEW_RIGHT
#endif
#ifdef VIEW_TOP
	#undef VIEW_TOP
#endif
#ifdef VIEW_BOTTOM
	#undef VIEW_BOTTOM
#endif

#define		VIEW_TOP                0x3010
#define		VIEW_BOTTOM			    0x3020
#define		VIEW_LEFT               0x3030
#define		VIEW_RIGHT              0x3040
#define		VIEW_FRONT              0x3050
#define		VIEW_BACK               0x3060
#define		VIEW_USER               0x3070
#define		VIEW_CAMERA             0x3080
#define		VIEW_WINDOW             0x3090
/*物体块包含 三角网格0x4100 灯光0x4600 照相机0x4700   */
#define		EDIT_OBJECT             0x4000 
#define		OBJ_HIDDEN              0x4010
#define		OBJ_VIS_LOFTER          0x4011
#define		OBJ_MATTE               0x4013
#define		OBJ_FAST                0x4014
#define		OBJ_PROCEDURAL          0x4015
#define		OBJ_FROZEN              0x4016
#define		OBJ_DONT_RCVSHADOW      0x4017

#define		OBJ_TRIMESH             0x4100 
#define		TRI_VERTEX              0x4110
#define		TRI_VERTEXOPTIONS       0x4111
#define		TRI_MAPPINGCOORS	    0x4140
#define		TRI_LOCAL               0x4160
#define		TRI_VISIBLE             0x4165   
#define		TRI_MAPPINGSTANDARD     0x4170
#define		TRI_FACE                0x4120
#define		TRI_SMOOTH		        0x4150
#define		TRI_MATERIAL            0x4130


#define		OBJ_LIGHT               0x4600 
#define		DL_SPOTLIGHT            0x4610
#define		DL_OFF                  0x4620
#define		DL_ATTENUATE            0x4625
#define		DL_RAYSHAD              0x4627
#define		DL_SHADOWED             0x4630
#define		DL_LOCAL_SHADOW         0x4640
#define		DL_LOCAL_SHADOW2        0x4641
#define		DL_SEE_CONE             0x4650
#define		DL_SPOT_RECTANGULAR     0x4651
#define		DL_SPOT_OVERSHOOT       0x4652
#define		DL_SPOT_PROJECTOR       0x4653
#define		DL_EXCLUDE              0x4654
#define		DL_SPOT_ROLL            0x4656
#define		DL_SPOT_ASPECT          0x4657
#define		DL_RAY_BIAS             0x4658
#define		DL_INNER_RANGE 		    0x4659
#define		DL_OUTER_RANGE 		    0x465A
#define		DL_INTENSITY		    0x465B
#define		N_AMBIENT_LIGHT         0x4680

#define		OBJ_CAMERA              0x4700
#define		CAM_SEE_CONE            0x4710
#define		CAM_RANGES              0x4720

/* 材质块包含纹理等信息  */
#define		EDIT_MATERIAL           0xAFFF
#define		MAT_NAME                0xA000
#define		MAT_AMBIENT             0xA010
#define		MAT_DIFFUSE             0xA020
#define		MAT_SPECULAR            0xA030
#define		MAT_SHININESS           0xA040
#define		MAT_SHIN2PCT		    0xA041
#define		MAT_SHIN3PCT		    0xA042
#define		MAT_TRANSPARENCY  	    0xA050
#define		MAT_XPFALL              0xA052
#define		MAT_REFBLUR             0xA053
#define		MAT_SELF_ILLUM          0xA080
#define		MAT_TWO_SIDE            0xA081
#define		MAT_DECAL               0xA082
#define		MAT_ADDITIVE            0xA083
#define		MAT_SELF_ILPCT  	    0xA084
#define		MAT_WIRE			    0xA085
#define		MAT_SUPERSMP		    0xA086
#define		MAT_WIRESIZE		    0xA087
#define		MAT_FACEMAP		 	    0xA088
#define		MAT_XPFALLIN		    0xA08A
#define		MAT_PHONGSOFT	 	    0xA08C
#define		MAT_WIREABS  		    0xA08E
#define		MAT_SHADING             0xA100
#define		MAT_TEXMAP              0xA200
#define		MAT_OPACMAP             0xA210
#define		MAT_REFLMAP             0xA220
#define		MAT_BUMPMAP             0xA230
#define		MAT_SPECMAP             0xA204
#define		MAT_USE_XPFALL          0xA240
#define		MAT_USE_REFBLUR         0xA250
#define		MAT_BUMP_PERCENT        0xA252
#define		MAT_MAPNAME             0xA300
#define		MAT_ACUBIC              0xA310
#define		MAT_TEX2MAP 		    0xA33A
#define		MAT_SHINMAP 		    0xA33C
#define		MAT_SELFIMAP 	  	    0xA33D
#define		MAT_TEXMASK 		    0xA33E
#define		MAT_TEX2MASK 		    0xA340
#define		MAT_OPACMASK 		    0xA342
#define		MAT_BUMPMASK 	 	    0xA344
#define		MAT_SHINMASK 		    0xA346
#define		MAT_SPECMASK 		    0xA348
#define		MAT_SELFIMASK 		    0xA34A
#define		MAT_REFLMASK 		    0xA34C
#define		MAT_MAP_TILINGOLD       0xA350
#define		MAT_MAP_TILING 		    0xA351
#define		MAT_MAP_TEXBLUR_OLD     0xA352
#define		MAT_MAP_TEXBLUR 	    0xA353
#define		MAT_MAP_USCALE 		    0xA354
#define		MAT_MAP_VSCALE 		    0xA356
#define		MAT_MAP_UOFFSET 	    0xA358
#define		MAT_MAP_VOFFSET 	    0xA35A
#define		MAT_MAP_ANG 		    0xA35C
#define		MAT_MAP_COL1 		    0xA360
#define		MAT_MAP_COL2 		    0xA362
#define		MAT_MAP_RCOL 		    0xA364
#define		MAT_MAP_GCOL 	 	    0xA366
#define		MAT_MAP_BCOL 		    0xA368




#define		COLOR_F				    0x0010
#define		COLOR_24			    0x0011
#define		INT_PERCENTAGE		    0x0030
#define		FLOAT_PERCENTAGE	    0x0031

