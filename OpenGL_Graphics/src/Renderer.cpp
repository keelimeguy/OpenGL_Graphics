#include "Renderer.h"
#include <iostream>

void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
	bool ret = true;
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (0x" << std::hex << error << "):"
			<< function << " " << file << ":" << line << std::endl;
		ret = false;
	}
	return ret;
}
