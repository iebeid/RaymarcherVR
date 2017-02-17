#ifndef SHADER_H
#define SHADER_H 1
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <GL/glew.h>
using namespace std;
class Shader{
public:
	Shader(){ ; }
	~Shader(){ ; }
	static unsigned long getFileLength(ifstream& file);
	static GLubyte* load_shader(char* filename);
public:
	const char * vertex_shader;
	const char * fragment_shader;
};

#endif