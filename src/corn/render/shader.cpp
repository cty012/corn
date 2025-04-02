#include <fstream>
#include <bgfx/bgfx.h>
#include "shader.h"
#include "shader_embedded.h"

namespace corn {
    bgfx::ShaderHandle loadShader(const std::string& path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) {
            return BGFX_INVALID_HANDLE;
        }

        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        const bgfx::Memory* mem = bgfx::alloc(static_cast<uint32_t>(fileSize + 1));
        if (!file.read(reinterpret_cast<char*>(mem->data), fileSize)) {
            return BGFX_INVALID_HANDLE;
        }
        mem->data[mem->size - 1] = '\0';

        return bgfx::createShader(mem);
    }

    bgfx::ShaderHandle loadEmbeddedShader(const std::string& name) {
        auto it = EMBEDDED_SHADERS.find(name);
        if (it == EMBEDDED_SHADERS.end()) {
            return BGFX_INVALID_HANDLE;
        }

        const std::vector<unsigned char>& shaderData = it->second;
        const bgfx::Memory* mem = bgfx::alloc(static_cast<uint32_t>(shaderData.size() + 1));
        std::ranges::copy(shaderData, mem->data);
        mem->data[mem->size - 1] = '\0';

        return bgfx::createShader(mem);
    }

    Shader::Shader() : program_(BGFX_INVALID_HANDLE) {}

    Shader::Shader(Shader&& other) noexcept : program_(other.program_) {
        other.program_ = BGFX_INVALID_HANDLE;
    }

    Shader& Shader::operator=(Shader&& other) noexcept {
        if (this == &other) return *this;
        this->destroy();
        this->program_ = other.program_;
        other.program_ = BGFX_INVALID_HANDLE;
        return *this;
    }

    Shader::~Shader() noexcept {
        this->destroy();
    }

    void Shader::destroy() {
        if (bgfx::isValid(this->program_)) {
            bgfx::destroy(this->program_);
            this->program_ = BGFX_INVALID_HANDLE;
        }
    }

    bool Shader::loadEmbedded(const std::string& vsName, const std::string& fsName) {
        this->destroy();
        bgfx::ShaderHandle vertexShader = loadEmbeddedShader(vsName);
        if (!bgfx::isValid(vertexShader)) {
            return false;
        }
        bgfx::ShaderHandle fragmentShader = loadEmbeddedShader(fsName);
        if (!bgfx::isValid(fragmentShader)) {
            bgfx::destroy(vertexShader);
            return false;
        }
        this->program_ = bgfx::createProgram(vertexShader, fragmentShader, true);
        return true;
    }

    bool Shader::loadFromFile(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
        this->destroy();
        bgfx::ShaderHandle vertexShader = loadShader(vertexPath.string());
        if (!bgfx::isValid(vertexShader)) {
            return false;
        }
        bgfx::ShaderHandle fragmentShader = loadShader(fragmentPath.string());
        if (!bgfx::isValid(fragmentShader)) {
            bgfx::destroy(vertexShader);
            return false;
        }
        this->program_ = bgfx::createProgram(vertexShader, fragmentShader, true);
        return true;
    }

    bgfx::ProgramHandle Shader::getProgramHandle() const {
        return this->program_;
    }
}
