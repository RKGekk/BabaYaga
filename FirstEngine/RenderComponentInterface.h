#pragma once

#include <memory>

#include "SceneNode.h"
#include "ActorComponent.h"

class RenderComponentInterface : public ActorComponent {
public:

    virtual std::shared_ptr<SceneNode> VGetSceneNode() = 0;
};