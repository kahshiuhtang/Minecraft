#include "renderer/shader.hpp"

// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char *filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	throw(errno);
}

// Constructor that build the Shader Program from 2 different shaders
MCRFT::Shader::Shader(const char *vertexFile, const char *fragmentFile)
{
	// Read vertexFile and fragmentFile and store the strings
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	// Convert the shader source strings into character arrays
	const char *vertexSource = vertexCode.c_str();
	const char *fragmentSource = fragmentCode.c_str();

	// Create Vertex Shader Object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);

	// Create Shader Program Object and get its reference
	id = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(id);

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

// Activates the Shader Program
void MCRFT::Shader::activate()
{
	glUseProgram(id);
}

// Deletes the Shader Program
void MCRFT::Shader::delete_shader()
{
	glDeleteProgram(id);
}

unsigned int MCRFT::ShaderManager::addshader(const char *vertexpath, const char *fragmentpath)
{
	auto newshader = std::make_shared<MCRFT::Shader>(vertexpath, fragmentpath);
	if(m_shaders.find(newshader->id) == m_shaders.end())
	{
		m_shaders[newshader->id] = newshader;
		return newshader->id;
	}
	return 0;
}

void MCRFT::ShaderManager::eraseshader(unsigned int shaderid)
{
	if (m_shaders.find(shaderid) != m_shaders.end()) {
        m_shaders.erase(shaderid);
    }
}

std::shared_ptr<MCRFT::Shader> MCRFT::ShaderManager::getshader(unsigned int shaderid)
{
	if (m_shaders.find(shaderid) != m_shaders.end()) {
        return m_shaders[shaderid];
    }
	return nullptr;
}