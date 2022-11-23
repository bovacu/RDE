//
// Created by borja on 11/22/22.
//

#ifndef RDE_COMPONENT_BASE_H
#define RDE_COMPONENT_BASE_H

struct ComponentBase {
    virtual void setEnabled(bool _enabled) = 0;
    virtual bool isEnabled() = 0;
};

#endif //RDE_COMPONENT_BASE_H
