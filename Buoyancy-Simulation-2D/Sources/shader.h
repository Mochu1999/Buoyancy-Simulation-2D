#pragma once


using namespace std;


struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

struct Shader {

	std::string m_FilePath;
	int m_RendererID;
	int colorLocation;

	Shader(const std::string& filepath)
		:m_FilePath(filepath), m_RendererID(0) { //says that we are maintaining m_FilePath only fof debugging purpouses
		ShaderProgramSource source = ParseShader(filepath);
		m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);

		colorLocation = glGetUniformLocation(m_RendererID, "u_Color");
	}

	ShaderProgramSource ParseShader(const string& filepath) { //Converts a .shader into 2 separate streams and each one into strings for glShaderSource in CompileShader
		ifstream stream(filepath);

		enum class ShaderType {
			NONE = -1, VERTEX = 0, FRAGMENT = 1
		};

		string line;
		stringstream ss[2];
		ShaderType type = ShaderType::NONE;
		while (getline(stream, line)) {
			if (line.find("#shader") != std::string::npos) { //npos is the constant that will be given if it was not found, otherwise it will give a position, not a bool
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != string::npos)
					type = ShaderType::FRAGMENT;

			}
			else { //what isn't in #shader line is included in the stream
				ss[(int)type] << line << '\n'; //the enum positionates vertex in 0 and fr in 1
			}
		}

		return { ss[0].str(),ss[1].str() }; //str converts streams into strings

	}


	unsigned int CompileShader(unsigned int type, const string& source) {
		unsigned int id = glCreateShader(type); //you specifies type of shader to create

		const char* src = &source[0];//you will use this pointer to point to the start of your source, char bc string is in chars, equivalent to const char* src = source.c_str()

		glShaderSource(id, 1, &src, nullptr); //Sets the source code in a shader object id 
		//[1] number of elements in the string 
		//[3] if I had an array of source code, this would set the lenght of it to avoid mixing it with other things, I understand 
		glCompileShader(id);

		//Error handling from sintax
		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result); //if status of id is successful it will give TRUE
		if (!result) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //Gets the length from the error
			//char* message = (char*)alloca(length * sizeof(char)); 
			GLchar message[1024]; //from a example, i think it sets the size of the message, validate
			glGetShaderInfoLog(id, length, &length, message);
			cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << endl;
			cout << message << endl;
			glDeleteShader(id);
			return 0;
		}

		return id;
	}

	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) { 	//creates shader with their string
		unsigned int program = glCreateProgram();// A program object is an object to which shader objects can be attached   and returns a non-zero value by which it can be referenced (unless error, then 0)


		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

		//linking them:
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);// This validates the program to ensure that the shaders are compatible with the current OpenGL context
		glDeleteShader(vs); // deletes the shader, they are occupying memory with intermediate files
		glDeleteShader(fs);
		return program;
	}
	~Shader() {
		glDeleteProgram(m_RendererID);
	}
	void Bind() const {
		glUseProgram(m_RendererID);
	}

	void Unbind() const {
		glUseProgram(0);
	}

	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]); //v is that we are sending an array
		//[2] if we need to transpose
	}
	void SetUniform1i(const std::string& name, int value) {
		glUniform1i(GetUniformLocation(name), value); //v is that we are sending an array
		//[2] if we need to transpose
	}

	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
		int location = GetUniformLocation(name);
		if (location != -1) { // Only set the uniform if it exists in the shader
			glUniform4f(location, v0, v1, v2, v3);
		}
	}

	int GetUniformLocation(const std::string& name) {
		/*if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];
		*/
		//solo estaba esto de abajo antes de unordored map
		int location = glGetUniformLocation(m_RendererID, name.c_str()); //The location represents a handle or identifier that OpenGL uses to identify and access a specific uniform variable//Without the location, OpenGL wouldn't know which uniform variable you are referring to when you call glUniform4f
		if (location == -1) {//if location ==-1 it means it couldn't find the uniform, it can happen if the uniform is unused//But why do I need this line?
			std::cout << "Warning " << name << " does not exist" << std::endl;
		}

		/*m_UniformLocationCache[name] = location; */
		return location;
	}

};