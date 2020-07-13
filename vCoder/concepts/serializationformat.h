//
//  serializationformat.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/13/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once
#include "concept.h"

namespace vcoder::concepts
{
    /// @brief A concept of a serialization format class conformant to BasicElement serialization requirements.
    /// @note This is basically a subset of the nlohmann::json library's interface as BasicElement uses it by default to serialize.
    ///       Anything implementing this concept will work fine with it, provided you change the typedef to use your format.
    class SerializationFormat : Concept
    {
    public:
        /// @brief Default-constructs the element.
        SerializationFormat();
        
        /// @brief Tries parsing this element's value as T.
        /// @tparam T The type to parse this element's value as
        /// @return The parsed value
        template<class T>
        T get();
        
        /// @brief Accesses a subelement by its string key.
        template<class String>
        SerializationFormat& operator[](const String& key);
        
        /// @brief Adds this value to this element's list (if it's an array).
        /// @param element The element to add
        void push_back(const SerializationFormat& element);
    };
}
