/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* player.h
    Contains the Player class, which contains the data describing a player
    and is responsible for updating it. */

#pragma once

#include "config.h"

#include "common.h"
#include "rotation.h"
#include "bullet.h"
#include "object.h"

#include <vector>
#include <string>

class Model;
class Map;


class Player
{
  public:
    enum ControlType
    {
      Control_AngularVelocity,
      Control_AngularAcceleration
    };

    enum Team
    {
      Team_Blue,
      Team_Red
    };

    enum AIActions
    {
      AI_Acceleration  = 0x01,
      AI_Turning       = 0x02,
      AI_Pitching      = 0x04,
      AI_EvasiveAction = 0x08
    };

  public:
    Player(Map *pMap);
    virtual ~Player();

    static void initModel();
    static void destroyModel();

    inline static const Model* model()
      { return _model; }

    void reset();

    inline void setName(const std::string &pName)
      { _name = pName; }
    inline std::string name() const
      { return _name; }

    inline void setTeam(Team pTeam)
      { _team = pTeam; }
    inline Team team() const
      { return _team; }

    bool frameVisible() const
      { return _frameVisible; }
    void setFrameVisible(bool pVisible)
      { _frameVisible = pVisible; }

    Color color() const;

    inline void setControlType(ControlType pControlType)
      { _controlType = pControlType; }
    ControlType controlType() const
      { return _controlType; }

    inline int hp() const
      { return _hp; }

    inline bool destroyed() const
      { return (_hp == 0) && (_fade == 0.0f); }

    inline void setAmmo(int pAmmo)
      { _ammo = pAmmo; }
    inline int ammo() const
      { return _ammo; }

    inline void setFiring(bool pFiring)
      { _firing = pFiring; }
    inline bool firing() const
      { return _firing; }

    inline void setAI(bool pAI)
      { _ai = pAI; _aiState = 0;
         if (pAI) setControlType(Control_AngularAcceleration); }
    inline bool ai() const
      { return _ai; }

    inline void setAIActions(int pActions)
      { _aiActions = pActions; }
    inline int aiActions() const
      { return _aiActions; }


    inline float velocity() const
      { return _velocity.length(); }

    inline float acceleration() const
      { return _acceleration.length(); }

    inline int mapPositionX() const
      { return _quadPositionX; }

    inline int mapPositionZ() const
      { return _quadPositionZ; }

    Vector3D mapOffset() const;

    inline const Vector3D& positionOffset() const
      { return _position; }

    inline Vector3D actualPosition() const
      { return mapOffset() + _position; }

    inline const Rotation& rotation() const
      { return _rotation; }

    inline float height() const
      { return _position.y; }

    inline void setHeading(float pHeading)
      { _rotation.setAngles(0.0f, -pHeading, 0.0f); }

    void setMapPosition(int pX, int pZ)
      { _quadPositionX = pX; _quadPositionZ = pZ; }

    void setPositionOffset(const Vector3D &pOffset)
      { _position = pOffset; }

    float altitude() const;

    void setControl(float pAccelerationControl, const Vector3D &pAngularControl);

    float accelerationControl() const
      { return _accelerationControl; }

    inline float maximumAccelerationControl() const
      { return MAX_ACCELERATION; }

    Vector3D angularControl() const;

    Vector3D maximumAngularControl() const;

    void checkHit(Bullet *bullet);

    void render(float frameRotation = 0.0f);

    void update();

    void resetTimers();

    std::vector<Bullet*> createdBullets();

  private:
    static Model *_model;

    Timer _updateTimer, _firingTimer, _aiTimer;

    Map *_map;

    std::string _name;

    Team _team;
    bool _frameVisible;
    ControlType _controlType;
    int _hp;
    float _fade;
    int _ammo;
    bool _firing;
    std::vector<Bullet*> _createdBullets;

    bool _ai;
    int _aiActions;
    int _aiState, _lastAIState;
    float _aiParam, _lastAIParam;

    Vector3D _angularAcceleration;
    Vector3D _angularVelocity;
    Rotation _rotation;

    Vector3D _acceleration;
    Vector3D _velocity;
    float _velocityValue;
    Vector3D _position;
    int _quadPositionX, _quadPositionZ;

    float _accelerationControl;
    Vector3D _angularVelocityControl;
    Vector3D _angularAccelerationControl;

    static const int MAX_HP;

    static const float MAX_ACCELERATION;
    static const float MIN_VELOCITY;
    static const float MAX_VELOCITY;

    static const Vector3D MAX_ANGULAR_ACCELERATION;
    static const Vector3D MAX_ANGULAR_VELOCITY;
};
