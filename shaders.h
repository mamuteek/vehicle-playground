#ifndef SHADERS_H_
#define SHADERS_H_

// We are linking to the static version of GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw3.h>

#include <vector>
#include <iostream>


static const GLchar* vertexSource =
"#version 330 core\n"
"layout(location = 0) in vec3 in_position;"
"layout(location = 1) in vec3 in_color;"
"out vec4 frag_color;"
"uniform mat4 mvp;"
"void main()"
"{"
"	gl_Position = mvp * vec4(in_position, 1);"
"	frag_color = vec4(in_color, 1);"
"}";


static const GLchar* vertexSourceTriang =
"#version 330 core\n"
"layout(location = 2) in vec3 in_position;"
"layout(location = 3) in vec4 in_color;"
"out vec4 frag_color;"
"uniform mat4 mvp;"
"void main()"
"{"
"	gl_Position = mvp * vec4(in_position, 1);"
"	frag_color = in_color;"
"}";


static const GLchar* fragmentSource =
"#version 330 core\n"
"in vec4 frag_color;"
"out vec4 out_color;"
"void main()"
"{"
"	out_color = frag_color;"
"}";


class myShaders
{
	public:
		GLuint linesShaderProgram;
		GLuint trianglesShaderProgram;

		myShaders() :
			linesShaderProgram(),
			trianglesShaderProgram()
		{
			// Compile vertex shader: lines version
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &vertexSource, NULL);
			glCompileShader(vertexShader);

			GLint isCompiled = 0;
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE) {
				GLint maxLength = 0;
				glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> errorLog(maxLength);
				glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);
				glDeleteShader(vertexShader);
				std::cout << &errorLog[0] << "\n";
				return;
			}
			else {
				std::cout << "Vertex shader compiled. \n";
			}


			// Compile vertex shader: triangles version
			GLuint vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader2, 1, &vertexSourceTriang, NULL);
			glCompileShader(vertexShader2);

			isCompiled = 0;
			glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE) {
				GLint maxLength = 0;
				glGetShaderiv(vertexShader2, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> errorLog(maxLength);
				glGetShaderInfoLog(vertexShader2, maxLength, &maxLength, &errorLog[0]);
				glDeleteShader(vertexShader2);
				std::cout << &errorLog[0] << "\n";
				return;
			}
			else {
				std::cout << "Vertex shader compiled. \n";
			}

			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
			glCompileShader(fragmentShader);

			isCompiled = 0;
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE) {
				GLint maxLength = 0;
				glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> errorLog(maxLength);
				glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]);
				glDeleteShader(fragmentShader);
				std::cout << &errorLog[0] << "\n";
				return;
			}
			else {
				std::cout << "Fragment shader compiled. \n";
			}

			linesShaderProgram = glCreateProgram();
			glAttachShader(linesShaderProgram, vertexShader);
			glAttachShader(linesShaderProgram, fragmentShader);
			glLinkProgram(linesShaderProgram);

			GLint isLinked = 0;
			glGetProgramiv(linesShaderProgram, GL_LINK_STATUS, &isLinked);
			if (isLinked == GL_FALSE) {
				GLint maxLength = 0;
				glGetProgramiv(linesShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(linesShaderProgram, maxLength, &maxLength, &infoLog[0]);
				glDeleteProgram(linesShaderProgram);
				std::cout << &infoLog[0] << "\n";
				return;
			}
			else {
				std::cout << "Shader linking successful. \n";
			}

			trianglesShaderProgram = glCreateProgram();
			glAttachShader(trianglesShaderProgram, vertexShader2);
			glAttachShader(trianglesShaderProgram, fragmentShader);
			glLinkProgram(trianglesShaderProgram);

			isLinked = 0;
			glGetProgramiv(trianglesShaderProgram, GL_LINK_STATUS, &isLinked);
			if (isLinked == GL_FALSE) {
				GLint maxLength = 0;
				glGetProgramiv(trianglesShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(trianglesShaderProgram, maxLength, &maxLength, &infoLog[0]);
				glDeleteProgram(trianglesShaderProgram);
				std::cout << &infoLog[0] << "\n";
				return;
			}
			else {
				std::cout << "Shader linking successful. \n";
			}

			glDeleteShader(vertexShader);
			glDeleteShader(vertexShader2);
			glDeleteShader(fragmentShader);
		}

		~myShaders()
		{
			glDeleteProgram(linesShaderProgram);
			glDeleteProgram(trianglesShaderProgram);
		}
};



#endif /* SHADERS_H_ */
