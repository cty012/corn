#pragma once

#include <filesystem>
#include <string>
#include <bgfx/bgfx.h>

namespace corn {
    class Shader {
    public:
        Shader();
        ~Shader() noexcept;

        void destroy();

        bool loadEmbedded(const std::string& vsName, const std::string& fsName);
        bool loadFromFile(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

        // Getters
        [[nodiscard]] bgfx::ProgramHandle getProgramHandle() const;

    private:
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;
        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;

        bgfx::ProgramHandle program_;
    };
}
