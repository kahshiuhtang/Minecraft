#ifndef MCRFT_SHADER_H
#define MCRFT_SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace MCRFT
{
	class Shader
	{
	public:
		unsigned int id;
		Shader(const char *vertexPath, const char *fragmentPath);
		void use() const
		{
			glUseProgram(id);
		}
		void setbool(const std::string &name, bool value) const
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
		}
		void setint(const std::string &name, int value) const
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
		}
		void setfloat(const std::string &name, float value) const
		{
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
		}
		void setvec2(const std::string &name, const glm::vec2 &value) const
		{
			glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
		}
		void setvec2(const std::string &name, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
		}
		void setvec3(const std::string &name, const glm::vec3 &value) const
		{
			glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
		}
		void setvec3(const std::string &name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
		}
		void setvec4(const std::string &name, const glm::vec4 &value) const
		{
			glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
		}
		void setvec4(const std::string &name, float x, float y, float z, float w) const
		{
			glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
		}
		void setmat2(const std::string &name, const glm::mat2 &mat) const
		{
			glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void setmat3(const std::string &name, const glm::mat3 &mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void setmat4(const std::string &name, const glm::mat4 &mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void activate();
		void delete_shader();

	private:
		void check_compile_errors(GLuint shader, std::string type)
		{
			GLint success;
			GLchar infoLog[1024];
			if (type != "PROGRAM")
			{
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "check_compile_errors()::SHADER_COMPILATION_ERROR of type: " << type << "\n"
							  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
			else
			{
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "check_compile_errors()::PROGRAM_LINKING_ERROR of type: " << type << "\n"
							  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
		}
	};

	class ShaderManager
	{
		public:
			unsigned int addshader(const char *vertexpath, const char *fragmentpath);
			void eraseshader(unsigned int shaderid);
			std::shared_ptr<MCRFT::Shader> getshader(unsigned int shaderid);
			
		private:
			std::unordered_map<unsigned int, std::shared_ptr<MCRFT::Shader>> m_shaders;
	};
}

#endif