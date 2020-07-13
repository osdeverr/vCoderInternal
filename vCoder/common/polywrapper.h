//
//  polywrapper.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/13/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once
#include <utility>

namespace vcoder::common
{
    /// @brief A simple RAII wrapper for polymorphic class types. Used to return those in a safe manner.
    template<class Base>
    class PolyWrapper
    {
    public:
        /// @brief Constructs a PolyWrapper<Base> instance from a derived class instance. Commonly used in return values as it's implicitly constructible.
        /// @tparam Derived The derived class type
        /// @param instance The instance to construct this object from
        template<class Derived>
        PolyWrapper(Derived&& instance)
        {
            mBase = new Derived(std::move(instance));
        }
        
        /// @brief Overloads operator* to provide pointer-like access.
        Base& operator*()
        {
            return *mBase;
        }
        
        /// @brief Overloads operator-> to provide pointer-like access.
        Base* operator->()
        {
            return mBase;
        }
        
        /// @brief Destroys the PolyWrapper<Base> instance and deallocates the associated memory.
        ~PolyWrapper()
        {
            if(mBase)
                delete mBase;
        }
    private:
        Base* mBase;
    };
}
