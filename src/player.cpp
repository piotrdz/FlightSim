/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* player.cpp
    Zawiera implementację klasy Player. */

#include "player.h"

#include "model.h"
#include "filemanager.h"
#include "map.h"
#include "application.h"

#include <cassert>
#include <cmath>
#include <cstdlib>

using namespace std;

const int Player::MAX_HP = 50;

const float Player::MAX_ACCELERATION = 10.0f;
const float Player::MIN_VELOCITY = 50.0f;
const float Player::MAX_VELOCITY = 300.0f;

const Vector3D Player::MAX_ANGULAR_ACCELERATION = Vector3D(10.0f, 0.5f, 10.0f);
const Vector3D Player::MAX_ANGULAR_VELOCITY = Vector3D(30.0f, 1.0f, 30.0f);

Model* Player::_model = NULL;


Player::Player(Map* pMap)
{
  _map = pMap;

  _updateTimer.setIntervalMsec(2);
  _firingTimer.setIntervalMsec(100);
  _aiTimer.setIntervalMsec(0);

  _updateTimer.reset();
  _firingTimer.reset();
  _aiTimer.reset();

  _team = Team_Blue;
  _controlType = Control_AngularVelocity;
  _hp = Player::MAX_HP;
  _frameVisible = true;
  _fade = 1.0f;
  _ammo = -1;
  _firing = false;

  _ai = false;
  _aiActions = 0;
  _aiState = 0;
  _aiParam = 0.0f;
  _lastAIState = 10000;
  _lastAIParam = 10000.0f;

  reset();
}

Player::~Player()
{
  _map = NULL;
}

void Player::initModel()
{
  assert(_model == NULL);

  FileManager::instance()->registerFile("FighterModel", "data/fighter.ply");
  if (FileManager::instance()->ensureCanRead("FighterModel"))
  {
    _model = new Model("FighterModel");
    if (!_model->load(FileManager::instance()->fileName("FighterModel")))
    {
      Application::instance()->print("Player", "Nie można wczytać modelu myśliwca!");
      Application::instance()->quit(1);
    }
  }
}

void Player::destroyModel()
{
  delete _model;
  _model = NULL;
}

void Player::reset()
{
  _angularAcceleration = Vector3D();
  _angularVelocity = Vector3D();
  _rotation = Rotation();

  _acceleration = Vector3D();
  _velocityValue = Player::MIN_VELOCITY;
  _velocity = Vector3D(0.0f, 0.0f, _velocityValue);
  _position = Vector3D(0.0f, 1.2f * _map->quadSize().y, 0.0f);

  _quadPositionX = 0;
  _quadPositionZ = 0;

  _accelerationControl = 0.0f;
  _angularVelocityControl = Vector3D();
  _angularAccelerationControl = Vector3D();
}

Color Player::color() const
{
  switch (_team)
  {
    case Team_Blue:
      return Color(0.0f, 0.0f, 1.0f);

    case Team_Red:
      return Color(1.0f, 0.0f, 0.0f);
  }

  return Color();
}

float Player::altitude() const
{
  Vector3D qs = _map->quadSize();
  Vector3D ts = _map->quadTileSize();
  int tilePosX = (int)((_position.x + 0.5f * qs.x) / ts.x);
  int tilePosZ = (int)((_position.z + 0.5f * qs.z) / ts.z);
  float mapH = _map->tileValue(_quadPositionX, _quadPositionZ, tilePosX, tilePosZ);
  return _position.y - mapH;
}

Vector3D Player::mapOffset() const
{
  Vector3D qs = _map->quadSize();
  return Vector3D(qs.x * _quadPositionX, 0.0f, qs.z * _quadPositionZ);
}

void Player::setControl(float pAccelerationControl, const Vector3D &pAngularControl)
{
  _accelerationControl = pAccelerationControl;
  if (_controlType == Control_AngularAcceleration)
    _angularAccelerationControl = pAngularControl;
  else if (_controlType == Control_AngularVelocity)
    _angularVelocityControl = pAngularControl;
}

