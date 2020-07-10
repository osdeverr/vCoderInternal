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
#include "../common/json.hpp"

namespace vcoder::elements
{
    /// @brief The base class for all vCoder elements.
    class BasicElement
    {
    private:
        template<class Format>
        class Serializer : public vcoder::common::ISerializable<Format>
        {
        public:
            Serializer(BasicElement* element)
            : mElement(element) {}
            
            virtual Format serialize() const override
            {
                Format f;
                f["name"] = mElement->name();
                f["type"] = mElement->type();
                f["specific"] = mElement->getSpecificSerializable().serialize();
                
                for(auto& child : mElement->mChildren)
                    f["children"].push_back(child->getSerializable().serialize());
                    
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
        /// @remarks This is basically RAII: the entire hierarchy is guaranteed to be destroyed.
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
        template<class Format>
        static BasicElement* deserialize(const Format& data);
        
        /// @brief Gets the CX serializable associated with this element's generic and type-specific data.
        /// @return The CX serializable
        vcoder::common::ISerializable<nlohmann::json>&& getSerializable()
        {
            return Serializer<nlohmann::json>(this);
        }
        
        /// @brief Gets the CX serializable associated with this element's type-specific data.
        /// @return The CX serializable
        virtual vcoder::common::ISerializable<nlohmann::json>&& getSpecificSerializable() = 0;
        
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
