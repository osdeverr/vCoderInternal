//
//  serializable.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/10/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once
#include "reflection.h"

namespace vcoder::common
{
    /// @brief An interface to serialize and deserialize an object. Implementations usually access an object pointer stored inside.
    /// @tparam Format The serialization format: vCoder usually uses JSON
    /// @remarks Both methods may throw if the implementation decides so.
    template<class Format>
    class ISerializable
    {
    public:
        virtual ~ISerializable() {}
        
        /// @brief Serializes the internal object and returns its serialized representation.
        /// @return The serialized representation of this object
        virtual Format serialize() const = 0;
        
        /// @brief Deserializes the serialized representation into the internal object.
        /// @param data The serialized representation of the object
        virtual void deserializeFrom(const Format& data) = 0;
    };
    
    /// @brief An implementation of @ref ISerializable<Format> using CX to [de]serialize objects.
    /// @tparam Format The serialization format: vCoder usually uses JSON
    /// @tparam T The class to [de]serialize: @b must include CXPROPS
    template<class Format, class T>
    class CXSerializable : public ISerializable<Format>
    {
    public:
        /// @brief Constructs the CXSerializable object from a reference to an object of T.
        /// @param ref The reference to the object to [de]serialize
        CXSerializable(T& ref)
        : mRef(ref) {}
        
        /// @brief An implementation of ISerializable<Format>::serialize.
        virtual Format serialize() const override
        {
            return CX::SerializeObject<Format>(mRef);
        }
        
        /// @brief An implementation of ISerializable<Format>::deserializeFrom.
        /// @param data The serialized representation of the object
        virtual void deserializeFrom(const Format& data) override
        {
            CX::DeserializeObject<T>(data, mRef);
        }
        
    private:
        T& mRef;
    };
}