Vector3D Player::angularControl() const
{
  if (_controlType == Control_AngularAcceleration)
    return _angularAccelerationControl;
  else if (_controlType == Control_AngularVelocity)
    return _angularVelocityControl;

  return Vector3D();
}

Vector3D Player::maximumAngularControl() const
{
  if (_controlType == Control_AngularAcceleration)
    return Player::MAX_ANGULAR_ACCELERATION;
  else if (_controlType == Control_AngularVelocity)
    return Player::MAX_ANGULAR_VELOCITY;

  return Vector3D();
}

void Player::checkHit(Bullet* bullet)
{
  if (_hp == 0)
    return;

  Vector3D pos = bullet->position() - actualPosition();

  if (pos.between(_model->boundingBoxMin(), _model->boundingBoxMax()))
  {
    --_hp;
    bullet->setDecayed();

    Application::instance()->print("Player",
      _name + ": trafienie! HP: " + toString<int>(_hp) + "/" +
      toString<int>(Player::MAX_HP));

    if (_ai && ((_aiActions & AI_EvasiveAction) != 0)
        && (_aiState == 0))
    {
      _aiTimer.setInterval(0);
      _aiState = 40;
    }
  }
}

void Player::render(float frameRotation)
{
  glPushMatrix();
  {
    Vector3D pos = actualPosition();
    glTranslatef(pos.x, pos.y, pos.z);

    float matrix[16] = { 0.0f };
    _rotation.reverseToGLMatrix(matrix);
    glMultMatrixf(matrix);

    Color c = color();

    float fighterSpecular[] = { 0.3f * c.r, 0.3f * c.g, 0.3f * c.b, _fade };
    glMaterialfv(GL_FRONT, GL_SPECULAR, fighterSpecular);

    float fighterDiffuse[] = { c.r, c.g, c.b, _fade };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, fighterDiffuse);

    if (_fade < 1.0f)
    {
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glColor4f(1.0f, 1.0f, 1.0f, _fade);
    _model->render();

    if (_frameVisible)
    {
      glRotatef(frameRotation, 0.0f, 1.0f, 0.0f);

      glDepthMask(GL_FALSE);
      glDisable(GL_LIGHTING);

      glColor4f(c.r, c.g, c.b, _fade);

      glLineWidth(3.0f);

      float size = max(max(1.5f * fabs(_model->boundingBoxMin().x),
                           1.5f * fabs(_model->boundingBoxMax().x)),
                       max(1.5f * fabs(_model->boundingBoxMin().y),
                           1.5f * fabs(_model->boundingBoxMax().y)));

      glBegin(GL_LINES);
      {
        glVertex3f(-size, -size, 0.0f);
        glVertex3f( size, -size, 0.0f);

        glVertex3f( size, -size, 0.0f);
        glVertex3f( size,  size, 0.0f);

        glVertex3f( size, size, 0.0f);
        glVertex3f(-size, size, 0.0f);

        glVertex3f(-size,  size, 0.0f);
        glVertex3f(-size, -size, 0.0f);
      }
      glEnd();

      glLineWidth(1.0f);

      glBegin(GL_QUADS);
      {
        float w = 2.0f * size * (_hp / ((float)Player::MAX_HP));
        glVertex3f(-size,  size + 1.0f, 0.0f);
        glVertex3f(-size,  size + 2.5f, 0.0f);
        glVertex3f(-size + w, size + 2.5f, 0.0f);
        glVertex3f(-size + w, size + 1.0f, 0.0f);
      }
      glEnd();

      glEnable(GL_LIGHTING);
      glDepthMask(GL_TRUE);
    }

    if (_fade < 1.0f)
    {
      glDisable(GL_BLEND);
      glDepthMask(GL_TRUE);
    }
  }
  glPopMatrix();
}

