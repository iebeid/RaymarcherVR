#include "RaymarcherCave.h"

#include <math.h>
#include <string>
#include <iostream>

#include <boost/cast.hpp>

#include <vrj/vrjConfig.h>
#include <vrj/Display/Viewport.h>
#include <vrj/Draw/OpenGL/DrawManager.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/common.hpp>

#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/common.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/extend.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/normal.hpp>

#include "Shader.h"
#include "FPS.h"

using namespace vrj;
using namespace std;

#define PI 3.14159265358

//Global variables
//TODO: global variables are bad, they need to be included in the App class
GLuint cave_shader_programme;
float cave_time_counter;
glm::vec2 cave_iResolution;
glm::vec3 user_rotation;
glm::vec3 user_translation;
GLuint cave_vao;
FPS cave_counter;
float cave_width;
float cave_height;

//The quad needed to render for raytracing
GLfloat cave_points[] = {
	-1.0f, -1.0f,
	-1.0f, 1.0f,
	1.0f, -1.0f,
	1.0f, 1.0
};

//App lifecycle initialization
void CaveApp::init()
{
	std::cout << "---------- CaveApp:init() ---------------" << std::endl;
	mWand.init("VJWand");
	mHead.init("VJHead");
	mButton0.init("VJButton0");
	mButton1.init("VJButton1");
}

//App lifecycle initialization
void CaveApp::apiInit()
{
}

void CaveApp::bufferPreDraw()
{
}

void CaveApp::preFrame()
{
	mCurTime += 0.005f;
}

glm::mat4 get_model_matrix_glm(float angle_x, float angle_y, float angle_z, float trans_x, float trans_y, float trans_z){
	float * rot = new float[9];
	glm::mat4 model_mat = glm::mat4();
	glm::mat3 rotation_matrix = glm::mat3();
	glm::mat3 rotation_x(1, 0, 0, 0, cos((angle_x*PI) / 180), -sin((angle_x*PI) / 180), 0, sin((angle_x*PI) / 180), cos((angle_x*PI) / 180));
	glm::mat3 rotation_y(cos((angle_y*PI) / 180), 0, sin((angle_y*PI) / 180), 0, 1, 0, -sin((angle_y*PI) / 180), 0, cos((angle_y*PI) / 180));
	glm::mat3 rotation_z(cos((angle_z*PI) / 180), -sin((angle_z*PI) / 180), 0, sin((angle_z*PI) / 180), cos((angle_z*PI) / 180), 0, 0, 0, 1);
	rotation_matrix = rotation_z * rotation_y * rotation_x;
	rot[0] = rotation_matrix[0][0];
	rot[1] = rotation_matrix[1][0];
	rot[2] = rotation_matrix[2][0];
	rot[3] = rotation_matrix[0][1];
	rot[4] = rotation_matrix[2][2];
	rot[5] = rotation_matrix[2][1];
	rot[6] = rotation_matrix[0][2];
	rot[7] = rotation_matrix[1][2];
	rot[8] = rotation_matrix[1][1];
	model_mat[0][0] = rot[0];
	model_mat[1][0] = rot[1];
	model_mat[2][0] = rot[2];
	model_mat[3][0] = trans_x;
	model_mat[0][1] = rot[3];
	model_mat[1][1] = rot[4];
	model_mat[2][1] = rot[5];
	model_mat[3][1] = trans_y;
	model_mat[0][2] = rot[6];
	model_mat[1][2] = rot[7];
	model_mat[2][2] = rot[8];
	model_mat[3][2] = trans_z;
	model_mat[0][3] = 0;
	model_mat[1][3] = 0;
	model_mat[2][3] = 0;
	model_mat[3][3] = 1;
	return model_mat;
}

