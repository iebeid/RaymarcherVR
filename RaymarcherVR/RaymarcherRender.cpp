#include "RaymarcherRender.h"
#include "Shader.h"
#include "FPS.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint vao;
GLuint shader_programme;
FPS counter;
float time_counter;
glm::vec2 iResolution;
glm::vec3 camera_rotation;
glm::vec3 camera_translation;
bool shader_updated;
float camera_x;
float camera_y;
float camera_z;
float camera_rot_x;
float camera_rot_y;
float camera_rot_z;

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		camera_z = camera_z + 0.5f;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		camera_z = camera_z - 0.5f;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		camera_x = camera_x - 0.5f;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		camera_x = camera_x + 0.5f;
	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		camera_y = camera_y + 0.5f;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		camera_y = camera_y - 0.5f;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		camera_rot_z = camera_rot_z + 0.1f;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		camera_rot_z = camera_rot_z - 0.1f;
	}
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	iResolution = glm::vec2(width, height);
	
}

GLFWwindow* setup_raymarcher(int width, int height, int pos_x, int pos_y, const char * title, Shader s){

	shader_updated = false;

	GLfloat points[] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0
	};

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1920, 1200, title, NULL, NULL);
	glfwSetWindowSize(window, width, height);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetWindowPos(window, pos_x, pos_y);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	glewExperimental = GL_TRUE;
	glewInit();
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//Create VBO
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	//Create VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//Input uniforms positions
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//Create and Use Shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &s.vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &s.fragment_shader, NULL);
	glCompileShader(fs);
	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);
	iResolution = glm::vec2(width,height);
	glUniform2fv(glGetUniformLocation(shader_programme, "iResolution"), 1, glm::value_ptr(iResolution));
	time_counter = 1.0f;
	glUniform1f(glGetUniformLocation(shader_programme, "iGlobalTime"), time_counter);
	camera_rot_x = 0.0f;
	camera_rot_y = 0.0f;
	camera_rot_z = 0.0f;
	camera_rotation = glm::vec3(camera_rot_x, camera_rot_y, camera_rot_z);
	glUniform3fv(glGetUniformLocation(shader_programme, "user_rotation"), 1, glm::value_ptr(camera_rotation));
	camera_x = 0.0f;
	camera_y = 3.5f;
	camera_z = 5.0f;
	camera_translation = glm::vec3(camera_x, camera_y, camera_z);
	glUniform3fv(glGetUniformLocation(shader_programme, "user_translation"), 1, glm::value_ptr(camera_translation));
	glUseProgram(shader_programme);

	return window;
}

void render_raymarcher(GLFWwindow* window, Shader s, int width, int height)
{
	//counter.start_fps_counter();
	glfwMakeContextCurrent(window);
	if (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glUseProgram(shader_programme);
		
		glUniform2fv(glGetUniformLocation(shader_programme, "iResolution"), 1, glm::value_ptr(iResolution));
		time_counter = time_counter + 0.02f;
		glUniform1f(glGetUniformLocation(shader_programme, "iGlobalTime"), time_counter);
		camera_rotation = glm::vec3(camera_rot_x, camera_rot_y, camera_rot_z);
		glUniform3fv(glGetUniformLocation(shader_programme, "user_rotation"), 1, glm::value_ptr(camera_rotation));
		camera_translation = glm::vec3(camera_x, camera_y, camera_z);
		glUniform3fv(glGetUniformLocation(shader_programme, "user_translation"), 1, glm::value_ptr(camera_translation));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	//counter.end_fps_counter();
	//counter.print_fps();
}

void terminate_raymarcher(GLFWwindow* window){
	glfwDestroyWindow(window);
	glfwTerminate();
}