void Player::update()
{
  if (_updateTimer.checkTimeout())
  {
    float delta = _updateTimer.timeoutDifference() / 1e9f;

    // Obliczenie przyspieszenia kątowego obrotów własnych

    if (_controlType == Control_AngularAcceleration)
    {
      _angularAcceleration += (_angularAccelerationControl - _angularAcceleration) * delta * 6.5f;

      Vector3D deltaAngularAcceleration = _angularAcceleration - _angularAccelerationControl;
      if (fabs(deltaAngularAcceleration.x) < 0.5f)
        _angularAcceleration.x = _angularAccelerationControl.x;
      if (fabs(deltaAngularAcceleration.y) < 0.5f)
        _angularAcceleration.y = _angularAccelerationControl.y;
      if (fabs(deltaAngularAcceleration.z) < 0.5f)
        _angularAcceleration.z = _angularAccelerationControl.z;
    }
    else if (_controlType == Control_AngularVelocity)
    {
      _angularAcceleration = (_angularVelocityControl - _angularVelocity) * 0.75f;
    }

    _angularAcceleration.clamp(-Player::MAX_ANGULAR_ACCELERATION, Player::MAX_ANGULAR_ACCELERATION);

    // Uaktualnienie prędkości kątowej obrotów własnych

    _angularVelocity += delta * _angularAcceleration;
    _angularVelocity.clamp(-Player::MAX_ANGULAR_VELOCITY, Player::MAX_ANGULAR_VELOCITY);

    if (fabs(_angularAcceleration.x) < 0.1f)
      _angularVelocity.x = 0.0f;
    if (fabs(_angularAcceleration.y) < 0.1f)
      _angularVelocity.y = 0.0f;
    if (fabs(_angularAcceleration.z) < 0.1f)
      _angularVelocity.z = 0.0f;

    // Obliczenie prędkości kątowej przy skręcaniu (pochyleniu samolotu na 1 skrzydło)

    float turnRate = 0.0f;
    if (fabs(_rotation.roll()) > 1.0f)
    {
      float r  = fabs(_rotation.roll());
      if (r > 45.0f)
        r = 45.0f;

      float turnRadius = (_velocityValue * _velocityValue)
                          / (tan(r * PI_180) * 10.0f);

      turnRate = 360.0f / ((2.0f * M_PI * turnRadius) / _velocityValue);

      if (_rotation.roll() < 0.0f)
        turnRate = -turnRate;
    }

    // Uaktualnienie pozycji, prędkości i obrotu

    _velocityValue += delta * _accelerationControl;
    if (_velocityValue > Player::MAX_VELOCITY)
      _velocityValue = Player::MAX_VELOCITY;
    else if (_velocityValue < Player::MIN_VELOCITY)
      _velocityValue = Player::MIN_VELOCITY;

    _velocity = _velocityValue * _rotation.mainAxis();
    _position += delta * _velocity;

    _rotation.rotateLocal(-_angularVelocity.x * delta,
                           _angularVelocity.y * delta,
                          -_angularVelocity.z * delta);

    if (turnRate != 0.0f)
      _rotation.rotateGlobal(0.0f, turnRate * delta, 0.0f);

    // Zmiana obecnego pola po przekroczeniu granic(y) zerowego pola

    Vector3D qs = _map->quadSize();

    if (_position.x > 0.5f * qs.x)
    {
      ++_quadPositionX;
      _position.x -= qs.x;
    }
    else if (_position.x < -0.5f * qs.x)
    {
      --_quadPositionX;
      _position.x += qs.x;
    }

    if (_position.z > 0.5f * qs.z)
    {
      ++_quadPositionZ;
      _position.z -= qs.z;
    }
    else if (_position.z < -0.5f * qs.z)
    {
      --_quadPositionZ;
      _position.z += qs.z;
    }

    // Reakcja na zestrzelenie

    if ((_hp == 0) && (_fade == 1.0f))
    {
      _fade = 0.99f;
    }
    else if (_fade < 1.0f)
    {
      _fade -= delta * 0.3f;
      if (_fade < 0.0f)
        _fade = 0.0f;
    }
  }

  if (_firingTimer.checkTimeout())
  {
    if (_firing && (_ammo != 0))
    {
      Vector3D pos = actualPosition() + Vector3D::normalize(_velocity) * 20.0f;
      Vector3D vel = Vector3D::normalize(_velocity) * (_velocity.length() + 400.0f);
      Vector3D side = _rotation.sideAxis();
      Vector3D up = _rotation.upAxis();
      _createdBullets.push_back(new Bullet(pos, vel, side, up));

      if (_ammo != -1)
        --_ammo;
    }
  }

  if (_ai && _aiTimer.checkTimeout())
  {
    if ((_lastAIState != _aiState) || (_lastAIParam != _aiParam))
    {
      Application::instance()->print(_name, string("AI: stan: ") + toString<int>(_aiState) + " param: " + toString<float>(_aiParam));
      _lastAIState = _aiState;
      _lastAIParam = _aiParam;
    }

    // Lot prosty; wybór następnej fazy
    if (_aiState == 0)
    {
      bool ok = true;
      do
      {
        _aiState = 10 * (rand() % 4);
        ok = true;
        if (_aiState == 10)
          ok = (_aiActions & AI_Acceleration) != 0;
        else if (_aiState == 20)
          ok = (_aiActions & AI_Turning) != 0;
        else if (_aiState == 30)
          ok = (_aiActions & AI_Pitching) != 0;
      } while (!ok);

      _aiTimer.setIntervalMsec(500 + rand() % 4000);
    }
    // Zmiana prędkości
    else if (_aiState < 20)
    {
      if (_aiState == 10)
      {
        _accelerationControl = Player::MAX_ACCELERATION;
        float v08 = Player::MIN_VELOCITY + 0.8f * (Player::MAX_VELOCITY - Player::MIN_VELOCITY);
        if ((_velocity.length() > v08) || (rand() % 2 == 0))
          _accelerationControl *= -1.0f;

        _aiState = 11;
        _aiTimer.setIntervalMsec(3000 + rand() % 7000);
      }
      else if (_aiState == 11)
      {
        _accelerationControl = 0.0f;
        _aiState = 0;
        _aiTimer.setIntervalMsec(2000 + rand() % 3000);
      }
    }
    // Skręcanie
    else if (_aiState < 30)
    {
      // Przechylenie - początek
      if (_aiState == 20)
      {
        _angularAccelerationControl.z = Player::MAX_ANGULAR_ACCELERATION.z;
        if (rand() % 2 == 0)
          _angularAccelerationControl.z *= -1.0f;

        _aiState = 21;
        _aiTimer.setIntervalMsec(1);
      }
      // Przechylenie - kontynuacja i skręcanie
      else if (_aiState == 21)
      {
        if (fabs(_rotation.roll()) > 30.0f)
        {
          _angularAccelerationControl.z = 0.0f;
          _aiState = 22;
          _aiTimer.setIntervalMsec(10000 + rand() % 10000);
        }
        else
        {
          _angularAccelerationControl.z = Player::MAX_ANGULAR_ACCELERATION.z *
                                          (31.0f - fabs(_rotation.roll())) / 31.0f;
          if (_rotation.roll() < 0.0f)
            _angularAccelerationControl.z *= -1.0f;
        }
      }
      // Przechylenie z powrotem do poziomu - początek
      else if (_aiState == 22)
      {
        _angularAccelerationControl.z = Player::MAX_ANGULAR_ACCELERATION.z;
        _aiParam = 1.0f;
        if (_rotation.roll() > 0.0f)
        {
          _angularAccelerationControl.z *= -1.0f;
          _aiParam = -1.0f;
        }

        _aiState = 23;
        _aiTimer.setIntervalMsec(1);
      }
      // Kontynuacja
      else if (_aiState == 23)
      {
        if (((_aiParam < 0.0f) && (_rotation.roll() < 0.5f)) ||
            ((_aiParam > 0.0f) && (_rotation.roll() > -0.5f)))
        {
          _angularAccelerationControl.z = 0.0f;
          _aiState = 0;
          _aiTimer.setIntervalMsec(2000 + rand() % 3000);
        }
        else
        {
          _angularAccelerationControl.z = Player::MAX_ANGULAR_ACCELERATION.z * fabs(_rotation.roll()) / 29.0f;
          if (_rotation.roll() > 0.0f)
            _angularAccelerationControl.z *= -1.0f;
        }
      }
    }
    // Zmiana wysokości
    else if (_aiState < 40)
    {
      // Nachylenie - początek
      if (_aiState == 30)
      {
        _angularAccelerationControl.x = Player::MAX_ANGULAR_ACCELERATION.x;
        _aiParam = _position.y + (rand() % 1000 - 500) / 50.0f;
        if (_aiParam < 1.05f * _map->quadSize().y)
          _aiParam = 1.05f * _map->quadSize().y;

        if (_aiParam < _position.y)
          _angularAccelerationControl.x *= -1.0f;

        _aiState = 31;
        _aiTimer.setIntervalMsec(1);
      }
      // Nachylenie - kontynuacja
      else if (_aiState == 31)
      {
        if (fabs(_rotation.pitch()) > 15.0f)
        {
          _angularAccelerationControl.x = 0.0f;
          _aiState = 32;
          _aiTimer.setIntervalMsec(1);
        }
        else
        {
          _angularAccelerationControl.x = Player::MAX_ANGULAR_ACCELERATION.x *
                                          (16.0f - fabs(_rotation.pitch())) / 16.0f;
          if (_rotation.pitch() < 0.0f)
            _angularAccelerationControl.x *= -1.0f;
        }
      }
      // Wznoszenie/opadanie i powrót do poziomu
      else if (_aiState == 32)
      {
        if ((_rotation.pitch() < 0.0f) && (_position.y < _aiParam))
        {
          _angularAccelerationControl.x = Player::MAX_ANGULAR_ACCELERATION.x;
          _aiState = 33;
          _aiTimer.setIntervalMsec(1);
          _aiParam = 1.0f;
        }
        else if ((_rotation.pitch() > 0.0f) && (_position.y > _aiParam))
        {
          _angularAccelerationControl.x = -Player::MAX_ANGULAR_ACCELERATION.x;
          _aiState = 33;
          _aiTimer.setIntervalMsec(1);
          _aiParam = -1.0f;
        }
      }
      // Kontynuacja
      else if (_aiState == 33)
      {
        if ( ((_aiParam > 0.0f) && (_rotation.pitch() > 0.2f)) ||
             ((_aiParam < 0.0f) && (_rotation.pitch() < -0.2f)) )
        {
          _angularAccelerationControl.x = 0.0f;
          _aiState = 0;
          _aiTimer.setIntervalMsec(2000 + rand() % 3000);
        }
        else
        {
          _angularAccelerationControl.x = Player::MAX_ANGULAR_ACCELERATION.x *
                                            (0.5f + fabs(_rotation.pitch())) / 14.0f;
          if (_rotation.pitch() > 0.0f)
            _angularAccelerationControl.x *= -1.0f;
        }
      }
    }
    // Manewry unikające
    else if (_aiState < 50)
    {
      _aiState = 10 * (1 + rand() % 3);
    }
  }
}

void Player::resetTimers()
{
  _updateTimer.reset();
  _firingTimer.reset();
  _aiTimer.reset();
}

vector<Bullet*> Player::createdBullets()
{
  vector<Bullet*> result = _createdBullets;
  _createdBullets.clear();
  return result;
}
