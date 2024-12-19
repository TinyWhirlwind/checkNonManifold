#include"stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "Wrl1Reader.h"

#pragma warning(disable:4996)
#pragma warning(disable:4700)
#pragma warning(disable:4101)
static struct keyword_str 
{
	char *key;
	int tnum;
} kwords[] = {{"DEF",	  T_DEF},
	      {"USE"	  , T_USE},
	      {"AsciiText", T_ASCII_TEXT},
	      {"Cone", T_CONE},
	      {"Cube", T_CUBE},
	      {"Cylinder", T_CYLINDER},
	      {"DirectionalLight", T_DIRECTIONAL_LIGHT},
	      {"FontStyle", T_FONT_STYLE},
	      {"IndexedLineSet", T_INDEXED_LINE_SET},
	      {"Info", T_INFO},
	      {"LOD", T_LOD},
	      {"Normal", T_NORMAL},
	      {"NormalBinding", T_NORMAL_BINDING},
	      {"OrthographicCamera", T_ORTHOGRAPHIC_CAMERA},
	      {"PointLight", T_POINT_LIGHT},
	      {"PointSet", T_POINT_SET},
	      {"Rotation", T_ROTATION},
	      {"Scale", T_SCALE},
	      {"ShapeHints", T_SHAPE_HINTS},
	      {"Sphere", T_SPHERE},
	      {"SpotLight", T_SPOT_LIGHT},
	      {"Texture2", T_TEXTURE_2},
	      {"Texture2Transform", T_TEXTURE_2_TRANSFORM},
	      {"Transform", T_TRANSFORM},
	      {"Translation", T_TRANSLATION},
	      {"WWWAnchor", T_WWW_ANCHOR},
	      {"WWWInline", T_WWW_INLINE},
	      {"Separator", T_SEPARATOR},
	      {"Switch",    T_SWITCH},
	      {"Material",  T_MATERIAL},
	      {"Coordinate3", T_COORDINATE3},
	      {"IndexedFaceSet", T_INDEXED_FACESET},
	      {"MaterialBinding", T_MATERIAL_BINDING},
	      {"TextureCoordinate2", T_TEXTURE_COORDINATE_2},
	      {"MatrixTransform", T_MATRIX_TRANSFORM},
	      {"PerspectiveCamera", T_PERSPECTIVE_CAMERA},
	      {"string", T_STRING},
	      {"spacing", T_SPACING},
	      {"justification", T_JUSTIFICATION},
	      {"width", T_WIDTH},
	      {"parts", T_PARTS},
	      {"bottomRadius", T_BOTTOM_RADIUS},
	      {"height", T_HEIGHT},
	      {"point"	  , T_POINT},
	      {"depth", T_DEPTH},
	      {"radius", T_RADIUS},
	      {"on", T_ON},
	      {"intensity", T_INTENSITY},
	      {"color", T_COLOR},
	      {"direction", T_DIRECTION},
	      {"size", T_SIZE},
	      {"family", T_FAMILY},
	      {"style", T_STYLE},
	      {"coordIndex",  T_COORD_INDEX},
	      {"materialIndex", T_MATERIAL_INDEX},
	      {"normalIndex", T_NORMAL_INDEX},
	      {"textureCoordIndex", T_TEXTURE_COORD_INDEX},
	      {"range", T_RANGE},
	      {"center", T_CENTER},
	      {"ambientColor", T_AMBCOLOR},
	      {"diffuseColor", T_DIFFCOLOR},
	      {"specularColor", T_SPECCOLOR},
	      {"emissiveColor", T_EMICOLOR},
	      {"shininess", T_SHININESS},
	      {"transparency", T_TRANSPARENCY},
	      {"value", T_VALUE},
	      {"matrix", T_MATRIX},
	      {"vector", T_VECTOR},
	      {"position", T_POSITION},
	      {"orientation", T_ORIENTATION},
	      {"focalDistance", T_FOCAL_DISTANCE},
	      {"heightAngle", T_HEIGHT_ANGLE},
	      {"location", T_LOCATION},
	      {"startIndex", T_START_INDEX},
	      {"numPoints", T_NUM_POINTS},
	      {"rotation", T_F_ROTATION},
	      {"scaleFactor", T_SCALE_FACTOR},
	      {"renderCulling", T_RENDER_CULLING},
	      {"vertexOrdering", T_VERTEX_ORDER},
	      {"shapeType", T_SHAPE_TYPE},
	      {"faceType", T_FACE_TYPE},
	      {"creaseAngle", T_CREASE_ANGLE},
	      {"dropOffRate", T_DROP_OFF_RATE},
	      {"cutOffAngle", T_CUT_OFF_ANGLE},
	      {"whichChild", T_WHICH_CHILD},
	      {"filename", T_FILE_NAME},
	      {"image", T_IMAGE},
	      {"wrapS", T_WRAP_S},
	      {"wrapT", T_WRAP_T},
	      {"translation", T_F_TRANSLATION},
	      {"rotation", T_F_ROTATION},
	      {"scaleOrientation", T_SCALE_ORIENTATION},
	      {"name", T_F_NAME},
	      {"description", T_DESCRIPTION},
	      {"map", T_MAP},
	      {"bboxSize", T_BBOX_SIZE},
	      {"bboxCenter", T_BBOX_CENTER},
	      {"LEFT", T_LEFT},
	      {"CENTER", T_CENTER},
	      {"RIGHT", T_RIGHT},
	      {"SIDES", T_SIDES},
	      {"BOTTOM", T_BOTTOM},
	      {"ALL", T_ALL},
	      {"TOP", T_TOP},
	      {"SERIF", T_SERIF},
	      {"SANS", T_SANS},
	      {"TYPEWRITER", T_TYPEWRITER},
	      {"NONE", T_NONE},
	      {"BOLD", T_BOLD},
	      {"ITALIC", T_ITALIC},
	      {"DEFAULT", T_DEFAULT},
	      {"OVERALL", T_OVERALL},
	      {"PER_PART", T_PER_PART},
	      {"PER_PART_INDEXED", T_PER_PART_INDEXED},
	      {"PER_FACE", T_PER_FACE},
	      {"PER_FACE_INDEXED", T_PER_FACE_INDEXED},
	      {"PER_VERTEX", T_PER_VERTEX},
	      {"PER_VERTEX_INDEXED", T_PER_VERTEX_INDEXED},
	      {"AUTO", T_AUTO},
	      {"ON", T_V_ON},
	      {"OFF", T_V_OFF},
	      {"UNKNOWN_ORDERING", T_UNK_ORDERING},
	      {"CLOCKWISE", T_CLOCKWISE},
	      {"COUNTERCLOCKWISE", T_CO_CLOCKWISE},
	      {"UNKNOWN_SHAPE_TYPE", T_UNK_SHAPE},
	      {"SOLID", T_V_SOLID},
	      {"UNKNOWN_FACE_TYPE", T_UNK_FACE},
	      {"CONVEX", T_V_CONVEX},
	      {"REPEAT", T_REPEAT},
	      {"CLAMP", T_CLAMP},
	      {"POINT", T_V_POINT},
	      {"TRUE", T_TRUE},
	      {"FALSE", T_FALSE},
	      {NULL, 0}
};

static int lineno;
static FILE *in;
static int eof = 0;
static float fval;
static float b_fval;
static char *sval;
static char *b_sval;
static char varString[128];
static char *filename;
static int lookahead;
static int unread_char = 0;
static char last_char;
static std::vector<float> g_fvalues;
static std::vector<int> g_ivalues;



static struct vrml_1_node_stack 
{
	struct vrml_1_node one_node;
	struct vrml_1_node_stack *next;
} *node_stack;

static struct def_list {
	char def_name[25];
	struct vrml_1_node *node_ref;
	struct def_list *next_item;
} *defined_names;

static struct vrml_1_node *scene_graph;
static struct vrml_1_node current_node;

static char *vrml_err_str = "no vrml error";


static int nextToken2();

