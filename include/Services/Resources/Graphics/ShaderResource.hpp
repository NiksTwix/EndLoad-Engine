#pragma once
#include <Services/Resources/IResource.hpp>
#include <Services/Resources/Graphics/Shader.hpp>
#include <memory>



class ShaderResource: public IResource
{
public:
    bool Load(const std::string& path) override;

    void Release() override;

    std::string GetType() const override { return  "Shader"; }

    std::shared_ptr<Shader> GetContent() { return content; }

private:
    std::shared_ptr<Shader> content;

};