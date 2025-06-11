#include "ecs.h"

namespace qyhs::ecs
{
    void ComponentLibrary::serialize(Archive& archive, EntitySerializer& entity_serializer)
    {
        if (archive.isReadMode())
        {
            //bool has_next;
            //size_t begin = archive.getPos();
            //do
            //{
            //    archive >> has_next;
            //    if(has_next)
            //    {
            //        std::string name;
            //        archive >> name;
            //        uint64_t jump_pos = 0;
            //        archive >> jump_pos;
            //        auto it = entries.find(name);
            //        if(it != entries.end())
            //        {
            //            it->second.component_manager->serialize(archive, entity_serializer);
            //        }
            //        else
            //        {
            //            // component manager of this name was not registered, skip serialization by jumping over the data
            //            archive.jump(jump_pos);
            //        }
            //    }
            //}
            //while(has_next);
            bool has_next;
            size_t begin = archive.getPos();
            archive >> has_next;
            while (has_next)
            {
                std::string name;
                archive >> name;
                uint64_t jump_pos = 0;
                archive >> jump_pos;
                auto it = entries.find(name);
                if (it != entries.end())
                {
                    it->second.component_manager->serialize(archive, entity_serializer);
                }
                else
                {
                    archive.jump(jump_pos);
                }
                archive >> has_next;
            }
        }
        else
        {
            for (auto& iter : entries)
            {
                archive << true;
                std::string n = iter.first;
                archive << iter.first;
                size_t offset = archive.writeUnKnownJumpPosition();
                iter.second.component_manager->serialize(archive, entity_serializer);
                archive.patchUnKnownJumpPosition(offset);

            }
            archive << false;
        }
    }
    void EntitySerializer::registerResource(const std::string& name)
    {
        if (name.empty())
        {
            return;
        }
        resource_registration.insert(name);
    }
}