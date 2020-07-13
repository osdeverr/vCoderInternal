//
//  concept.h
//  vCoder
//
//  Created by Nikita Ivanov on 7/13/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#pragma once

namespace vcoder::concepts
{
    /// @brief The Concept class is supposed to be inherited from to explicitly prevent any instaniation of concept classes.
    class Concept
    {
    private:
        Concept() = delete;
        Concept(const Concept&) = delete;
        Concept(Concept&&) = delete;
        
        virtual ~Concept() = 0;
    };
}
