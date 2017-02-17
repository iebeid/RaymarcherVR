#include "Shader.h"

unsigned long Shader::getFileLength(ifstream& file)
{
	if (!file.good()) return 0;
	unsigned long pos = (unsigned long)file.tellg();
	file.seekg(0, ios::end);
	unsigned long len = (unsigned long)file.tellg();
	file.seekg(ios::beg);
	return len;
}

GLubyte* Shader::load_shader(char* filename)
{
	GLubyte*   ShaderSource = 0;
	bool       _memalloc = false;
	ifstream file;
	file.open(filename, ios::in);
	if (!file) cout << "File not found" << endl;
	unsigned long len = getFileLength(file);
	if (len == 0) cout << "Empty file" << endl;
	if (ShaderSource != 0)
	{
		if (_memalloc)
			delete[] ShaderSource;
	}
	ShaderSource = (GLubyte*) new char[len + 1];
	if (ShaderSource == 0) cout << "can't reserve memory" << endl;
	_memalloc = true;
	ShaderSource[len] = 0;
	unsigned int i = 0;
	while (file.good())
	{
		ShaderSource[i] = file.get();
		if (!file.eof())
			i++;
	}
	ShaderSource[i] = 0;
	file.close();
	return ShaderSource;
}