//TODO: get the width and the height of the viewport
void CaveApp::draw()
{
	//Start frame counter
	cave_counter.start_fps_counter();

	//Get width and height of the viewport dynamically
	vrj::opengl::DrawManager* drawMgr = boost::polymorphic_downcast<vrj::opengl::DrawManager*>(getDrawManager());
	vrj::opengl::UserData*    udata = drawMgr->currentUserData();
	vrj::ProjectionPtr const  proj = udata->getProjection();
	vrj::Frustum const& frust = proj->getFrustum();
	glm::mat4 view_mat = glm::make_mat4x4(proj->getViewMatrix().getData());
	glm::mat4 proj_mat = glm::frustum(frust[vrj::Frustum::VJ_LEFT], frust[vrj::Frustum::VJ_RIGHT], frust[vrj::Frustum::VJ_BOTTOM], frust[vrj::Frustum::VJ_TOP], frust[vrj::Frustum::VJ_NEAR], frust[vrj::Frustum::VJ_FAR]);
	glm::mat4 model_mat = get_model_matrix_glm(0.0f, 0.0f, 0.0f, 0.0f, 3.5f, 1.0f);
	glm::mat4 model_view_proj = proj_mat * view_mat * model_mat;
	
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(model_view_proj, scale, rotation, translation, skew, perspective);
	glm::vec3 euler_rotation = glm::eulerAngles(rotation) * (PI / 180.0f);
	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(cave_vao);
	glUseProgram(cave_shader_programme);

	//Updating shader uniforms
	cave_iResolution = glm::vec2(cave_width, cave_height);
	glUniform2fv(glGetUniformLocation(cave_shader_programme, "iResolution"), 1, glm::value_ptr(cave_iResolution));
	cave_time_counter = cave_time_counter + 0.02f;
	glUniform1f(glGetUniformLocation(cave_shader_programme, "iGlobalTime"), cave_time_counter);
	user_rotation = glm::vec3(euler_rotation.x, euler_rotation.y, euler_rotation.z);
	glUniform3fv(glGetUniformLocation(cave_shader_programme, "user_rotation"), 1, glm::value_ptr(user_rotation));
	user_translation = glm::vec3(translation.x, translation.y, translation.z);
	glUniform3fv(glGetUniformLocation(cave_shader_programme, "user_translation"), 1, glm::value_ptr(user_translation));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	cave_counter.end_fps_counter();
	//cave_counter.print_fps();
}

void CaveApp::intraFrame()
{
}

void CaveApp::postFrame()
{
}

void CaveApp::latePreFrame()
{
}

void CaveApp::contextPostDraw()
{
}

void CaveApp::contextPreDraw()
{
}

void CaveApp::initGLState()
{
	//Initialize OpenGL
	const GLubyte* renderer;
	const GLubyte* version;

	glewExperimental = GL_TRUE;
	glewInit();
	renderer = glGetString(GL_RENDERER);
	version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	const char* env_p = std::getenv("SHADER_PATH");
	const char* vertex_shader_file_name = "vertex.vert";
	const char* fragment_shader_file_name = "fragment.frag";
	char vertex_shader_file_name_str[250];
	char fragment_shader_file_name_str[250];
	strcpy(vertex_shader_file_name_str, env_p);
	strcat(vertex_shader_file_name_str, "/");
	strcat(vertex_shader_file_name_str, vertex_shader_file_name);
	strcpy(fragment_shader_file_name_str, env_p);
	strcat(fragment_shader_file_name_str, "/");
	strcat(fragment_shader_file_name_str, fragment_shader_file_name);

	//Initialize Shader
	const char* vertex_shader = (const char*)Shader::load_shader(vertex_shader_file_name_str);
	const char* fragment_shader = (const char*)Shader::load_shader(fragment_shader_file_name_str);
	Shader s;
	s.vertex_shader = vertex_shader;
	s.fragment_shader = fragment_shader;

	//Get viewport information
	char* env_viewport_width = std::getenv("VIEWPORT_WIDTH");
	char* env_viewport_height = std::getenv("VIEWPORT_HEIGHT");
	cave_width = (float)atof(env_viewport_width);
	cave_height = (float)atof(env_viewport_height);

	//Create VBO
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cave_points), cave_points, GL_STATIC_DRAW);

	//Create VAO
	glGenVertexArrays(1, &cave_vao);
	glBindVertexArray(cave_vao);

	//Input uniforms positions
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	cave_iResolution = glm::vec2(cave_width, cave_height);
	glUniform2fv(glGetUniformLocation(cave_shader_programme, "iResolution"), 1, glm::value_ptr(cave_iResolution));
	cave_time_counter = 1.0f;
	glUniform1f(glGetUniformLocation(cave_shader_programme, "iGlobalTime"), cave_time_counter);
	user_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glUniform3fv(glGetUniformLocation(cave_shader_programme, "user_rotation"), 1, glm::value_ptr(user_rotation));
	user_translation = glm::vec3(0.0f, 3.5f, 5.0f);
	glUniform3fv(glGetUniformLocation(cave_shader_programme, "user_translation"), 1, glm::value_ptr(user_translation));

	//Compile shader
	//Vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &s.vertex_shader, NULL);
	glCompileShader(vs);

	//Fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &s.fragment_shader, NULL);
	glCompileShader(fs);

	//Link shader
	cave_shader_programme = glCreateProgram();
	glAttachShader(cave_shader_programme, fs);
	glAttachShader(cave_shader_programme, vs);
	glLinkProgram(cave_shader_programme);

	//Use shader
	glUseProgram(cave_shader_programme);
}