static void reset_current_node() 
{
	current_node.node_type = 0;
	current_node.node_name[0] = '\0';
	current_node.is_instance = 0;
	current_node.fields = NULL;	current_node.num_fields = 0;
	current_node.children = NULL;	current_node.num_children = 0;
}
static struct def_list *insert_item_to_defined_names(char *name, struct vrml_1_node *node) {
	char Str[20];
	struct def_list *list_head = defined_names;
	struct def_list *new_node;
	if((new_node = (struct def_list *) malloc(sizeof(struct def_list))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	if (24 < strlen(name)) name[24] = '\0';
	strcpy(new_node->def_name, name);
	new_node->node_ref = node;
	new_node->next_item = list_head;
	defined_names = new_node;
	return new_node;
}

static struct vrml_1_fields *get_fields(int how_many) {
	char Str[20];
	struct vrml_1_fields *fresh_fields;
	if ((fresh_fields = (struct vrml_1_fields *) malloc(how_many*sizeof(struct vrml_1_fields))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	return fresh_fields;
}

static struct vrml_1_node_stack *push_node_stack() {
	char Str[20];
	struct vrml_1_node_stack *head = node_stack;
	struct vrml_1_node_stack *new_item;
	if ((new_item = (struct vrml_1_node_stack *) malloc(sizeof(struct vrml_1_node_stack))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	new_item->one_node.node_type = current_node.node_type;
	strcpy(new_item->one_node.node_name, current_node.node_name);
	new_item->one_node.is_instance = current_node.is_instance;
	new_item->one_node.fields = current_node.fields;
	new_item->one_node.num_fields = current_node.num_fields;
	new_item->one_node.children = current_node.children;
	new_item->one_node.num_children = current_node.num_children;
	new_item->next = head;
	node_stack = new_item;
	return new_item;
}

static struct vrml_1_node *pop_node_stack() {
	char Str[20];
	struct vrml_1_node *fresh_node;
	struct vrml_1_node_stack *memo = node_stack;
	if ((fresh_node = (struct vrml_1_node *) malloc(sizeof(struct vrml_1_node))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	fresh_node->node_type = (current_node.node_type = node_stack->one_node.node_type);
	strcpy(current_node.node_name, node_stack->one_node.node_name);
	strcpy(fresh_node->node_name, node_stack->one_node.node_name);
	fresh_node->is_instance = (current_node.is_instance = node_stack->one_node.is_instance);
	fresh_node->fields = (current_node.fields = node_stack->one_node.fields);
	fresh_node->num_fields = (current_node.num_fields = node_stack->one_node.num_fields);
	fresh_node->children = (current_node.children = node_stack->one_node.children);
	fresh_node->num_children = (current_node.num_children = node_stack->one_node.num_children);
	node_stack = node_stack->next;
	free(memo);
	return fresh_node;
}

static struct vrml_1_node *new_node_from_current_node() {
	struct vrml_1_node *requested_node;
	push_node_stack();
	requested_node = pop_node_stack();
	return requested_node;
}

static void insert_name_to_node(char *name, struct vrml_1_node *node) {
	if (24 < strlen(name)) name[24] = '\0';
	strcpy(node->node_name, name);
}

static vrml_1_node **get_children_p(int i) 
{
	vrml_1_node **p_field;
	char Str[20];
	if (i == 0) return NULL;
	if ((p_field = (vrml_1_node **) malloc(i*sizeof(vrml_1_node *))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	return p_field;
}
static void synerr(char *err) 
{
	sprintf(varString, "file \"%s\" line %d: %s", filename, lineno, err);
	vrml_err_str = varString;
}
char nextChar() 
{
	char c;
	if(unread_char == 1) 
	{
	  unread_char = 0;
	  return last_char;
	}
	if((c = getc(in)) == EOF) 
	{
	  eof = 1;
	  return 0;
	}
	if(c == '\n') lineno++;
	last_char = c;
	return c;
}
static int unread() 
{
  if(unread_char == 1){return 0;}
  unread_char = 1;
  return 1;
}
static int nextToken1() 
{
    #define MAXF 512
	int i;
	int str_end = 0;
	char format[3];
	char Str[80];
	char c, f[MAXF], b;
	struct keyword_str *kptr;

	if (eof) return 0;
	while ((c = nextChar()) && !eof && (c == ' ' || c == '\t' || c == '\n' || c == '\r'));
	if (eof) return 0;

	if (c == '#') 
	{
	  while ((c = nextChar()) && !eof && (c != '\n' ||  c == '\r'));
	  if (eof) return 0;
	  return T_COMMENT;
	}

	if(c == '"') 
	{
	  for(i = 0; !eof && i < MAXF && !str_end; i++) 
	  {
		b = c;
		c = nextChar();
		if (b == '\\') 
		{
		  f[i] = c;
		}
		else 
		{
		  if (c == '\\') continue;
		  else if (c == '"') {str_end = 1; continue;}
		  else 
		  {
			f[i] = c;
		  }
		}
	  }

		f[i-1] = '\0';
		
		free(sval);

		if((sval = strdup(f)) == NULL) 
		{
			sprintf(Str, "Fehler in strdup");
			exit(-1);
		}
		return T_V_STRING;
	}

	if ((c >= '0' && c <= '9') || c == '.' || c == '-') 
	{
		strcpy(format, "%f");
		for (i = 0; !eof && i < MAXF && ((c >= '0' && c <= '9') || c == '.' || c == '-' || c == 'x'); i++) 
		{
			if (c == 'x' && i != 1) break;
			else if (c == 'x' && i == 1) strcpy(format, "%x");
			else if (c == '0' && i == 0) strcpy(format, "%o");
			else if (c == '.') strcpy(format, "%f");
			f[i] = c;
			c = nextChar();
		}
		unread();
		f[i] = '\0';
		if (sscanf(f, format, &fval) != 1) 
		{
			vrml_err_str = "Number format error";
			return -1;
		}
		return T_NUMBER;
	}
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') 
	{
		for (i = 0; !eof && i < MAXF && ((c >= 'A' && c <= 'Z') ||
						 (c >= 'a' && c <= 'z') ||
						 (c >= '0' && c <= '9') || c== '_'); i++) 
		{
			f[i] = c;
			c =  nextChar();
		}
		unread();
		f[i] = '\0';

		for (kptr = kwords; kptr->key != NULL; kptr++) 
		{
			if(!strcmp(kptr->key, f)) 
			{
				free(sval);
				if ((sval = strdup(f)) == NULL) 
				{
					sprintf(Str, "Fehler in strdup");
					exit(-1);
				}
				return kptr->tnum;
			}
		}
		free(sval);

		if ((sval = strdup(f)) == NULL) 
		{
			sprintf(Str, "Fehler in strdup");
			exit(-1);
		}
		return T_NAME;
	}
	return ((int) c);
}
static int nextToken2() 
{
	int token;
	while ((token = nextToken1()) == T_COMMENT);
	return token;
}
static int nextToken() 
{
	float b2_fval;
	char *b2_sval;
	int b_look;
	b_look = lookahead;
	lookahead = nextToken2();
	b2_fval = fval;
	b2_sval = strdup(sval);
	fval = b_fval;
	free(sval);
	sval = strdup(b_sval);
	b_fval = b2_fval;
	free(b_sval);
	b_sval = strdup(b2_sval);
	free(b2_sval);
	return b_look;
}
static int expect_token(int t) 
{
	int token;
	token = nextToken();
	if (token < 0) return 0;
	if (token != t) {
		sprintf(varString, "file \"%s\" line %d: '%c'", filename, lineno, t);
		vrml_err_str = varString;
		return 0;
	}
	return 1;
}
static char **parse_SFString() {
	int token;
	char Str[27];
	char **new_str;
	if ((lookahead != T_V_STRING) && (lookahead != T_NAME)) return NULL;
	token = nextToken();
	if (((new_str = (char **) malloc(sizeof(char *))) == NULL) || ((new_str[0] = strdup(sval)) == NULL)) {
		sprintf(Str, "error in strdup or malloc");
	
		exit(-1);
	}
	return new_str;
}

static int parse_MFString_R(char ***new_str, int level, int *count) {
 	char Str[20];
	char *memo_str;
	int token;
	while ((token != '[') && ((char)lookahead == ',')) token = nextToken();
	if ((char)lookahead == ']') {
		if (level == 0) return 0;
		if ((*new_str = (char **) malloc(level*sizeof(char *))) == NULL) {
			sprintf(Str, "error in malloc");
		
			exit(-1);
		}
		*count = level;
	}
	else {
		if (eof) return 0;
		token = nextToken();
		if (token == ',') token = nextToken();
		if (token != T_NAME && token != T_V_STRING) {
			synerr("string expected");
	
			parse_MFString_R(new_str, level+1, count);
		}
		else {
			memo_str = strdup(sval);
			if (!(parse_MFString_R(new_str, level+1, count))) return 0;
			(*new_str)[level] = strdup(memo_str);
			free(memo_str);
		}
	}
	return 1;
}

static char **parse_MFString(int *co) {
	int token;
	char **new_str;
	if ((char)lookahead != '[') { *co = 1; new_str = parse_SFString(); }
	else {
		token = nextToken();
		if (!parse_MFString_R(&new_str, 0, co)) { *co = 0; return NULL; }
	}
	return new_str;
}

static float *parse_SFFloat() {
	int token;
	float *new_float;
	char Str[20];
	if (lookahead != T_NUMBER) return NULL;
	token = nextToken();
	if ((new_float = (float *) malloc(sizeof(float))) == NULL) {
		sprintf(Str, "error in malloc");
	
		exit(-1);
	}
	*new_float = fval;
	return new_float;
}

static int parse_MFFloat_R(float **new_float, int level, int *count) 
{
	int token;
	char Str[20];
	float memo;

	while ((token != '[') && ((char)lookahead == ',')) token = nextToken();
	if ((char)lookahead == ']') {
		if (level == 0) return 0;
		if ((*new_float = (float *) malloc(level*sizeof(float))) == NULL) {
			sprintf(Str, "error in malloc");
			exit(-1);
		}
		*count = level;
	}
	else {
		token = nextToken();
		if (token == ',') token = nextToken();
		if (token != T_NUMBER) {
			synerr("number expected");
			parse_MFFloat_R(new_float, level+1, count);
		}
		else {
			memo = fval;
			if (!(parse_MFFloat_R(new_float, level+1, count))) return 0;
			(*new_float)[level] = memo;
		}
	}
	return 1;
}

static float *parse_MFFloat(int *co) {
	int token;
	float *new_float;
	*co = 0;
	if ((char)lookahead != '[') {*co = 1; new_float = parse_SFFloat();}
	else {
		token = nextToken();
		if (!parse_MFFloat_R(&new_float, 0, co)) return NULL;
	}
	return new_float;
}

static int parse_SFEnum() {
	int token;
	token = nextToken();
	if (token < 0) return 0;
	return token;
}

static int *parse_SFBitMask(int *co) {
	int token;
	int memo[32];
	int *new_bit;
	int i;
	char Str[20];
	token = nextToken();
	if (token != '(') {
		//token = nextToken();
		if ((new_bit = (int *) malloc(sizeof(int))) == NULL) {
			sprintf(Str, "error in malloc");
			exit(-1);
		}
		*new_bit=token;
		return new_bit;
	}
	else {
		token = nextToken();
		for (i = 0; token != ')' && token >= 0 && i < 32; i++) {
			if (token == '|') token = nextToken();
			memo[i] = token;
			token = nextToken();
		}
		if ((new_bit = (int *) malloc(i*sizeof(int))) == NULL) {
			sprintf(Str, "error in malloc");
			exit(-1);
		}
		*co = i;
		for (i = 0; i < *co; i++) {new_bit[i] = memo[i];}
		return new_bit;
	}
}

static float *parse_SFVecXf(int x) 
{
	int token;
	int i;
	char Str[20];
	float *new_vec;

	if ((char)lookahead == '[') token = nextToken();
	if ((new_vec = (float *) malloc(x*sizeof(float))) == NULL) 
	{
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	for (i = 0; i < x; i++) 
	{
		while ((token = nextToken()) == ',');
		if (token != T_NUMBER) {free(new_vec);  return NULL;}
		new_vec[i] = fval;
	}
	return new_vec;
}

static float *parse_SFVec3f() {
	float *new_vector;
	new_vector = parse_SFVecXf(3);
	return new_vector;
}

static float *parse_SFVec2f() {
	float *new_vector;
	new_vector = parse_SFVecXf(2);
	return new_vector;
}

static float **parse_MFVecXf(int *co, int x, int level) 
{
	int token;
	int err = 0;
	char Str[20];
	float **new_vector;
	float *one_elem;

	if(level == 0 && (char)lookahead != '[') 
	{
	  if ((new_vector = (float **) malloc(sizeof(float *))) == NULL) err = 1;
	  if ((new_vector[0] = parse_SFVecXf(x)) == NULL) return NULL;
	  *co = 1;
	}
	else if ((char)lookahead == ']') 
	{
	  if ((new_vector = (float **) malloc(level*sizeof(float *))) == NULL) err = 1;
		*co = level;
		return new_vector;
	}
	else 
	{
	   if (level == 0) token = nextToken();
	  do{
	     //if (token == ']') {*co = 0; return NULL;}
	     if ((one_elem = parse_SFVecXf(x)) == NULL) {*co = 0; return NULL;}
		 for(int k=0;k<x;k++)
		 g_fvalues.push_back(one_elem[k]);
		 delete one_elem;
         if ((char)lookahead == ',') token = nextToken();
	  }while((char)lookahead != ']');

	  *co = g_fvalues.size()/x;
	  
	  //if ((new_vector = parse_MFVecXf(co, x, level+1)) == NULL) err = 1;
	  //else new_vector[level] = one_elem;

	}
	if (err == 1) 
	{
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	return new_vector;
}
static float **parse_MFVec3f(int *count) {
	float **new_v;
	new_v = parse_MFVecXf(count, 3, 0);
	return new_v;
}

static int *parse_SFBool() {
	int token;
	int *new_bool;
	char Str[20];
	token = nextToken();
	if (token != T_NUMBER && token != T_TRUE && token != T_FALSE) return NULL;
	if ((new_bool = (int *) malloc(sizeof(int))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	if (token == T_NUMBER) {
		if (fval == 0) *new_bool = 0;
		else *new_bool = 1;
	}
	if (token == T_TRUE) *new_bool = 1;
	else *new_bool = 0;
	return new_bool;
}

static float *parse_SFColor() {
	int token;
	int i;
	float *new_color;
	char Str[25];
	if ((char)lookahead == '[') token = nextToken();
	if ((new_color = (float *) malloc(3*sizeof(float))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	for (i = 0; i < 3; i++) {
		if ((token = nextToken()) != T_NUMBER) {free(new_color); return NULL;}
		new_color[i] = fval;
	}
	return new_color;
}

static float **parse_MFColor(int *co) {
	float **new_c;
	new_c = parse_MFVec3f(co);
	return new_c;
}

static int *parse_SFLong() {
	int token;
	int *new_int;
	char Str[20];
	if ((token = nextToken()) != T_NUMBER) return NULL;
	if ((new_int = (int *) malloc(sizeof(int))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	new_int[0] = (int) fval;
	return new_int;
}

static int *parse_MFLong_R(int *co, int level) 
{
	int token;
	int *new_int;
	int memo;
	//char Str[20];
	if (level == 0 && (char)lookahead != '[') 
	{
		new_int = parse_SFLong();
		*co = 1;
	}
	else 
	{
	  new_int  =NULL;
	  do{ 
		 do{
		    token = nextToken();
			if((char)lookahead == ']')break;
		  }while(token != T_NUMBER) ;

		  //if (token == ',' || token == '[') token = nextToken();
		  if((char)lookahead != ']')
		  {
		   memo = (int) fval;
		   g_ivalues.push_back(memo);
		  }
	  }while((char)lookahead != ']');
	}
	return new_int;
}
static int *parse_MFLong(int *co) 
{
	int *new_int;
	new_int = parse_MFLong_R(co, 0);
	return new_int;
}
static float **parse_SFMatrix() 
{
	int i;
	float **new_matrix;
	char Str[20];
	if ((new_matrix = (float **) malloc(4*sizeof(float *))) == NULL) 
	{
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	for (i = 0; i < 4; i++) 
	{
		if ((new_matrix[i] = parse_SFVecXf(4)) == NULL) {free(new_matrix); return NULL;}
	}
	return new_matrix;
}

static float *parse_SFRotation() 
{
	float *new_float;
	new_float = parse_SFVecXf(4);
	return new_float;
}

static CONVRMLSFImage *parse_SFImage() 
{
	int token;
	int i;
	int err = 0;
	char Str[20];
	CONVRMLSFImage *new_image;
	if (( new_image = (CONVRMLSFImage *) malloc(sizeof(CONVRMLSFImage))) == NULL) err = 1;
	if ((token = nextToken()) != T_NUMBER) return NULL;
	new_image->width = (int) fval;
	if ((token = nextToken()) != T_NUMBER) return NULL;
	new_image->height = (int) fval;
	if ((token = nextToken()) != T_NUMBER) return NULL;
	new_image->num_components = (int) fval;
	if ((new_image->pixel = (int *) malloc(new_image->width*new_image->height*sizeof(int))) == NULL) err = 1;
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	for (i = 0; i < new_image->width*new_image->height; i++) 
	{
		if ((token = nextToken()) < 0) return NULL;
		new_image->pixel[i] = (int) fval;
	}
	return new_image;
}

static int parse_use() 
{
	int token;
	struct def_list *search_list;
	if (lookahead < 0) return 0;
	token = nextToken();
	if (sval == NULL) {
		synerr("identifier expected"); return 0;
	}
	for (search_list = defined_names; search_list != NULL; search_list = search_list->next_item) {
		if (!strcmp(search_list->def_name, sval)) {
			current_node.node_type = search_list->node_ref->node_type;
			strcpy(current_node.node_name, search_list->node_ref->node_name);
			current_node.is_instance = 1;
			current_node.fields = NULL; current_node.num_fields = 0;
			current_node.children = get_children_p(1);
			current_node.children[0] = search_list->node_ref;
			current_node.num_children = 1;
			break;
		}
	}
	if (search_list == NULL) {synerr("reference name not found"); return 0;}
	return 1;
}

static int parse_ascii_text() {
	int token;
	char Str[25];
	int err = 0;
	int num;
	char **text_str;
	float *fl;
	float *memo;
	struct vrml_1_fields *ascii_field;

	reset_current_node();
	ascii_field = get_fields(4);
	sprintf(Str, "");
	ascii_field[0].field_name = T_STRING;
	if (!(ascii_field[0].field_value.str_value = (char **) malloc(sizeof(char *)))) err =1;
	else if (!(ascii_field[0].field_value.str_value[0] = strdup(Str))) err = 1;
	ascii_field[0].num_values = 1;
	ascii_field[1].field_name = T_SPACING;
	ascii_field[1].field_value.s_float_value = 1.0;
	ascii_field[1].num_values = 1;
	ascii_field[2].field_name = T_JUSTIFICATION;
	ascii_field[2].field_value.enum_value = T_LEFT;
	ascii_field[2].num_values = 1;
	ascii_field[3].field_name = T_WIDTH;
	if (!(ascii_field[3].field_value.float_value = (float *)malloc(sizeof(float)))) err = 1;
	else ascii_field[3].field_value.float_value[0] = 0;
	ascii_field[3].num_values = 1;
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch (token) {
			case T_STRING:
				if (!(text_str = parse_MFString(&num))) {
					synerr("AsciiText node with invalid string");
				}
				else {
					free(ascii_field[0].field_value.str_value[0]);
					free(ascii_field[0].field_value.str_value);
					ascii_field[0].field_value.str_value = text_str;
					ascii_field[0].num_values = num;
				}
				break;
			case T_SPACING:
				memo = parse_SFFloat();
				ascii_field[1].field_value.s_float_value = *memo;
				break;
			case T_JUSTIFICATION:
				if (!(ascii_field[2].field_value.enum_value = parse_SFEnum())) {
					synerr("AsciiText node with invalid justification");
					ascii_field[2].field_value.enum_value = T_LEFT;
				}
				break;
			case T_WIDTH:
				if (!(fl = parse_MFFloat(&num))) {
					synerr("AsciiText node with invalid width");
				}
				else {
					free(ascii_field[3].field_value.float_value);
					ascii_field[3].field_value.float_value = fl;
					ascii_field[3].num_values = num;
				}
				break;
			default: synerr("AsciiText node with invalid field"); break;
		}
	}
	current_node.node_type = T_ASCII_TEXT;
	current_node.fields = ascii_field;
	current_node.num_fields = 4;
	return 1;
}

static int parse_cone() {
	int token;
	int count;
	char Str[20];
	float *memo;
	vrml_1_fields *cone_field;

	reset_current_node();
	cone_field = get_fields(3);
	cone_field[0].field_name = T_PARTS;
	if ((cone_field[0].field_value.bitmask_value = (int *) malloc(sizeof(int))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	cone_field[0].field_value.bitmask_value[0] = T_ALL;
 	cone_field[0].num_values = 1;
	cone_field[1].field_name = T_BOTTOM_RADIUS;
	cone_field[1].field_value.s_float_value = 1;
	cone_field[1].num_values = 1;
	cone_field[2].field_name = T_HEIGHT;
	cone_field[2].field_value.s_float_value = 2;
	cone_field[2].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch (token) {
			case T_PARTS:
				if (!(cone_field[0].field_value.bitmask_value = parse_SFBitMask(&count)))
					return 0;
				cone_field[0].num_values = count;
				break;
			case T_BOTTOM_RADIUS:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				cone_field[1].field_value.s_float_value = *memo;
				break;
			case T_HEIGHT:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				cone_field[2].field_value.s_float_value = *memo;
				break;
		}
	}
	current_node.node_type = T_CONE;
	current_node.fields = cone_field;
	current_node.num_fields = 3;
	return 1;
}

static int parse_coordinate3() 
{
	int token;
	int err = 0;
	float **memo;
	int count;
	char Str[20];
	vrml_1_fields * coord_field;

	reset_current_node();
	coord_field = get_fields(1);
	coord_field[0].field_name = T_POINT;
	//if ((coord_field[0].field_value.matrix_value = (float **) malloc(sizeof(float *))) == NULL) err = 1;
	//else 
//	{
		//if ((coord_field[0].field_value.matrix_value[0] = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
		//else 
		//{
		//	coord_field[0].field_value.matrix_value[0][0] = coord_field[0].field_value.matrix_value [0][1] = 0;
		//	coord_field[0].field_value.matrix_value[0][2] = 0;
		//}
	//}
	coord_field[0].num_values = 1;
	while ((char)lookahead != '}') 
	{
		if (0 > (token = nextToken())) return 0;
		switch (token) 
		{
		  case T_POINT:
			if ((memo = parse_MFVec3f(&count)) != NULL) 
			{
			  //free(coord_field[0].field_value.matrix_value[0]);
			  //free(coord_field[0].field_value.matrix_value);
			 // coord_field[0].field_value.matrix_value = memo;
			  coord_field[0].num_values = count*3;
			  coord_field[0].field_value.float_value = new float[count*3];
			  int end = (int)g_fvalues.size();
			  for(int k=0;k<end;k++)
			  coord_field[0].field_value.float_value[k] = g_fvalues[k];
			  g_fvalues.clear();

			}
			break;
		}
	}
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	current_node.node_type = T_COORDINATE3;
	current_node.fields = coord_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_cube() {
	int token;
	float *memo;
	vrml_1_fields *cube_field;

	reset_current_node();
	cube_field = get_fields(3);
	cube_field[0].field_name = T_WIDTH;
	cube_field[0].field_value.s_float_value = 2; cube_field[0].num_values = 1;
	cube_field[1].field_name = T_HEIGHT;
	cube_field[1].field_value.s_float_value = 2; cube_field[1].num_values = 1;
	cube_field[2].field_name = T_DEPTH;
	cube_field[2].field_value.s_float_value = 2; cube_field[2].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch (token) {
			case T_WIDTH:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				cube_field[0].field_value.s_float_value = *memo;
				break;
			case T_HEIGHT:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				cube_field[1].field_value.s_float_value = *memo;
				break;
			case T_DEPTH:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				cube_field[2].field_value.s_float_value = *memo;
				break;
		}
	}
	current_node.node_type = T_CUBE;
	current_node.fields = cube_field;
	current_node.num_fields = 3;
	return 1;
}

static int parse_cylinder() {
	int token;
	int *memo;
	int count=0;
	int err = 0;
	float *memo_f;
	char Str[20];
	vrml_1_fields *cyl_field;

	reset_current_node();
	cyl_field = get_fields(3);
	cyl_field[0].field_name = T_PARTS;
	if ((cyl_field[0].field_value.bitmask_value = (int *) malloc(sizeof(int))) == NULL) err = 1;
	else cyl_field[0].field_value.bitmask_value[0] = T_ALL;
	cyl_field[0].num_values = 1;
	cyl_field[1].field_name = T_RADIUS;
	cyl_field[1].field_value.s_float_value = 1; cyl_field[1].num_values = 1;
	cyl_field[2].field_name = T_HEIGHT;
	cyl_field[2].field_value.s_float_value = 2; cyl_field[2].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch (token) {
			case T_PARTS:
				if ((memo = parse_SFBitMask(&count)) == NULL) return 0;
				free(cyl_field[0].field_value.bitmask_value);
				cyl_field[0].field_value.bitmask_value = memo;
				cyl_field[0].num_values = count;
				break;
			case T_RADIUS:
				if ((memo_f = parse_SFFloat()) == NULL) return 0;
				cyl_field[1].field_value.s_float_value = *memo_f;
				break;
			case T_HEIGHT:
				if ((memo_f = parse_SFFloat()) == NULL) return 0;
				cyl_field[2].field_value.s_float_value = *memo_f;
				break;
		}
	}
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	current_node.node_type = T_CYLINDER;
	current_node.fields = cyl_field;
	current_node.num_fields = 3;
	return 1;
}

static int parse_directional_light() {
	int token;
	int *memo;
	float *memo_f;
	int err = 0;
	char Str[20];
	vrml_1_fields *dlight_field;

	reset_current_node();
	dlight_field = get_fields(4);
	dlight_field[0].field_name = T_ON;
	dlight_field[0].field_value.boolean_value = 1; dlight_field[0].num_values = 1;
	dlight_field[1].field_name = T_INTENSITY;
	dlight_field[1].field_value.s_float_value = 1.0; dlight_field[1].num_values = 1;
	dlight_field[2].field_name = T_COLOR;
	if ((dlight_field[2].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
	else {
		dlight_field[2].field_value.float_value[0] = dlight_field[2].field_value.float_value[1] = 1.0;
		dlight_field[2].field_value.float_value[2] = 1;
	}
	dlight_field[2].num_values = 3;
	dlight_field[3].field_name = T_DIRECTION;
	if ((dlight_field[3].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
	else {
		dlight_field[3].field_value.float_value[0] = dlight_field[3].field_value.float_value[1] = 0.0;
		dlight_field[3].field_value.float_value[2] = -1;
	}
	dlight_field[3].num_values = 3;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch (token) {
			case T_ON:
				if ((memo = parse_SFBool()) == NULL) return 0;
				dlight_field[0].field_value.boolean_value = *memo;
				break;
			case T_INTENSITY:
				if ((memo_f = parse_SFFloat()) == NULL) return 0;
				dlight_field[1].field_value.s_float_value = *memo_f;
				break;
			case T_COLOR:
				free(dlight_field[2].field_value.float_value);
				if ((memo_f = parse_SFColor()) == NULL) return 0;
				dlight_field[2].field_value.float_value = memo_f;
				break;
			case T_DIRECTION:
				free(dlight_field[3].field_value.float_value);
				if ((memo_f = parse_SFVec3f()) == NULL) return 0;
				dlight_field[3].field_value.float_value = memo_f;
				break;
		}
	}
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	current_node.node_type = T_DIRECTIONAL_LIGHT;
	current_node.fields = dlight_field;
	current_node.num_fields = 4;
	return 1;
}

static int parse_font_style() {
	int token;
	int *memo;
	float *memo_f;
	int count;
	char Str[20];
	vrml_1_fields *fstyle_field;

	reset_current_node();
	fstyle_field = get_fields(3);
	fstyle_field[0].field_name = T_SIZE;
	fstyle_field[0].field_value.s_float_value = 10.0; fstyle_field[0].num_values = 1;
	fstyle_field[1].field_name = T_FAMILY;
	fstyle_field[1].field_value.enum_value = T_SERIF; fstyle_field[1].num_values = 1;
	fstyle_field[2].field_name = T_STYLE;
	if ((fstyle_field[2].field_value.bitmask_value = (int *) malloc(sizeof(int))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	fstyle_field[2].field_value.bitmask_value[0] = T_NONE;
	fstyle_field[2].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch (token) {
			case T_SIZE:
				if ((memo_f = parse_SFFloat()) == NULL) return 0;
				fstyle_field[0].field_value.s_float_value = *memo_f;
				break;
			case T_FAMILY:
				fstyle_field[1].field_value.enum_value = parse_SFEnum();
				break;
			case T_STYLE:
				free (fstyle_field[2].field_value.bitmask_value);
				if ((memo = parse_SFBitMask(&count)) == NULL) return 0;
				fstyle_field[2].field_value.bitmask_value = memo;
				fstyle_field[2].num_values = count;
				break;
		}
	}
	current_node.node_type = T_FONT_STYLE;
	current_node.fields = fstyle_field;
	current_node.num_fields = 3;
	return 1;
}

static int parse_indexed_face_set() 
{
	int token;
	int i;
	int *memo;
	int count;
	char Str[20];
	vrml_1_fields *face_field;

	reset_current_node();
	face_field = get_fields(4);
	face_field[0].field_name = T_COORD_INDEX;
	face_field[1].field_name = T_MATERIAL_INDEX;
	face_field[2].field_name = T_NORMAL_INDEX;
	face_field[3].field_name = T_TEXTURE_COORD_INDEX;
	for (i = 0; i < 4; i++) 
	{
		if ((face_field[i].field_value.int_value = (int *) malloc(sizeof(int))) == NULL) {
			sprintf(Str, "error in malloc");
			exit(-1);
		}
		face_field[i].field_value.int_value[0] = -1;
		face_field[i].num_values = 1;
	}
	face_field[0].field_value.int_value[0] = 0;
	while ((char)lookahead != '}') 
	{
		if (0 > (token = nextToken())) return 0;
		if (token == T_COORD_INDEX) i = 0;
		else if (token == T_MATERIAL_INDEX) i = 1;
		else if (token == T_NORMAL_INDEX) i = 2;
		else if (token == T_TEXTURE_COORD_INDEX) i = 3;
		else continue;
		parse_MFLong(&count);
		{
		  free(face_field[i].field_value.int_value);
		  face_field[i].num_values = (int)g_ivalues.size()*3/4;
		
		  face_field[i].field_value.int_value = new int[(int)g_ivalues.size()*3/4];

		  int end = (int)g_ivalues.size();
		
		  int index=0;
		  for(int k=0;k<end;k++)
		  {
			if((k+1)%4==0)continue;  
			face_field[i].field_value.int_value[index++] = g_ivalues[k]; 
		  }
		  g_ivalues.clear();
		
		}
	}
	current_node.node_type = T_INDEXED_FACESET;
	current_node.fields = face_field;
	current_node.num_fields = 4;
	return 1;
}

static int parse_indexed_line_set() {
	int token;
	int *memo;
	int i;
	int count;
	char Str[20];
	vrml_1_fields *line_field;

	reset_current_node();
	line_field = get_fields(4);
	line_field[0].field_name = T_COORD_INDEX;
	line_field[1].field_name = T_MATERIAL_INDEX;
	line_field[2].field_name = T_NORMAL_INDEX;
	line_field[3].field_name = T_TEXTURE_COORD_INDEX;
	for (i = 0; i < 4; i++) {
		if ((line_field[i].field_value.int_value = (int *) malloc(sizeof(int))) == NULL) {
			sprintf(Str, "error in malloc");
			exit(-1);
		}
		line_field[i].field_value.int_value[0] = -1;
		line_field[i].num_values = 1;
	}
	line_field[0].field_value.int_value[0] = 0;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_COORD_INDEX) i = 0;
		else if (token == T_MATERIAL_INDEX) i = 1;
		else if (token == T_NORMAL_INDEX) i = 2;
		else if (token == T_TEXTURE_COORD_INDEX) i = 3;
		else continue;
		free(line_field[i].field_value.int_value);
		if ((memo = parse_MFLong(&count)) == NULL) return 0;
		line_field[i].field_value.int_value = memo;
		line_field[i].num_values = count;
	}
	current_node.node_type = T_INDEXED_LINE_SET;
	current_node.fields = line_field;
	current_node.num_fields = 4;
	return 1;
}

static int parse_info() {
	int token;
	vrml_1_fields *info_field;
	int marker = 0;

	reset_current_node();
	info_field = get_fields(1);
	info_field[0].field_name = T_STRING; info_field[0].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_STRING && marker == 0) {
			if ((info_field[0].field_value.str_value = parse_SFString()) == NULL) return 0;
			marker = 1;
		}
	}
	if (marker == 0) return 0;
	current_node.node_type = T_INFO;
	current_node.fields = info_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_lod(int level) {
	int i;
	int err = 0;
	int count;
	int child = 0;
	char Str[28];
	void *reloc;
	vrml_1_fields *lod_field;
	vrml_1_node *lod_node;

	reset_current_node();
	lod_field = get_fields(2);
	lod_field[0].field_name = T_RANGE; lod_field[1].field_name = T_CENTER;
	lod_field[0].field_value.float_value = NULL; lod_field[0].num_values = 0;
	if ((lod_field[1].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	for (i = 0; i < 3; i++) {lod_field[1].field_value.float_value[i] = 0;}
	lod_field[1].num_values = 3;
	while ((char)lookahead != '}') {
		switch(lookahead) {
			case T_RANGE:
				if ((lod_field[0].field_value.float_value = parse_MFFloat(&count)) == NULL) return 0;
				lod_field[0].num_values = count;
				break;
			case T_CENTER:
				free(lod_field[1].field_value.float_value);
				if ((lod_field[1].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
			default:
				if (child == 0) {
					child++;
					if ((current_node.children = (vrml_1_node **) malloc(sizeof(vrml_1_node *))) == NULL) err = 1;
				}
				else {
					child++;
					if ((reloc = realloc(current_node.children, child*sizeof(vrml_1_node *))) == NULL) err = 1;
					else current_node.children = (vrml_1_node **) reloc;
				}
				if (err) {
					sprintf(Str, "error in malloc / realloc");
					exit(-1);
				}
				current_node.num_children = child;
				push_node_stack();
				if (( lod_node = parse_node(level+1)) == NULL) {
					pop_node_stack();
					current_node.num_children = 0;
					free(current_node.children);
					return 0;
				}
				pop_node_stack();
				current_node.children[child-1] = lod_node;
				break;
		}
	}
	current_node.node_type = T_LOD;
	current_node.fields = lod_field;
	current_node.num_fields = 2;
	return 1;
}

static int parse_material() {
	int token;
	int err = 0;
	int i;
	int count;
	char Str[20];
	vrml_1_fields *mat_field;

	reset_current_node();
	mat_field = get_fields(6);
	mat_field[0].field_name = T_AMBCOLOR; mat_field[1].field_name = T_DIFFCOLOR; mat_field[2].field_name = T_SPECCOLOR;
	mat_field[3].field_name = T_EMICOLOR; mat_field[4].field_name = T_SHININESS; mat_field[5].field_name = T_TRANSPARENCY;
	for (i = 0; i < 4; i++) {
		if ((mat_field[i].field_value.matrix_value = (float **) malloc(sizeof(float *))) == NULL) err = 1;
		else if ((mat_field[i].field_value.matrix_value[0] = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
		else {
			mat_field[i].field_value.matrix_value[0][0] = mat_field[i].field_value.matrix_value[0][1] = mat_field[i].field_value.matrix_value[0][2] = 0;
			mat_field[i].num_values = 1;
		}
	}
	for (i = 4; i < 6; i++) {
		if ((mat_field[i].field_value.float_value = (float *) malloc(sizeof(float))) == NULL) err = 1;
		else mat_field[i].num_values = 1;
	}
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	mat_field[0].field_value.matrix_value[0][0] = mat_field[0].field_value.matrix_value[0][1] = mat_field[0].field_value.matrix_value[0][2] = 0.2f;
	mat_field[1].field_value.matrix_value[0][0] = mat_field[1].field_value.matrix_value[0][1] = mat_field[1].field_value.matrix_value[0][2] = 0.8f;
	mat_field[4].field_value.float_value[0] = 0.2f;
	mat_field[5].field_value.float_value[0] = 0;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch (token) {
			case T_AMBCOLOR:
				free(mat_field[0].field_value.matrix_value[0]);
				free(mat_field[0].field_value.matrix_value);
				if ((mat_field[0].field_value.matrix_value = parse_MFColor(&count)) == NULL) return 0;
				mat_field[0].num_values = count;
				break;
			case T_DIFFCOLOR:
				free(mat_field[1].field_value.matrix_value[0]);
				free(mat_field[1].field_value.matrix_value);
				if ((mat_field[1].field_value.matrix_value = parse_MFColor(&count)) == NULL) return 0;
				mat_field[1].num_values = count;
				break;
			case T_SPECCOLOR:
				free(mat_field[2].field_value.matrix_value[0]);
				free(mat_field[2].field_value.matrix_value);
				if ((mat_field[2].field_value.matrix_value = parse_MFColor(&count)) == NULL) return 0;
				mat_field[2].num_values = count;
				break;
			case T_EMICOLOR:
				free(mat_field[3].field_value.matrix_value[0]);
				free(mat_field[3].field_value.matrix_value);
				if ((mat_field[3].field_value.matrix_value = parse_MFColor(&count)) == NULL) return 0;
				mat_field[3].num_values = count;
				break;
			case T_SHININESS:
				free(mat_field[4].field_value.float_value);
				if ((mat_field[4].field_value.float_value = parse_MFFloat(&count)) == NULL) return 0;
				mat_field[4].num_values = count;
				break;
			case T_TRANSPARENCY:
				free(mat_field[5].field_value.float_value);
				if ((mat_field[5].field_value.float_value = parse_MFFloat(&count)) == NULL) return 0;
				mat_field[5].num_values = count;
				break;
		}
	}
	current_node.node_type = T_MATERIAL;
	current_node.fields = mat_field;
	current_node.num_fields = 6;
	return 1;
}

static int parse_material_binding() {
	int token;
	int memo;
	vrml_1_fields *matbind_field;

	reset_current_node();
	matbind_field = get_fields(1);
	matbind_field[0].field_name = T_MATERIAL_BINDING;
	matbind_field[0].field_value.enum_value = T_OVERALL;
	matbind_field[0].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_VALUE) {
			memo  = parse_SFEnum();
			if (memo == T_PER_VERTEX_INDEXED || memo >= T_DEFAULT || memo <= T_PER_VERTEX) {
				matbind_field[0].field_value.enum_value = memo;
			}
		}
	}
	current_node.node_type = T_MATERIAL_BINDING;
	current_node.fields = matbind_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_matrix_transform() {
	int token;
	int i, j;
	int err = 0;
	char Str[20];
	vrml_1_fields *mattrans_field;

	reset_current_node();
	mattrans_field = get_fields(1);
	mattrans_field[0].field_name = T_MATRIX;
	if ((mattrans_field[0].field_value.matrix_value = (float **) malloc(4*sizeof(float *))) == NULL) err = 1;
	else {
		for (i = 0; i < 4; i++) {
			if ((mattrans_field[0].field_value.matrix_value[i] = (float *) malloc(4*sizeof(float))) == NULL) {err = 1; break;}
			for (j = 0; j < 4; j++) {mattrans_field[0].field_value.matrix_value[i][j] = (i == j ? 1.0f : 0.0f);}
		}
	}
	mattrans_field[0].num_values = 4;
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_MATRIX) {
			for (i = 0; i < 4; i++) free(mattrans_field[0].field_value.matrix_value[i]);
			free(mattrans_field[0].field_value.matrix_value);
			if ((mattrans_field[0].field_value.matrix_value = parse_SFMatrix()) == NULL) return 0;
		}
	}
	current_node.node_type = T_MATRIX_TRANSFORM;
	current_node.fields = mattrans_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_normal() 
{
	int token;
	int count;
	vrml_1_fields *normal_field;

	reset_current_node();
	normal_field = get_fields(1);
	normal_field[0].field_name = T_VECTOR;
	normal_field[0].field_value.matrix_value = NULL; normal_field[0].num_values = 0;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_VECTOR) {
			if ((normal_field[0].field_value.matrix_value = parse_MFVec3f(&count)) == NULL) return 0;
			normal_field[0].num_values = count;
		}
	}
	current_node.node_type = T_NORMAL;
	current_node.fields = normal_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_normal_binding() {
	int token;
	vrml_1_fields *norbi_field;

	reset_current_node();
	norbi_field = get_fields(1);
	norbi_field[0].field_name = T_VALUE;
	norbi_field[0].field_value.enum_value = T_DEFAULT; norbi_field[0].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_VALUE) {
			norbi_field[0].field_value.enum_value = parse_SFEnum();
		}
	}
	current_node.node_type = T_NORMAL_BINDING;
	current_node.fields = norbi_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_orthographic_camera() {
	int token;
	float *memo;
	int err = 0;
	char Str[20];
	vrml_1_fields *ocam_field;

	reset_current_node();
	ocam_field = get_fields(4);
	ocam_field[0].field_name = T_POSITION; ocam_field[1].field_name = T_ORIENTATION;
	ocam_field[2].field_name = T_FOCAL_DISTANCE; ocam_field[3].field_name = T_HEIGHT;
	if ((ocam_field[0].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
	else {
		ocam_field[0].field_value.float_value[0] = ocam_field[0].field_value.float_value[1] = 0;
		ocam_field[0].field_value.float_value[2] = 1;
	}
	if ((ocam_field[1].field_value.float_value = (float *) malloc(4*sizeof(float))) == NULL) err = 1;
	else {
		ocam_field[1].field_value.float_value[0] = ocam_field[1].field_value.float_value[1] = ocam_field[1].field_value.float_value[3] = 0;
		ocam_field[1].field_value.float_value[2] = 1;
	}
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	ocam_field[2].field_value.s_float_value = 5; ocam_field[3].field_value.s_float_value = 2;
	ocam_field[2].num_values = ocam_field[3].num_values = 1;
	ocam_field[0].num_values = 3; ocam_field[1].num_values = 4;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch(token) {
			case T_POSITION:
				free(ocam_field[0].field_value.float_value);
				if ((ocam_field[0].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
			case T_ORIENTATION:
				free(ocam_field[1].field_value.float_value);
				if ((ocam_field[1].field_value.float_value = parse_SFRotation()) == NULL) return 0;
				break;
			case T_FOCAL_DISTANCE:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				ocam_field[2].field_value.s_float_value = *memo;
				break;
			case T_HEIGHT:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				ocam_field[3].field_value.s_float_value = *memo;
				break;
		}
	}
	current_node.node_type = T_ORTHOGRAPHIC_CAMERA;
	current_node.fields = ocam_field;
	current_node.num_fields = 4;
	return 1;
}

static int parse_perspective_camera() {
	int token;
	int err = 0;
	float *memo;
	char Str[20];
	vrml_1_fields *pcam_field;

	reset_current_node();
	pcam_field = get_fields(4);
	pcam_field[0].field_name = T_POSITION; pcam_field[1].field_name = T_ORIENTATION;
	pcam_field[2].field_name = T_FOCAL_DISTANCE; pcam_field[3].field_name = T_HEIGHT_ANGLE;
	if ((pcam_field[0].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
	else {
		pcam_field[0].field_value.float_value[0] = pcam_field[0].field_value.float_value[1] = 0;
		pcam_field[0].field_value.float_value[2] = 1;
	}
	if ((pcam_field[1].field_value.float_value = (float *) malloc(4*sizeof(float))) == NULL) err = 1;
	else {
		pcam_field[1].field_value.float_value[0] = pcam_field[1].field_value.float_value[1] = 0;
		pcam_field[1].field_value.float_value[2] = 1; pcam_field[1].field_value.float_value[3] = 0;
	}
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	pcam_field[0].num_values = 3; pcam_field[1].num_values = 4;
	pcam_field[2].field_value.s_float_value = 5;
	pcam_field[3].field_value.s_float_value = 0.785398f;
	pcam_field[2].num_values = pcam_field[3].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch(token) {
			case T_POSITION:
				free(pcam_field[0].field_value.float_value);
				if ((pcam_field[0].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
			case T_ORIENTATION:
				free(pcam_field[1].field_value.float_value);
				if ((pcam_field[1].field_value.float_value = parse_SFRotation()) == NULL) return 0;
				break;
			case T_FOCAL_DISTANCE:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				pcam_field[2].field_value.s_float_value = *memo;
				break;
			case T_HEIGHT_ANGLE:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				pcam_field[3].field_value.s_float_value = *memo;
				break;
		}
	}
	current_node.node_type = T_PERSPECTIVE_CAMERA;
	current_node.fields = pcam_field;
	current_node.num_fields = 4;
	return 1;
}

static int parse_point_set() {
	int token;
	int *memo;
	vrml_1_fields *pset_field;

	reset_current_node();
	pset_field = get_fields(2);
	pset_field[0].field_name = T_START_INDEX; pset_field[1].field_name = T_NUM_POINTS;
	pset_field[0].field_value.s_int_value = 0;
	pset_field[1].field_value.s_int_value = -1;
	pset_field[0].num_values = pset_field[1].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch(token) {
			case T_START_INDEX:
				if ((memo = parse_SFLong()) == NULL) return 0;
				pset_field[0].field_value.s_int_value = *memo;
				break;
			case T_NUM_POINTS:
				if ((memo = parse_SFLong()) == NULL) return 0;
				pset_field[1].field_value.s_int_value = *memo;
				break;
		}
	}
	current_node.node_type = T_POINT_SET;
	current_node.fields = pset_field;
	current_node.num_fields = 2;
	return 1;
}

static int parse_rotation() {
	int token;
	char Str[20];
	vrml_1_fields *rot_field;

	reset_current_node();
	rot_field = get_fields(1);
	rot_field[0].field_name = T_F_ROTATION;
	if ((rot_field[0].field_value.float_value = (float *) malloc(4*sizeof(float))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	rot_field[0].field_value.float_value[0] = rot_field[0].field_value.float_value[1] = 0;
	rot_field[0].field_value.float_value[2] = 1; rot_field[0].field_value.float_value[3] = 0;
	rot_field[0].num_values = 4;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_F_ROTATION) {
			free(rot_field[0].field_value.float_value);
			if ((rot_field[0].field_value.float_value = parse_SFRotation()) == NULL) return 0;
		}
	}
	current_node.node_type = T_ROTATION;
	current_node.fields = rot_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_scale() {
	int token;
	char Str[20];
	vrml_1_fields *scale_field;

	reset_current_node();
	scale_field = get_fields(1);
	scale_field[0].field_name = T_SCALE_FACTOR;
	if ((scale_field[0].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) {
		sprintf(Str, "error in malloc");

		exit(-1);
	}
	scale_field[0].field_value.float_value[0] = scale_field[0].field_value.float_value[1] = 1;
	scale_field[0].field_value.float_value[2] = 1; scale_field[0].num_values = 3;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_SCALE_FACTOR) {
			free(scale_field[0].field_value.float_value);
			if ((scale_field[0].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
		}
	}
	current_node.node_type = T_SCALE;
	current_node.fields = scale_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_separator(int level) 
{
	void *reloc;
	int child = 0;
	int err = 0;
	char Str[20];
	vrml_1_fields *sep_field;
	vrml_1_node *mem_node;

	reset_current_node();
	sep_field = get_fields(1);
	sep_field[0].field_name = T_RENDER_CULLING;
	sep_field[0].field_value.enum_value = T_AUTO;
	sep_field[0].num_values = 1;
	while ((char)lookahead != '}') 
	{
		switch(lookahead) 
		{
			case T_RENDER_CULLING:
				sep_field[0].field_value.enum_value = parse_SFEnum();
				break;
			default:
				if (child == 0) 
				{
					child++;
					if ((current_node.children = (vrml_1_node **) malloc(sizeof(vrml_1_node *))) == NULL) err = 1;
				}
				else 
				{
					child++;
					if ((reloc = realloc(current_node.children, child*sizeof(vrml_1_node *))) == NULL) err = 1;
					current_node.children = (vrml_1_node **) reloc;
				}
				if (err) {
					sprintf(Str, "error in malloc");
					exit(-1);
				}
				current_node.num_children = child;
				push_node_stack();
				if ((mem_node = parse_node(level+1)) == NULL) 
				{
					pop_node_stack();
					free(current_node.children);
					current_node.num_children = 0;
					return 0;
				}
				pop_node_stack();
				current_node.children[child-1] = mem_node;
				break;
		}
	}
	current_node.node_type = T_SEPARATOR;
	current_node.fields = sep_field;
	current_node.num_fields = 1;
	return 1;
}
static int parse_shape_hints() {
	int token;
	float *memo;
	vrml_1_fields *shints_field;

	reset_current_node();
	shints_field = get_fields(4);
	shints_field[0].field_name = T_VERTEX_ORDER; shints_field[1].field_name = T_SHAPE_TYPE;
	shints_field[2].field_name = T_FACE_TYPE; shints_field[3].field_name = T_CREASE_ANGLE;
	shints_field[0].field_value.enum_value = T_UNK_ORDERING; shints_field[1].field_value.enum_value = T_UNK_SHAPE;
	shints_field[2].field_value.enum_value = T_V_CONVEX; shints_field[3].field_value.s_float_value = 0.5;
	shints_field[0].num_values = shints_field[1].num_values = shints_field[2].num_values = shints_field[3].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch(token) {
			case T_VERTEX_ORDER:
				shints_field[0].field_value.enum_value = parse_SFEnum();
				break;
			case T_SHAPE_TYPE:
				shints_field[1].field_value.enum_value = parse_SFEnum();
				break;
			case T_FACE_TYPE:
				shints_field[2].field_value.enum_value = parse_SFEnum();
				break;
			case T_CREASE_ANGLE:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				shints_field[3].field_value.s_float_value = *memo;
				break;
		}
	}
	current_node.node_type = T_SHAPE_HINTS;
	current_node.fields = shints_field;
	current_node.num_fields = 4;
	return 1;
}

static int parse_sphere() {
	int token;
	float *memo;
	vrml_1_fields *sphere_field;

	reset_current_node();
	sphere_field = get_fields(1);
	sphere_field[0].field_name = T_RADIUS;
	sphere_field[0].field_value.s_float_value = 1; sphere_field[0].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_RADIUS) {
			if ((memo = parse_SFFloat()) == NULL) return 0;
			sphere_field[0].field_value.s_float_value = *memo;
		}
	}
	current_node.node_type = T_SPHERE;
	current_node.fields = sphere_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_spot_light() {
	int token;
	int *memo;
	float *memo_f;
	int err = 0;
	char Str[20];
	vrml_1_fields *slight_field;

	reset_current_node();
	slight_field = get_fields(7);
	slight_field[0].field_name = T_ON; slight_field[1].field_name = T_INTENSITY; slight_field[2].field_name = T_COLOR;
	slight_field[3].field_name = T_LOCATION; slight_field[4].field_name = T_DIRECTION;
	slight_field[5].field_name = T_DROP_OFF_RATE; slight_field[6].field_name = T_CUT_OFF_ANGLE;
	if ((slight_field[2].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
	if ((slight_field[3].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
	if ((slight_field[4].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	slight_field[0].field_value.boolean_value = 1; slight_field[1].field_value.s_float_value = 1;
	slight_field[2].field_value.float_value[0] = slight_field[2].field_value.float_value[1] = slight_field[2].field_value.float_value[2] = 1;
	slight_field[3].field_value.float_value[0] = slight_field[3].field_value.float_value[1] = 0;
	slight_field[3].field_value.float_value[2] = 1;
	slight_field[4].field_value.float_value[0] = slight_field[4].field_value.float_value[1] = 0;
	slight_field[4].field_value.float_value[2] = -1;
	slight_field[5].field_value.s_float_value = 0; slight_field[5].field_value.s_float_value = 0.785398f;
	slight_field[0].num_values = slight_field[1].num_values = slight_field[5].num_values = slight_field[6].num_values = 1;
	slight_field[2].num_values = slight_field[3].num_values = slight_field[4].num_values = 3;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch(token) {
			case T_ON:
				if ((memo = parse_SFBool()) == NULL) return 0;
				slight_field[0].field_value.boolean_value = *memo;
				break;
			case T_INTENSITY:
				if ((memo_f = parse_SFFloat()) == NULL) return 0;
				slight_field[1].field_value.s_float_value = *memo_f;
				break;
			case T_COLOR:
				free(slight_field[2].field_value.float_value);
				if ((slight_field[2].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
			case T_LOCATION:
				free(slight_field[3].field_value.float_value);
				if ((slight_field[3].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
			case T_DIRECTION:
				free(slight_field[4].field_value.float_value);
				if ((slight_field[4].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
			case T_DROP_OFF_RATE:
				if ((memo_f = parse_SFFloat()) == NULL) return 0;
				slight_field[5].field_value.s_float_value = *memo_f;
				break;
			case T_CUT_OFF_ANGLE:
				if ((memo_f = parse_SFFloat()) == NULL) return 0;
				slight_field[6].field_value.s_float_value = *memo_f;
				break;
		}
	}
	current_node.node_type = T_SPOT_LIGHT;
	current_node.fields = slight_field;
	current_node.num_fields = 7;
	return 1;
}

static int parse_switch(int level) {
	int child = 0;
	int *memo;
	void *reloc;
	int err = 0;
	char Str[20];
	vrml_1_fields *switch_field;
	vrml_1_node *switch_node;

	reset_current_node();
	switch_field = get_fields(1);
	switch_field[0].field_name = T_WHICH_CHILD;
	switch_field[0].field_value.s_int_value = -1;
	switch_field[0].num_values = 1;
	while ((char)lookahead != '}') {
		switch(lookahead) {
			case T_WHICH_CHILD:
				if ((memo = parse_SFLong()) == NULL)
					if (lookahead == T_NUMBER) {
						if ((memo = parse_SFLong()) == NULL)  return 0;
					} else return 0;
				switch_field[0].field_value.s_int_value = *memo;
				break;
			default:
				if (child == 0) {
					child++;
					if ((current_node.children = (vrml_1_node **) malloc(sizeof(vrml_1_node *))) == NULL) err = 1;
				}
				else {
					child++;
					if ((reloc = realloc(current_node.children, child*sizeof(vrml_1_node *))) == NULL) err = 1;
					else current_node.children = (vrml_1_node **) reloc;
				}
				if (err) {
					sprintf(Str, "error in malloc");
					exit(-1);
				}
				current_node.num_children = child;
				push_node_stack();
				if ((switch_node = parse_node(level+1)) == NULL) {
					pop_node_stack();
					current_node.num_children = 0;
					free(current_node.children);
					return 0;
				}
				pop_node_stack();
				current_node.children[child-1] = switch_node;
				break;
		}
	}
	current_node.node_type = T_SWITCH;
	current_node.fields = switch_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_texture2() {
	int token;
	int err = 0;
	char Str[20];
	vrml_1_fields *tex_field;

	reset_current_node();
	tex_field = get_fields(4);
	tex_field[0].field_name = T_FILE_NAME; tex_field[1].field_name = T_IMAGE;
	tex_field[2].field_name = T_WRAP_S; tex_field[3].field_name = T_WRAP_T;
	if ((tex_field[0].field_value.str_value = (char **) malloc(sizeof(char *))) == NULL) err = 1;
	tex_field[0].field_value.str_value[0] = strdup("");
	if ((tex_field[1].field_value.image_value = (CONVRMLSFImage *) malloc(sizeof(CONVRMLSFImage))) == NULL) err = 1;
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	tex_field[2].field_value.enum_value = tex_field[3].field_value.enum_value = T_REPEAT;
	tex_field[0].num_values = tex_field[1].num_values = 1;
	tex_field[2].num_values = tex_field[3].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch(token) {
			case T_FILE_NAME:
				free(tex_field[0].field_value.str_value[0]);
				free(tex_field[0].field_value.str_value);
				if ((tex_field[0].field_value.str_value = parse_SFString()) == NULL) return 0;
				break;
			case T_IMAGE:
				free(tex_field[1].field_value.image_value);
				if ((tex_field[1].field_value.image_value = parse_SFImage()) == NULL) return 0;
				break;
			case T_WRAP_S:
				tex_field[2].field_value.enum_value = parse_SFEnum();
				break;
			case T_WRAP_T:
				tex_field[3].field_value.enum_value = parse_SFEnum();
				break;
		}
	}
	current_node.node_type = T_TEXTURE_2;
	current_node.fields = tex_field;
	current_node.num_fields = 4;
	return 1;
}

static int parse_texture_2_transform() {
	int token;
	int err = 0;
	float *memo;
	char Str[20];
	vrml_1_fields *textr_field;

	reset_current_node();
	textr_field = get_fields(4);
	textr_field[0].field_name = T_F_TRANSLATION; textr_field[1].field_name = T_F_ROTATION;
	textr_field[2].field_name = T_SCALE_FACTOR; textr_field[3].field_name = T_CENTER;
	if ((textr_field[0].field_value.float_value = (float *) malloc(2*sizeof(float))) == NULL) err = 1;
	if ((textr_field[2].field_value.float_value = (float *) malloc(2*sizeof(float))) == NULL) err = 1;
	if ((textr_field[3].field_value.float_value = (float *) malloc(2*sizeof(float))) == NULL) err = 1;
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	textr_field[0].field_value.float_value[0] = textr_field[0].field_value.float_value[1] = 0;
	textr_field[1].field_value.s_float_value = 0;
	textr_field[2].field_value.float_value[0] = textr_field[2].field_value.float_value[1] = 1;
	textr_field[3].field_value.float_value[0] = textr_field[3].field_value.float_value[1] = 0;
	textr_field[0].num_values = textr_field[2].num_values = textr_field[3].num_values = 2;
	textr_field[1].num_values = 1;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch(token) {
			case T_F_TRANSLATION:
				free(textr_field[0].field_value.float_value);
				if ((textr_field[0].field_value.float_value = parse_SFVec2f()) == NULL) return 0;
				break;
			case T_F_ROTATION:
				if ((memo = parse_SFFloat()) == NULL) return 0;
				textr_field[1].field_value.s_float_value = *memo;
				break;
			case T_SCALE_FACTOR:
				free(textr_field[2].field_value.float_value);
				if ((textr_field[2].field_value.float_value = parse_SFVec2f()) == NULL) return 0;
				break;
			case T_CENTER:
				free(textr_field[3].field_value.float_value);
				if ((textr_field[3].field_value.float_value = parse_SFVec2f()) == NULL) return 0;
				break;
		}
	}
	current_node.node_type = T_TEXTURE_2_TRANSFORM;
	current_node.fields = textr_field;
	current_node.num_fields = 4;
	return 1;
}

static int parse_texture_coordinate_2() 
{
	int token;
	//char Str[20];
	vrml_1_fields *texco_field;

#ifdef TEMP_EXLUDE
	reset_current_node();
	texco_field = get_fields(1);
	texco_field[0].field_name = T_POINT; texco_field[0].num_values = 2;
	if ((texco_field[0].field_value.float_value = (float *) malloc(2*sizeof(float))) == NULL) {
		sprintf(Str, "error in malloc");
		SHOW_MESSAGE(Str);
		exit(-1);
	}
	texco_field[0].field_value.float_value[0] = texco_field[0].field_value.float_value[1] = 0;
#endif /* TEMP_EXLUDE */
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_POINT) {
			free(texco_field[0].field_value.float_value);
			if ((texco_field[0].field_value.float_value = parse_SFVec2f()) == NULL) return 0;
		}
	}
#ifdef TEMP_EXLUDE
	current_node.node_type = T_TEXTURE_COORDINATE_2;
	current_node.fields = texco_field;
	current_node.num_fields = 1;
	#endif /* TEMP_EXLUDE */
	return 1;
}

static int parse_transform() {
	int token;
	int err = 0;
	char Str[20];
	vrml_1_fields *trafo_field;

	reset_current_node();
	trafo_field = get_fields(5);
	trafo_field[0].field_name = T_F_TRANSLATION; trafo_field[1].field_name = T_F_ROTATION;
	trafo_field[2].field_name = T_SCALE_FACTOR; trafo_field[3].field_name = T_SCALE_ORIENTATION;
	trafo_field[4].field_name = T_CENTER;
	if ((trafo_field[0].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
	if ((trafo_field[1].field_value.float_value = (float *) malloc(4*sizeof(float))) == NULL) err = 1;
	if ((trafo_field[2].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
	if ((trafo_field[3].field_value.float_value = (float *) malloc(4*sizeof(float))) == NULL) err = 1;
	if ((trafo_field[4].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) err = 1;
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	trafo_field[0].field_value.float_value[0] = trafo_field[0].field_value.float_value[1] = trafo_field[0].field_value.float_value[2] = 0;
	trafo_field[1].field_value.float_value[0] = trafo_field[1].field_value.float_value[1] = trafo_field[1].field_value.float_value[3] = 0;
	trafo_field[1].field_value.float_value[2] = 1;
	trafo_field[2].field_value.float_value[0] = trafo_field[2].field_value.float_value[1] = trafo_field[2].field_value.float_value[2] = 1;
	trafo_field[3].field_value.float_value[0] = trafo_field[3].field_value.float_value[1] = trafo_field[3].field_value.float_value[3] = 0;
	trafo_field[3].field_value.float_value[2] = 1;
	trafo_field[4].field_value.float_value[0] = trafo_field[4].field_value.float_value[1] = trafo_field[4].field_value.float_value[2] = 0;
	trafo_field[0].num_values = trafo_field[2].num_values = trafo_field[4].num_values = 3;
	trafo_field[1].num_values = trafo_field[3].num_values = 4;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch(token) {
			case T_F_TRANSLATION:
				free(trafo_field[0].field_value.float_value);
				if ((trafo_field[0].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
			case T_F_ROTATION:
				free(trafo_field[1].field_value.float_value);
				if ((trafo_field[1].field_value.float_value = parse_SFRotation()) == NULL) return 0;
				break;
			case T_SCALE_FACTOR:
				free(trafo_field[2].field_value.float_value);
				if ((trafo_field[2].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
			case T_SCALE_ORIENTATION:
				free(trafo_field[3].field_value.float_value);
				if ((trafo_field[3].field_value.float_value = parse_SFRotation()) == NULL) return 0;
				break;
			case T_CENTER:
				free(trafo_field[4].field_value.float_value);
				if ((trafo_field[4].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
		}
	}
	current_node.node_type = T_TRANSFORM;
	current_node.fields = trafo_field;
	current_node.num_fields = 5;
	return 1;
}

static int  parse_translation() {
	int token;
	char Str[20];
	vrml_1_fields *trans_field;

	reset_current_node();
	trans_field = get_fields(1);
	trans_field[0].field_name = T_F_TRANSLATION; trans_field[0].num_values = 3;
	if ((trans_field[0].field_value.float_value = (float *) malloc(3*sizeof(float))) == NULL) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	trans_field[0].field_value.float_value[0] = trans_field[0].field_value.float_value[1] = trans_field[0].field_value.float_value[2] = 0;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		if (token == T_F_TRANSLATION) {
			free(trans_field[0].field_value.float_value);
			if ((trans_field[0].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
		}
	}
	current_node.node_type = T_TRANSLATION;
	current_node.fields = trans_field;
	current_node.num_fields = 1;
	return 1;
}

static int parse_www_anchor(int level) {
	int err = 0;
	int child = 0;
	char Str[20];
	void *reloc;
	vrml_1_fields *anchor_field;
	vrml_1_node *anchor_node;

	reset_current_node();
	anchor_field  = get_fields(3);
	anchor_field[0].field_name = T_F_NAME; anchor_field[1].field_name = T_DESCRIPTION;
	anchor_field[2].field_name = T_MAP;
	if ((anchor_field[0].field_value.str_value = (char **) malloc(sizeof(char *))) == NULL) err = 1;
	if ((anchor_field[1].field_value.str_value = (char **) malloc(sizeof(char *))) == NULL) err = 1;
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	anchor_field[0].field_value.str_value[0] = strdup("");
	anchor_field[1].field_value.str_value[0] = strdup("");
	anchor_field[2].field_value.enum_value = T_NONE;
	anchor_field[0].num_values = anchor_field[1].num_values = anchor_field[2].num_values = 1;
	while ((char)lookahead != '}') {
		switch(lookahead) {
			case T_F_NAME:
				free(anchor_field[0].field_value.str_value[0]);
				free(anchor_field[0].field_value.str_value);
				if ((anchor_field[0].field_value.str_value = parse_SFString()) == NULL) return 0;
				break;
			case T_DESCRIPTION:
				free(anchor_field[1].field_value.str_value[0]);
				free(anchor_field[1].field_value.str_value);
				if ((anchor_field[1].field_value.str_value = parse_SFString()) == NULL) return 0;
				break;
			case T_MAP:
				anchor_field[2].field_value.enum_value = parse_SFEnum();
				break;
			default:
				err = 0;
				if (child == 0) {
					child++;
					if ((current_node.children = (vrml_1_node **) malloc(sizeof(vrml_1_node *))) == NULL) err = 1;
				}
				else {
					child++;
					if ((reloc = realloc(current_node.children, child*sizeof(vrml_1_node *))) == NULL) err = 1;
					else current_node.children = (vrml_1_node **) reloc;
				}
				if (err) {
					sprintf(Str, "error in malloc");
					exit(-1);
				}
				current_node.num_children = child;
				push_node_stack();
				if ((anchor_node = parse_node(level+1)) == NULL) {
					pop_node_stack();
					current_node.num_children = 0;
					free(current_node.children);
					return 0;
				}
				pop_node_stack();
				current_node.children[child-1] = anchor_node;
				break;
		}
	}
	current_node.node_type = T_WWW_ANCHOR;
	current_node.fields = anchor_field;
	current_node.num_fields = 3;
	return 1;
}

static int parse_www_inline() 
{
	int token;
	int err = 0;
	char Str[20];
	vrml_1_fields *inl_field;

	reset_current_node();
	inl_field = get_fields(3);
	inl_field[0].field_name = T_F_NAME; inl_field[1].field_name = T_BBOX_SIZE;
	inl_field[2].field_name = T_BBOX_CENTER;
	if ((inl_field[0].field_value.str_value = (char **) malloc(sizeof(char*))) == NULL) err = 1;
	if ((inl_field[1].field_value.float_value = (float *) malloc (3*sizeof(float))) == NULL) err = 1;
	if ((inl_field[2].field_value.float_value = (float *) malloc (3*sizeof(float))) == NULL) err = 1;
	if (err) {
		sprintf(Str, "error in malloc");
		exit(-1);
	}
	inl_field[0].field_value.str_value[0] = strdup("");
	inl_field[1].field_value.float_value[0] = inl_field[1].field_value.float_value[1] = inl_field[1].field_value.float_value[2] = 0;
	inl_field[2].field_value.float_value[0] = inl_field[2].field_value.float_value[1] = inl_field[2].field_value.float_value[2] = 0;
	inl_field[0].num_values = 1; inl_field[1].num_values = inl_field[2].num_values = 3;
	while ((char)lookahead != '}') {
		if (0 > (token = nextToken())) return 0;
		switch(token) {
			case T_F_NAME:
				free(inl_field[0].field_value.str_value[0]);
				free(inl_field[0].field_value.str_value);
				if ((inl_field[0].field_value.str_value = parse_SFString()) == NULL) return 0;
				break;
			case T_BBOX_SIZE:
				free(inl_field[1].field_value.float_value);
				if ((inl_field[1].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
			case T_BBOX_CENTER:
				free(inl_field[2].field_value.float_value);
				if ((inl_field[2].field_value.float_value = parse_SFVec3f()) == NULL) return 0;
				break;
		}
	}
	current_node.node_type = T_WWW_INLINE;
	current_node.fields = inl_field;
	current_node.num_fields = 3;
	return 1;
}



int init_parser(char *fname) 
{
	char Str[20];
	unread_char = 0;
	lineno = 1;
	node_stack = NULL;
	defined_names = NULL;
	scene_graph = NULL;
	if (!(sval = (char *) malloc(sizeof(char)))) 
	{
	 sprintf(Str, "error in malloc");
		exit(-1);
	}
	else sval[0] = '\0';
	b_sval = NULL;
	lookahead = 0;
	filename = strdup(fname);
	reset_current_node();

	if ((in = fopen(fname, "r")) == NULL) 
	{
	  sprintf(varString, "cannot open vrml file %s", fname);
	  vrml_err_str = varString;
	  return 0;
	}

	lookahead = nextToken2();
	
	if(lookahead < 0) 
	{
	  sprintf(varString, "vrml file %s has invalid content", fname);
	  vrml_err_str = varString;
	  return 0;
	}

	b_fval = fval;
	b_sval = strdup(sval);
	return 1;
}
int exit_parser() 
{
	fclose(in);
	return 1;
}

vrml_1_node * parse_node(int level) 
{
	int token;
	int def_seen = 0;
	int use_seen = 0;
	int ok = 1;
	int ready = 0;
	vrml_1_node mem_node;
	struct vrml_1_node *this_node = NULL;
	mem_node.node_name[0] = '\0';
	/*if (level == 0)*/ token = nextToken();
	if (token < 0) return NULL;
	reset_current_node();
	while (ok && token != '}' && token != 0 && use_seen == 0 && ready == 0) 
	{
		switch (token) 
		{
			case T_DEF:
				if (def_seen) {synerr("DEF statement occured more than one time"); return 0;}
				token = nextToken();
				if (token < 0) return NULL;
				if (sval == NULL) {synerr("identifier expected"); return NULL;}
				insert_name_to_node(sval, &mem_node);
				def_seen = 1;
				break;
			case T_USE:  if (!(ok = parse_use())) synerr("\"use\" referenced node not found");
				use_seen = 1; break;
			case T_ASCII_TEXT:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_ascii_text())) synerr("bad syntax in ascii node");
				break;
			case T_CONE:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_cone())) synerr("bad syntax in cone node");
				break;
			case T_COORDINATE3:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_coordinate3())) synerr("bad syntax in coordinate3 node");
				break;
			case T_CUBE:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_cube())) synerr("bad syntax in cube node");
				break;
			case T_CYLINDER:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_cylinder())) synerr("bad syntax in cylinder node");
				break;
			case T_DIRECTIONAL_LIGHT:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_directional_light())) synerr("bad syntax in directional light node");
				break;
			case T_FONT_STYLE:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_font_style())) synerr("bad syntax in font style node");
				break;
			case T_INDEXED_FACESET:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_indexed_face_set())) synerr("bad syntax in indexed faceset node");
				break;
			case T_INDEXED_LINE_SET:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_indexed_line_set())) synerr("bad syntax in indexed lineset node");
				break;
			case T_INFO:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_info())) synerr("bad syntax in info node");
				break;
			case T_LOD:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_lod(level))) synerr("bad syntax in LOD node");
				break;
			case T_MATERIAL:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_material())) synerr("bad syntax in material node");
				break;
			case T_MATERIAL_BINDING:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_material_binding())) synerr("bad syntax in material binding node");
				break;
			case T_MATRIX_TRANSFORM:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_matrix_transform())) synerr("bad syntax in matrix transform node");
				break;
			case T_NORMAL:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_normal())) synerr("bad syntax in normal node");
				break;
			case T_NORMAL_BINDING:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_normal_binding())) synerr("bad syntax in normal binding node");
				break;
			case T_ORTHOGRAPHIC_CAMERA:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_orthographic_camera())) synerr("bad syntax in orthographic camera node");
				break;
			case T_PERSPECTIVE_CAMERA:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_perspective_camera())) synerr("bad syntax in perspective camera node");
				break;
			case T_POINT_SET:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_point_set())) synerr("bad syntax in pointset node");
				break;
			case T_ROTATION:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_rotation())) synerr("bad syntax in rotation node");
				break;
			case T_SCALE:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_scale())) synerr("bad syntax in scale node");
				break;
			case T_SEPARATOR:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_separator(level))) synerr("bad syntax in separator node");
				break;
			case T_SHAPE_HINTS:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_shape_hints())) synerr("bad syntax in shape hints node");
				break;
			case T_SPHERE:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_sphere())) synerr("bad syntax in sphere node");
				break;
			case T_SPOT_LIGHT:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_spot_light())) synerr("bad syntax in spot light node");
				break;
			case T_SWITCH:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_switch(level))) synerr("bad syntax in switch node");
				break;
			case T_TEXTURE_2:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_texture2())) synerr("bad syntax in texture2 node");
				break;
			case T_TEXTURE_2_TRANSFORM:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_texture_2_transform())) synerr("bad syntax in texture2transform node");
				break;
			case T_TEXTURE_COORDINATE_2:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_texture_coordinate_2())) synerr("bad syntax in texture coordinate2 node");
				break;
			case T_TRANSFORM:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_transform())) synerr("bad syntax in transform node");
				break;
			case T_TRANSLATION:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_translation())) synerr("bad syntax in translation node");
				break;
			case T_WWW_ANCHOR:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_www_anchor(level))) synerr("bad syntax in www anchor node");
				break;
			case T_WWW_INLINE:
				if (!expect_token('{')) return NULL;
				if (!(ok = parse_www_inline())) synerr("bad syntax in www inline node");
				break;
		}
		if (use_seen == 0) {
			token = nextToken();
		}
		if (token < 0) return NULL;
		if (!ok) {
			return NULL;
		}
	}
	if (use_seen == 0) {
		insert_name_to_node(mem_node.node_name, &current_node);
	}
	if (ok && level != 0 && token != '}' && use_seen == 0) {
		synerr("'}' expected"); return NULL;
	}
	else if (!ok) {
		synerr("invalid node syntax");
	}
	this_node = new_node_from_current_node();
	if (def_seen) {
		insert_item_to_defined_names(this_node->node_name, this_node);
	}
	return this_node;
}

