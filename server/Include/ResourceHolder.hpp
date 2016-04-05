#ifndef RESOURCEHOLDER_HPP
#define RESOURCEHOLDER_HPP

#include <string>
#include <map>
#include <memory>
#include <stdexcept>
#include <cassert>

template <typename Resource, typename Identifier>
class ResourceHolder
{
  public:
    void  load(Identifier id, const std::string& filename);

    template <typename Parameter>
    void  load(Identifier id, const std::string& filename,
        const Parameter& secondParam);

    Resource&   get(Identifier id);
    const Resource& get(Identifier id) const;

  private:
    std::map<Identifier, std::unique_ptr<Resource>> resourceMap;

  private:
    void insertResource(Identifier id, std::unique_ptr<Resource> resource);
};


#include "ResourceHolder.inl"
#endif
