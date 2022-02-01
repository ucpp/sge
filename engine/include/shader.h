#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace sge
{
	class Shader
	{
		enum class Type
		{
			Vertex,
			Fragment,
			Program
		};

	public:
		void compile(const std::string& vertex_source, const std::string& fragment_source);
		void use();
		uint32_t getId();

		void setBool(const char* name, bool value);
		void setInt(const char* name, int value);
		void setFloat(const char* name, float value);
		void setVec2(const char* name, float x, float y);
		void setVec3(const char* name, float x, float y, float z);
		void setVec4(const char* name, float x, float y, float z, float w);

		void setMatrix4(const char* name, const glm::mat4& matrix);
		void setVec3(const char* name, glm::vec3 v3);

	public:
		std::string name;
		bool is_lit;
		std::string vertex_source;
		std::string fragment_source;

	private:
		uint32_t id;

	private:
		void checkCompile(uint32_t id, Type type);
	};
}