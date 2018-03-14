// Useful documentation website: docs.gl

#include <GL\glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath) {
	std::ifstream stream(filePath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
	GLCALL(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLCALL(glShaderSource(id, 1, &src, nullptr));
	GLCALL(glCompileShader(id));

	int result;
	GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		int length;
		GLCALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		GLCALL(glGetShaderInfoLog(id, length, &length, message));
		std::cout << message << std::endl;
		GLCALL(glDeleteShader(id));
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	GLCALL(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCALL(glAttachShader(program, vs));
	GLCALL(glAttachShader(program, fs));
	GLCALL(glLinkProgram(program));
	GLCALL(glValidateProgram(program));

	GLCALL(glDeleteShader(vs));
	GLCALL(glDeleteShader(fs));

	return program;
}

int main() {
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "OpenGL_Testing", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		return -1;

	GLCALL(std::cout << glGetString(GL_VERSION) << std::endl);

	// We need to clean up our class objects before glfwTerminate()..
	// This scope is used to do that.
	{
		float positions[] = {
			-0.5f, -0.5f, // 0
			0.5f, -0.5f,  // 1
			0.5f, 0.5f,   // 2
			-0.5f, 0.5f,  // 3
		};

		// Index buffer for square (two triangles)
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		unsigned int vao; // vertex array object
		GLCALL(glGenVertexArrays(1, &vao));
		GLCALL(glBindVertexArray(vao));

		VertexBuffer vb(positions, 4 * 2 * sizeof(float));

		GLCALL(glEnableVertexAttribArray(0));
		GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

		IndexBuffer ib(indices, 6);

		ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
		unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
		GLCALL(glUseProgram(shader));

		GLCALL(int location = glGetUniformLocation(shader, "u_Color"));
		ASSERT(location != -1);

		// Unbind everything
		GLCALL(glBindVertexArray(0));
		GLCALL(glUseProgram(0));
		vb.Unbind();
		ib.Unbind();

		float r = 0.0f;
		float increment = 0.05f;
		while (!glfwWindowShouldClose(window)) {
			GLCALL(glClear(GL_COLOR_BUFFER_BIT));


			GLCALL(glUseProgram(shader));
			GLCALL(glUniform4f(location, r, 1.0f - r, 0.0f, 1.0f));

			GLCALL(glBindVertexArray(vao));
			ib.Bind();

			GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			r += increment;
			if (r > 1.0f) {
				increment = -0.05f;
				r = 1.0f;
			}
			else if (r < 0.0f) {
				increment = 0.05f;
				r = 0.0f;
			}

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		GLCALL(glDeleteProgram(shader));
	}
	glfwTerminate();
	return 0;
}
