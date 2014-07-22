#ifndef QSE_RENDERCOMPONENTINTERFACE_HPP
#define QSE_RENDERCOMPONENTINTERFACE_HPP

#include "ActorComponent.hpp"

class RenderComponentInterface : public ActorComponent
{
public:
//    virtual ComponentId VGetComponentId(void) const override { return COMPONENT_ID; }

    // RenderComponentInterface
    virtual shared_ptr<SceneNode> VGetSceneNode(void) = 0;
};

#endif