#pragma once

#include "Core/Common.h"

#include <glm/glm.hpp>

namespace GL {

	class Shader
	{
	public:
		Shader(const std::string& filepath);
		Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void bind() const;
		void unbind() const;

		const std::string& getName() const;

		void setInt(const std::string& name, int value);
		void setIntArray(const std::string& name, int* values, uint count);
		void setMat4(const std::string& name, const glm::mat4& value);
		void setFloat(const std::string& name, float value);
		void setFloat3(const std::string& name, const glm::vec3& value);
		void setFloat4(const std::string& name, const glm::vec4& value);

	private:
		int GetUniformLocation(const std::string& name) const;

		std::string ReadFile(const std::string filepath);
		std::unordered_map<uint, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<uint, std::string>& shaderSources);

	private:
		std::string mName;
		uint32_t mRendererID;
		mutable std::unordered_map<std::string, int> mUniformLocCache;
	};

}