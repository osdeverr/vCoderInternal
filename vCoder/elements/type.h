//
//  type.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/9/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once
#include "basicelement.h"

namespace vcoder::elements
{
    class Type : public BasicElement
    {
    public:
        Type(const std::string& name = "") : BasicElement(name)
        {}
        
        /// @brief Implements BasicElement::type().
        /// @return This element's type
        virtual std::string type() const override
        {
            return "Type";
        }
        
        /// @brief Implements BasicElement::getSerializable().
        /// @return This element's CX serializable
        virtual vcoder::common::ISerializable<nlohmann::json>&& getSpecificSerializable() override
        {
            return vcoder::common::CXSerializable<nlohmann::json, Type>(*this);
        }
        
        CXPROPS(Type) (
                       CXPROP(isType)
                       ) CXPROPS_END;
        
    private:
        bool isType = true;
    };
}