bool Wrl1Reader::read(string & file_name,BaseImporter& _bi)
{
	char * name = strdup(file_name.c_str());
	
	eof = 0;
	init_parser(name);
	parse_node(0);
	AddNodeInfo(&current_node,_bi);
	//ClearNode(&current_node);
    exit_parser();

    return true;
}
void Wrl1Reader::AddNodeInfo(vrml_1_node * node,BaseImporter& _bi)
{
 if(node==NULL)return; // 返回 如果是空节点


 if(node->node_type == T_COORDINATE3)
 {

  for(int i=0;i<node->num_fields;i++)
  {
	if(node->fields[i].field_name!=T_POINT)continue;

	int end = node->fields[i].num_values/3;
	for(int j=0;j<end;j++)
	{
     Point3f point;
	 point.X() = node->fields[i].field_value.float_value[j*3]; 
	 point.Y() = node->fields[i].field_value.float_value[j*3+1]; 
	 point.Z() = node->fields[i].field_value.float_value[j*3+2]; 
	 _bi.AddVertex(point);
	}
  }
 }

 if(node->node_type == T_INDEXED_FACESET)
 {
  for(int i=0;i<node->num_fields;i++)
  {
   if(node->fields[i].field_name!=T_COORD_INDEX)continue;
  
   int end = node->fields[i].num_values/3;

   for(int j=0;j<end;j++)
   {
     int v0,v1,v2;

	 v0 = node->fields[i].field_value.int_value[j*3];
	 v1 = node->fields[i].field_value.int_value[j*3+1];
     v2 = node->fields[i].field_value.int_value[j*3+2];
	 _bi.AddFace(v0,v1,v2);
   }
  }
 }

 /////////  遍历子节点
 for(int i=0;i<node->num_children;i++)
 {
  AddNodeInfo(node->children[i],_bi);
 }

}
void Wrl1Reader::ClearNode(vrml_1_node * node)
{
  if(node==NULL)return; // 返回 如果是空节点

  for(int i=0;i<node->num_fields;i++)
  {
	if(node->fields[i].field_name==T_POINT)
	{
	  if(node->fields[i].field_value.float_value!=NULL)
	  delete node->fields[i].field_value.float_value;
	  node->fields[i].field_value.float_value=NULL;
	}
	if(node->fields[i].field_name==T_COORD_INDEX)
	{
	  if(node->fields[i].field_value.int_value!=NULL)
	  delete node->fields[i].field_value.int_value;
	  node->fields[i].field_value.int_value=NULL;
	}
   
  }

  ///////  释放子节点信息

  for(int i=0;i<node->num_children;i++)
  {
	 ClearNode(node->children[i]);
	 delete node->children[i];
  }
  node->children = NULL;

}