#include<fstream>
#include<sstream>

GLuint LoadShaders(const char * vertexShaderPath ,const char * fragmentShaderPath){

	std::string vertexShaderSource;
    std::ifstream f(vertexShaderPath);
    if (f) {
		std::ostringstream ss;
		ss << f.rdbuf();
		vertexShaderSource = ss.str();
    }

	std::string fragmentShaderSource;
    std::ifstream ff(fragmentShaderPath);
    if (ff) {
		std::ostringstream ss;
		ss << ff.rdbuf();
		fragmentShaderSource = ss.str();
    }
	char const * vertexShaderPointer = vertexShaderSource.c_str();
	char const * fragmentShaderPointer = fragmentShaderSource.c_str();


	// Create the shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderPointer , NULL);
	glCompileShader(vertexShader);

	glShaderSource(fragmentShader, 1, &fragmentShaderPointer , NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}