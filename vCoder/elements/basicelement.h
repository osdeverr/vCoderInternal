//
//  basicelement.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/9/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once
#include <string>
#include <list>
#include <iostream>

#include "../common/serializable.h"
#include "../common/polywrapper.h"
#include "../common/json.hpp"

namespace vcoder::elements
{
    /// @brief The base class for all vCoder elements.
    class BasicElement
    {
    public:
        /// @brief Represents the serialization format to use for elements.
        /// @remarks We're using JSON: anything with the same interface would be alright though.
        using SerializationFormat = nlohmann::json;
        
        /// @brief Represents a pointer to the serializable object, here only for convenience.
        using SerializablePtr = common::PolyWrapper<common::ISerializable<SerializationFormat>>;
        
    private:
        template<class Format>
        class Serializer : public common::ISerializable<Format>
        {
        public:
            Serializer(BasicElement* element)
            : mElement(element) {}
            
            virtual Format serialize() const override
            {
                Format f;
                f["name"] = mElement->name();
                f["type"] = mElement->type();
                f["specific"] = mElement->getSpecificSerializable()->serialize();
                
                for(auto& child : mElement->mChildren)
                    f["children"].push_back(child->getSerializable()->serialize());
                    
                return f;
            }
            
            virtual void deserializeFrom(const Format& data) override
            {
                throw std::runtime_error("Operation not supported");
            }
        private:
            BasicElement* mElement;
        };
        
    protected:
        /// @brief Constructs a BasicElement instance.
        /// @param name The name of this element
        BasicElement(const std::string& name)
        {
            mName = name;
            mParent = nullptr;
        }
        
    public:
        /// @brief This is basically RAII: the entire hierarchy is guaranteed to be destroyed.
        /// @remarks The user is still responsible for deleting the root object - one could wrap it into a smart pointer for convenience...
        virtual ~BasicElement()
        {
            if(mParent)
                mParent->removeChild(this);
            
            for(auto& child : mChildren)
            {
                child->mParent = nullptr;
                delete child;
            }
        }
        
        BasicElement(const BasicElement&) = delete;
        BasicElement(BasicElement&&) = default;
        
        /// @brief Adds a child to this element's internal list.
        /// @param child The pointer to the child element: @b MUST be heap allocated
        void addChild(BasicElement* child)
        {
            if(!child)
                return;
            
            mChildren.push_back(child);
            child->mParent = this;
        }
        
        /// @brief Removes a child from this element's internal list.
        /// @param child The pointer to the child element
        void removeChild(BasicElement* child)
        {
            mChildren.remove(child);
            child->mParent = nullptr;
        }
        
        /// @brief Gets the type of this element.
        /// @return This element's type
        virtual std::string type() const = 0;
        
        /// @brief Deserializes a BasicElement from serialized data.
        /// @param data The data to recreate a BasicElement from
        static BasicElement* deserialize(const SerializationFormat& data);
        
        /// @brief Gets the CX serializable associated with this element's generic and type-specific data.
        /// @return The CX serializable
        SerializablePtr getSerializable()
        {
            return Serializer<SerializationFormat>(this);
        }
        
        /// @brief Gets the CX serializable associated with this element's type-specific data.
        /// @remarks One should @b not generally use it directly! Use getSerializable instead.
        /// @return The CX serializable
        virtual SerializablePtr getSpecificSerializable() = 0;
        
        /// @brief Gets the name of this element.
        /// @return This element's name
        std::string name() const
        {
            return mName;
        }
        
        /// @brief Gets a pointer to this element's parent.
        /// @return This element's parent
        BasicElement* parent()
        {
            return mParent;
        }
        
        /// @brief Gets a const pointer to this element's parent.
        /// @return This element's parent
        const BasicElement* parent() const
        {
            return mParent;
        }
        
        /// @brief Invokes the specified callback for all children.
        /// @param callback The callback to invoke: must accept one argument of type BasicElement&
        template<class F>
        void forAllChildren(const F& callback)
        {
            for(auto& child : mChildren)
                callback(*child);
        }
    private:
        std::string mName;
        BasicElement* mParent;
        std::list<BasicElement*> mChildren;
    };
}
