#include "App.h"

#include "RaymarcherRender.h"
#include "Queue.h"
#include "Shader.h"
#include "FPS.h"

#include <Windows.h>

#include <cstdlib>
#include "RaymarcherCave.h"
#include <vrj/Kernel/Kernel.h>

#include <sstream>
#include <string>

using namespace std;

int width = 640;
int height = 480;

Shader s;

static void produce_shader(Queue<Shader>& shaders){
	while (1){
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
		s.vertex_shader = vertex_shader;
		s.fragment_shader = fragment_shader;
		shaders.push(s);
		
	}
}

static void consume_shader(Queue<Shader>& shaders){
	//const char* env_p = std::getenv("SHADER_PATH");
	//const char* vertex_shader_file_name = "vertex.vert";
	//const char* fragment_shader_file_name = "fragment.frag";
	//char vertex_shader_file_name_str[250];
	//char fragment_shader_file_name_str[250];
	//strcpy(vertex_shader_file_name_str, env_p);
	//strcat(vertex_shader_file_name_str, "/");
	//strcat(vertex_shader_file_name_str, vertex_shader_file_name);
	//strcpy(fragment_shader_file_name_str, env_p);
	//strcat(fragment_shader_file_name_str, "/");
	//strcat(fragment_shader_file_name_str, fragment_shader_file_name);

	////Initialize Shader
	//const char* vertex_shader = (const char*)Shader::load_shader(vertex_shader_file_name_str);
	//const char* fragment_shader = (const char*)Shader::load_shader(fragment_shader_file_name_str);
	//Shader s;
	//s.vertex_shader = vertex_shader;
	//s.fragment_shader = fragment_shader;
	Shader produced_shader = shaders.pop();
	GLFWwindow * raymarching_window = setup_raymarcher(width, height, 100, 400, "RaymarcherVR", s);
	FPS counter;
	while (1){
		counter.start_fps_counter();
		Shader produced_shader_after_first_frame = shaders.pop();
		render_raymarcher(raymarching_window, s, width, height);
		counter.end_fps_counter();
		counter.print_fps();
	}
	terminate_raymarcher(raymarching_window);
}

static void cave_thread_callback(Queue<Shader>& shaders, int argc, char * argv[]){
	Kernel* kernel = Kernel::instance();
	CaveApp* application = new CaveApp();
	kernel->init(argc, argv);
	for (int i = 1; i < argc; ++i)
	{
		kernel->loadConfigFile(argv[i]);
	}
	kernel->start();
	kernel->setApplication(application);
	kernel->waitForKernelStop();
	delete application;
}

void MainApp::run(int argc, char * argv[]){
	cout << "Initialize RaymarcherVR" << endl;
	Queue<Shader> shaders;
	using namespace std::placeholders;
	const char* status_env_p = std::getenv("STATUS");
	string status(status_env_p);
	if (status == "MASTER"){
		std::thread prod1(std::bind(produce_shader, std::ref(shaders)));
		std::thread consumer1(std::bind(consume_shader, std::ref(shaders)));
		std::thread cave_thread(std::bind(cave_thread_callback, std::ref(shaders), std::ref(argc), std::ref(argv)));
		prod1.join();
		consumer1.join();
		cave_thread.join();
	}
	else if (status == "SLAVE"){
		std::thread cave_thread(std::bind(cave_thread_callback, std::ref(shaders), std::ref(argc), std::ref(argv)));
		cave_thread.join();
	}
	else if (status == "GLFW"){
		std::thread prod1(std::bind(produce_shader, std::ref(shaders)));
		std::thread glfw_thread(std::bind(consume_shader, std::ref(shaders)));
		prod1.join();
		glfw_thread.join();
	}
}

void MainApp::terminate(){

}