#pragma once
#include <Services\Resources\IResource.hpp>
#include <string>


class TextResource : public IResource 
{
public:
    bool Load(const std::string& path) override;

    void Release() override;

    std::string GetType() const override { return  "Text"; }

    std::string& GetContent() { return content; }

private:
    std::string content;

};