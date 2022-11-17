#include "glpch.h"
#include "Shader.h"

namespace GL {

	Shader::Shader(const std::string& filepath)
	{
	}

	Shader::Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
	}

	Shader::~Shader()
	{
	}

	void Shader::bind() const
	{
	}

	void Shader::unbind() const
	{
	}

	const std::string& Shader::getName() const
	{
		// TODO: insert return statement here
	}

	void Shader::setInt(const std::string& name, int value)
	{
	}

	void Shader::setIntArray(const std::string& name, int* values, uint count)
	{
	}

	void Shader::setMat4(const std::string& name, const glm::mat4& value)
	{
	}

	void Shader::setFloat(const std::string& name, float value)
	{
	}

	void Shader::setFloat3(const std::string& name, const glm::vec3& value)
	{
	}

	void Shader::setFloat4(const std::string& name, const glm::vec4& value)
	{
	}

	int Shader::GetUniformLocation(const std::string& name) const
	{
		return 0;
	}

	std::string Shader::ReadFile(const std::string filepath)
	{
		return std::string();
	}

	std::unordered_map<uint, std::string> Shader::PreProcess(const std::string& source)
	{
		return std::unordered_map<uint, std::string>();
	}

	void Shader::Compile(const std::unordered_map<uint, std::string>& shaderSources)
	{
	}
}