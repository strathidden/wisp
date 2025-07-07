#pragma once

#include "IFractal.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class FractalRegistry
{
public:
    FractalRegistry() = default;
    
    template<typename T>
    void registerFractal(const std::string& name)
    {
        // use Concept?
        static_assert(std::is_base_of_v<IFractal, T>, "T must derive from IFractal");
        m_constructors[name] = []() -> std::unique_ptr<IFractal> {
            return std::make_unique<T>();
        };
    }
    
    std::unique_ptr<IFractal> createFractal(const std::string& name) const;
    std::vector<std::string> getNames() const;
    
private:
    std::unordered_map<std::string, std::function<std::unique_ptr<IFractal>()>> m_constructors;
};
