//
//  elements.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/10/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once

#include "basicelement.h"
#include "root.h"
#include "function.h"
#include "type.h"
#include "namespace.h"

#define VELEM_STR(x) #x

#define VELEM_DECODE(T) \
if(type == VELEM_STR(T)) \
{ \
    ptr = new T(); \
    ptr->getSpecificSerializable().deserializeFrom(data); \
}

namespace vcoder::elements
{
    template<class Format>
    BasicElement* BasicElement::deserialize(const Format& data)
    {
        auto name = data["name"].template get<std::string>();
        auto type = data["type"].template get<std::string>();
        auto specific = data["specific"];
        BasicElement* ptr = nullptr;
        
        VELEM_DECODE(Root);
        VELEM_DECODE(Function);
        VELEM_DECODE(Type);
        VELEM_DECODE(Namespace);
        
        if(ptr)
        {
            ptr->mName = name;
            
            if(data.find("children") != data.end())
                for(auto& child : data["children"])
                    ptr->addChild(BasicElement::deserialize<Format>(child));
        }
        
        return ptr;
    }
}

#undef VELEM_DECODE
#undef VELEM_STR
