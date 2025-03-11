#pragma once
#include "GameComponent.h"
#include "Keys.h"
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

class PlayerControlComponent : public GameComponent {
public:
  int controlMode;
  GameComponent* controlledObject;

  PlayerControlComponent(
    Game* game,
    GameComponent* object,
    int mode
  )
    : 
    GameComponent(game),
    controlMode(mode),
    controlledObject(object)
  {
  }

  /*virtual void Update(ConstData* data = nullptr) override;*/
  virtual void Update() override;
};
