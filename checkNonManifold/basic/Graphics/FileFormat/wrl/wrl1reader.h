#ifndef VRML_1_PARSE_
#define VRML_1_PARSE_

#define T_SEPARATOR	       1000
#define T_SWITCH	       1001
#define T_DEF		       1002
#define T_MATERIAL	       1003
#define T_DIFFCOLOR	       1004
#define T_SPECCOLOR	       1005
#define T_SHININESS	       1006
#define T_TRANSPARENCY	       1007
#define T_COORDINATE3	       1008
#define T_POINT		       1009
#define T_USE		       1010
#define T_INDEXED_FACESET      1011
#define T_COORD_INDEX	       1012
#define T_MATERIAL_BINDING     1013
#define T_VALUE		       1014
#define T_PER_VERTEX_INDEXED   1015
#define T_TEXTURE_COORDINATE_2 1016
#define T_MATRIX_TRANSFORM     1017
#define T_MATRIX	       1018
#define T_PERSPECTIVE_CAMERA   1019
#define T_FOCAL_DISTANCE       1020
#define T_NAME		       1021
#define T_NUMBER	       1022
#define T_COMMENT	       1023
#define T_ASCII_TEXT	       1024
#define T_CONE		       1025
#define T_CUBE		       1026
#define T_CYLINDER	       1027
#define T_DIRECTIONAL_LIGHT    1028
#define T_FONT_STYLE           1029
#define T_INDEXED_LINE_SET     1030
#define T_INFO		       1031
#define T_LOD		       1032
#define T_NORMAL	       1033
#define T_NORMAL_BINDING       1034
#define T_ORTHOGRAPHIC_CAMERA  1035
#define T_POINT_LIGHT	       1036
#define T_POINT_SET	       1037
#define T_ROTATION	       1038
#define T_SCALE		       1039
#define T_SHAPE_HINTS	       1040
#define T_SPHERE	       1041
#define T_SPOT_LIGHT	       1042
#define T_TEXTURE_2	       1043
#define T_TEXTURE_2_TRANSFORM  1044
#define T_TRANSFORM	       1045
#define T_TRANSLATION	       1046
#define T_WWW_ANCHOR	       1047
#define T_WWW_INLINE	       1048
#define T_STRING	       1049
#define T_SPACING	       1050
#define T_JUSTIFICATION	       1051
#define T_WIDTH		       1052
#define T_PARTS		       1053
#define T_BOTTOM_RADIUS	       1054
#define T_HEIGHT	       1055
#define T_DEPTH		       1056
#define T_RADIUS	       1057
#define T_ON		       1058
#define T_INTENSITY	       1059
#define T_COLOR		       1060
#define T_SIZE		       1061
#define T_FAMILY	       1062
#define T_STYLE		       1063
#define T_MATERIAL_INDEX       1064
#define T_NORMAL_INDEX	       1065
#define T_TEXTURE_COORD_INDEX  1066
#define T_RANGE		       1067
#define T_CENTER	       1068
#define T_AMBCOLOR	       1069
#define T_EMICOLOR	       1070
#define T_VECTOR	       1071
#define T_POSITION	       1072
#define T_ORIENTATION	       1073
#define T_DIRECTION	       1074
#define T_HEIGHT_ANGLE	       1075
#define T_LOCATION	       1076
#define T_START_INDEX	       1077
#define T_NUM_POINTS	       1078
#define T_F_ROTATION	       1079
#define T_SCALE_FACTOR	       1080
#define T_RENDER_CULLING       1081
#define T_VERTEX_ORDER	       1082
#define T_SHAPE_TYPE	       1083
#define T_FACE_TYPE	       1084
#define T_CREASE_ANGLE	       1085
#define T_DROP_OFF_RATE	       1086
#define T_CUT_OFF_ANGLE	       1087
#define T_WHICH_CHILD	       1088
#define T_FILE_NAME	       1089
#define T_IMAGE		       1090
#define T_WRAP_S	       1091
#define T_WRAP_T	       1092
#define T_F_TRANSLATION	       1093
//#define T_F_ROTATION	       1035		
#define T_SCALE_ORIENTATION    1094
#define T_F_NAME	       1095
#define T_DESCRIPTION	       1096
#define T_MAP		       1097
#define T_BBOX_SIZE	       1098
#define T_BBOX_CENTER	       1099
#define T_LEFT				1100
//#define T_CENTER	       1101		
#define T_RIGHT		       1102
#define T_SIDES		       1103
#define T_BOTTOM	       1104
#define T_ALL		       1105
#define T_TOP		       1106
#define T_SERIF		       1107
#define T_SANS		       1108
#define T_TYPEWRITER	       1109
#define T_NONE		       1110
#define T_BOLD		       1111
#define T_ITALIC	       1112
#define T_DEFAULT	       1113
#define T_OVERALL	       1114
#define T_PER_PART	       1115
#define T_PER_PART_INDEXED     1116
#define T_PER_FACE	       1117
#define T_PER_FACE_INDEXED     1118
#define T_PER_VERTEX	       1119
#define T_AUTO		       1120
#define T_V_ON		       1121
#define T_V_OFF		       1122
#define T_UNK_ORDERING	       1123
#define T_CLOCKWISE	       1124
#define T_CO_CLOCKWISE	       1125
#define T_UNK_SHAPE	       1126
#define T_V_SOLID	       1127
#define T_UNK_FACE	       1128
#define T_V_CONVEX	       1129
#define T_REPEAT	       1130
#define T_CLAMP		       1131
#define T_V_POINT	       1132
#define T_V_STRING	       1133
#define T_TRUE		       1134
#define T_FALSE		       1136


#include<string>
#include "../BaseReader.h"

using namespace std;
struct CONVRMLSFImage 
{
	int width, height, num_components;
	int *pixel;
};

struct vrml_1_fields 
{
	int field_name;
	union field_values 
	{
		char **str_value;    /*representing {SF, MF}String*/
		int enum_value;      /*representing SFEnum*/
		int *bitmask_value;  /*representing SFBitMask*/
		int boolean_value;   /*representing SFBool*/
		float *float_value;  /*representing SFColor, MFFloat, SFRotation, SF{Vec2f, Vec3f}*/
		float s_float_value; /*representing SFFloat*/
		int *int_value;      /*representing MFLong*/
		int s_int_value;     /*representing SFLong*/
		float **matrix_value;/*representing MFColor, SFMatrix, MF{Vec2f, Vec3f}*/
		CONVRMLSFImage *image_value; /*representing SFImage*/
	} field_value;
	int num_values;
};

struct vrml_1_node 
{ 
	int node_type;
	char node_name[25];
	int is_instance; /*if this node occured within the scope of an use statement, in this case 'chidren' is a reference to the defined node*/
	struct vrml_1_fields *fields; /*contains all field values of one node*/
	int num_fields;
	struct vrml_1_node **children;
	int num_children;
};

class Wrl1Reader
{
public:

  bool read(string & file_name,BaseImporter& _bi);

  void AddNodeInfo(vrml_1_node * node,BaseImporter& _bi);

  void ClearNode(vrml_1_node * node);

};
extern int init_parser(char *fname);
extern int exit_parser();
extern int get_lineno();
extern vrml_1_node *parse_node(int level);

#endif
