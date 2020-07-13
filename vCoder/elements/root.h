//
//  root.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/9/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once
#include "basicelement.h"

namespace vcoder::elements
{
    /// @brief The root element in the hierarchy.
    class Root : public BasicElement
    {
    public:
        /// @brief Constructs the root element, internally named _ROOT.
        Root() : BasicElement("_ROOT")
        {}
        
        /// @brief Implements BasicElement::getSerializable().
        /// @return This element's CX serializable
        virtual vcoder::common::ISerializable<ElementSerializationFormat>&& getSpecificSerializable() override
        {
            return vcoder::common::CXSerializable<ElementSerializationFormat, Root>(*this);
        }
        
        /// @brief Implements BasicElement::type().
        /// @return This element's type
        virtual std::string type() const override
        {
            return "Root";
        }
        
        CXPROPS(Root) (
                        CXPROP(isRoot)
        ) CXPROPS_END;
        
    private:
        bool isRoot = true;
    };
}
