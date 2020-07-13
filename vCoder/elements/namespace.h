//
//  namespace.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/10/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once
#include "basicelement.h"
#include <string>

namespace vcoder::elements
{
    /// @brief A namespace containing any elements.
    class Namespace : public BasicElement
    {
    public:
        /// @brief Constructs the namespace element.
        Namespace(const std::string& name = "") : BasicElement(name)
        {}
        
        /// @brief Implements BasicElement::type().
        /// @return This element's type
        virtual std::string type() const override
        {
            return "Namespace";
        }
        
        /// @brief Implements BasicElement::getSerializable().
        /// @return This element's CX serializable
        virtual common::PolyWrapper<common::ISerializable<ElementSerializationFormat>> getSpecificSerializable() override
        {
            return common::CXSerializable<ElementSerializationFormat, Namespace>(*this);
        }
        
        CXPROPS(Namespace) (
                       CXPROP(isNamespace)
                       ) CXPROPS_END;
        
    private:
        bool isNamespace = true;
    };
}
