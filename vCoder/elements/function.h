//
//  function.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/9/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once
#include "basicelement.h"
#include <string>

namespace vcoder::elements
{
    /// @brief A function taking some parameters and returning a value.
    class Function : public BasicElement
    {
    public:
        /// @brief Constructs the function element.
        Function(const std::string& name = "") : BasicElement(name)
        {}
        
        /// @brief Implements BasicElement::getSerializable().
        /// @return This element's CX serializable
        virtual vcoder::common::ISerializable<nlohmann::json>&& getSpecificSerializable() override
        {
            return vcoder::common::CXSerializable<nlohmann::json, Function>(*this);
        }
        
        /// @brief Implements BasicElement::type().
        /// @return This element's type
        virtual std::string type() const override
        {
            return "Function";
        }
        
        CXPROPS(Function) (
                       CXPROP(isFunction)
                       ) CXPROPS_END;
        
    private:
        bool isFunction = true;
    };
}
