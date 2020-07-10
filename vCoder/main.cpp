//
//  main.cpp
//  vCoder
//
//  Created by Nikita Ivanov on 7/9/20.
//  Copyright © 2020 osdever. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "elements/elements.h"
#include "common/typedptr.h"
#include "common/serializable.h"
#include "common/json.hpp"

void printout(vcoder::elements::BasicElement& elem, int tabs = 0)
{
    for(int i = 0; i < tabs; i++)
        putchar('\t');
    
    printf("[%s] %s\n", elem.type().c_str(), elem.name().c_str());
    
    elem.forAllChildren([tabs](vcoder::elements::BasicElement& elem) { printout(elem, tabs + 1); });
}

void Serialize(const vcoder::common::ISerializable<nlohmann::json>& obj)
{
    std::cout << obj.serialize().dump(4) << '\n';
}

int main(int argc, const char * argv[]) {
    using namespace vcoder::elements;
    Root root;
    root.addChild(new Function("main"));
    
    auto ns = new Namespace("vcoder");
    ns->addChild(new Function("Run"));
    root.addChild(ns);
    
    printout(root);
    
    std::ifstream f("/Users/osdever/Documents/XCode/vCoder/vCoder/model.json");
    std::stringstream ss;
    ss << f.rdbuf();
    auto mdl = ss.str();
    
    BasicElement* pElement = BasicElement::deserialize(nlohmann::json::parse(mdl));
    printout(*pElement);
    
    delete pElement;
}
