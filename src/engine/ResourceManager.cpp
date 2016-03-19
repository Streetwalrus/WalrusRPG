#include "ResourceManager.h"
#include "Logger.h"
#include "TINYSTL/unordered_map.h"

using namespace WalrusRPG; /*::ResourceManager*/
using namespace WalrusRPG::Logger;
using WalrusRPG::PIAF::Archive;

using tinystl::unordered_map;

ManagedArchive::ManagedArchive(const char *path)
    : path(path), arc(ResourceManager::require(path))
{
    log("Resource Manager : +Ref %s", path);
}
ManagedArchive::~ManagedArchive()
{
    log("Resource Manager : -Ref %s", path);
    ResourceManager::release(path);
}

ManagedArchive::operator Archive *() const
{
    return arc;
}

struct node
{
    Archive *arc;
    uint16_t refcount;
};
static unordered_map<const char *, node> files;

void ResourceManager::init()
{
}

void ResourceManager::deinit()
{
    for (auto ptr = files.begin(), end = files.end(); ptr != end; ++ptr)
    {
        if (ptr->second.refcount == 0)
            log("Resource Manager : Prune Node : %s", ptr->first);
        else
        {
            log("Resource Manager : Delete Node : %s", ptr->first);
            delete ptr->second.arc;
        }
    }
    files.clear();
}

Archive *ResourceManager::require(const char *path)
{
    auto entry = files.find(path);
    if (entry == files.end())
    {
        log("Resource Manager : New : %s", path);
        return files.insert({path, {new Archive(path), 1}}).first->second.arc;
        // return nullptr;
    }
    if (entry->second.refcount == 0)
    {
        log("Resource Manager : Reload : %s", path);
        entry->second.arc = new Archive(path);
    }
    entry->second.refcount++;
    return entry->second.arc;
}

void ResourceManager::release(WalrusRPG::PIAF::Archive *arcs)
{
    for (auto ptr = files.begin(), end = files.end(); ptr != end; ++ptr)
    {
        if (ptr->second.arc == arcs)
        {
            ptr->second.refcount--;
            if (ptr->second.refcount == 0)
            {
                log("Resource Manager : Free : %s", ptr->first);
                delete ptr->second.arc;
                ptr->second.refcount = 0;
            }
            return;
        }
    }
}

void ResourceManager::release(const char *path)
{
    auto ptr = files.find(path);
    if (ptr == files.end())
        return;
    ptr->second.refcount--;
    if (ptr->second.refcount == 0)
    {
        log("Resource Manager : Free : %s", ptr->first);
        delete ptr->second.arc;
        ptr->second.refcount = 0;
    }
    return;
}