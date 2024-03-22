

#include "serializer.h"

namespace QYHS
{
    
    template<>
    char& Serializer::read(const Json& json_context, char& instance)
    {
        assert(json_context.is_number());
        return instance = json_context.number_value();
    }

    
    template<>
    int& Serializer::read(const Json& json_context, int& instance)
    {
        assert(json_context.is_number());
        return instance = static_cast<int>(json_context.number_value());
    }

    
    template<>
    unsigned int& Serializer::read(const Json& json_context, unsigned int& instance)
    {
        assert(json_context.is_number());
        return instance = static_cast<unsigned int>(json_context.number_value());
    }

    
    template<>
    float& Serializer::read(const Json& json_context, float& instance)
    {
        assert(json_context.is_number());
        return instance = static_cast<float>(json_context.number_value());
    }

    
    template<>
    double& Serializer::read(const Json& json_context, double& instance)
    {
        assert(json_context.is_number());
        return instance = static_cast<float>(json_context.number_value());
    }

    
    template<>
    bool& Serializer::read(const Json& json_context, bool& instance)
    {
        assert(json_context.is_bool());
        return instance = json_context.bool_value();
    }

    
    template<>
    std::string& Serializer::read(const Json& json_context, std::string& instance)
    {
        assert(json_context.is_string());
        return instance = json_context.string_value();
    }
}