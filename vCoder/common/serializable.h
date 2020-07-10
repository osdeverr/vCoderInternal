//
//  serializable.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/10/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once
#include "typedptr.h"
#include "reflection.h"

namespace vcoder::common
{
    template<class Format>
    class ISerializable
    {
    public:
        virtual ~ISerializable() {}
        
        virtual Format serialize() const = 0;
        virtual void deserializeFrom(const Format& data) = 0;
    };
    
    template<class Format, class T>
    class CXSerializable : public ISerializable<Format>
    {
    public:
        CXSerializable(T& ref)
        : mRef(ref) {}
        
        virtual Format serialize() const override
        {
            return CX::SerializeObject<Format>(mRef);
        }
        
        virtual void deserializeFrom(const Format& data) override
        {
            CX::DeserializeObject<T>(data, mRef);
        }
        
    private:
        T& mRef;
    };
}
