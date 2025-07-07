#include "FractalRegistry.h"

std::unique_ptr<IFractal> FractalRegistry::createFractal(const std::string& name) const
{
    auto it = m_constructors.find(name);
    if (it == m_constructors.end()) return nullptr;
    return it->second();
}

std::vector<std::string> FractalRegistry::getNames() const
{
    std::vector<std::string> names;
    for (const auto& pair : m_constructors)
    {
        names.push_back(pair.first);
    }
    return names;
}
