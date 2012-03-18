/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* simulation.cpp
    Contains the implementation of Simulation class. */

#include "simulation.h"

#include "fractal.h"
#include "render.h"
#include "decorator.h"
#include "label.h"
#include "filemanager.h"
#include "application.h"
#include "fontengine.h"
#include "menu.h"
#include "bindings.h"
#include "model.h"
#include "settings.h"

#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include <GL/gl.h>

using namespace std;

const float Simulation::VISIBLE_RANGE = 10000.0f;
const float Simulation::RADAR_RANGE = 5000.0f;


Simulation::Simulation(Widget* pParent,
                       const string &pName)
  : Widget(pParent, pName.empty() ? genericName("Simulation") : pName, 1000)
{
  enableInput();

  _map = new Map(&_fractal, name() + "_Map");
  _map->setScale(Vector3D(20.0f, 800.0f, 20.0f));

  _player = new Player(_map);
  _player->setName(_("Player"));
  _player->setTeam(Player::Team_Blue);
  _player->setControlType(Player::Control_AngularVelocity);
  _player->setAI(false);
  _player->setFrameVisible(false);

  _hudMode = Hud_Full;
  _hudFont = Decorator::instance()->getFont(FT_Small);
  _bigHudFont = Decorator::instance()->getFont(FT_Normal);

  _updateTimer.setIntervalMsec(5);

  _messageTimer.setIntervalMsec(50);


  _initializingLabel = new Label(this, _("Initializing the map..."),
                                 Decorator::instance()->getFont(FT_Big),
                                 AL_Center, false,
                                 Decorator::instance()->getColor(C_Text),
                                 true);
  _initializingLabel->show();

  _collisionLabel = new Label(this, "Collision!",
                              Decorator::instance()->getFont(FT_SimulationWarning),
                              AL_Center, false,
                              Color(1.0f, 0.0f, 0.0f),
                              true);

  _messageLabel = new Label(this, "", Decorator::instance()->getFont(FT_Big),
                            AL_Bottom | AL_HCenter, true,
                            Decorator::instance()->getColor(C_Text));

  vector<string> menuItems;
  menuItems.push_back(_("Return to the game"));
  menuItems.push_back(_("Settings"));
  menuItems.push_back(_("Exit the game"));
  _menu = new Menu(this, "", menuItems, true, name() + "_Menu");

  BindingManager *b = BindingManager::instance();
  b->registerKey("RollNegative", KeyBinding(SDLK_KP4));
  b->registerKey("RollPositive", KeyBinding(SDLK_KP6));
  b->registerKey("PitchNegative", KeyBinding(SDLK_KP8));
  b->registerKey("PitchPositive", KeyBinding(SDLK_KP2));
  b->registerKey("YawNegative", KeyBinding(SDLK_KP1));
  b->registerKey("YawPositive", KeyBinding(SDLK_KP3));
  b->registerKey("Accelerate", KeyBinding(SDLK_KP_PLUS));
  b->registerKey("Decelerate", KeyBinding(SDLK_KP_MINUS));
  b->registerKey("ViewYNegative", KeyBinding(SDLK_1));
  b->registerKey("ViewYPositive", KeyBinding(SDLK_4));
  b->registerKey("ViewXNegative", KeyBinding(SDLK_2));
  b->registerKey("ViewXPositive", KeyBinding(SDLK_3));
  b->registerKey("ViewZoomIn", KeyBinding(SDLK_5));
  b->registerKey("ViewZoomOut", KeyBinding(SDLK_6));
  b->registerKey("Fog", KeyBinding(SDLK_f));
  b->registerKey("Hud", KeyBinding(SDLK_h));
  b->registerKey("View", KeyBinding(SDLK_v));
  b->registerKey("GameMenu", KeyBinding(SDLK_ESCAPE));
  b->registerKey("Fire", KeyBinding(SDLK_SPACE));

  b->registerJoystickAxis("Roll", JoystickAxisBinding(0));
  b->registerJoystickAxis("Pitch", JoystickAxisBinding(1));
  b->registerJoystickAxis("Yaw", JoystickAxisBinding(3));
  b->registerJoystickAxis("Acceleration", JoystickAxisBinding(2));

  b->registerJoystickButton("Fire", JoystickButtonBinding(0));

  Settings *s = Settings::instance();
  s->registerSetting<string>("PlayerName", _player->name());
  s->registerSetting<int>("DisplayQuality", Quality_Medium);
  s->registerSetting<float>("FOV", 45.0f);

  reset();
}

Simulation::~Simulation()
{
  _menu = NULL;
  _initializingLabel = NULL;
  _collisionLabel = NULL;
  _messageLabel = NULL;

  delete _player;
  _player = NULL;

  deleteEnemyPlayers();

  deleteBullets();

  delete _map;
  _map = NULL;
}

void Simulation::deleteEnemyPlayers()
{
  for (list<Player*>::iterator it = _enemyPlayers.begin();
       it != _enemyPlayers.end(); ++it)
  {
    delete *it;
  }
  _enemyPlayers.clear();
}

void Simulation::deleteBullets()
{
  for (list<Bullet*>::iterator it = _bullets.begin();
       it != _bullets.end(); ++it)
  {
    delete *it;
  }
  _bullets.clear();
}

void Simulation::loadSettings()
{
  Settings *s = Settings::instance();
  _player->setName(s->setting<string>("PlayerName"));
  _displayQuality = (DisplayQuality)s->setting<int>("DisplayQuality");
  _fov = s->setting<float>("FOV");
}

void Simulation::reset()
{
  _map->clear();
  _initializing = true;
  _initializingLabel->show();

  _collisionLabel->hide();
  _menu->hide();

  _updateTimer.setEnabled(true);

  _player->reset();

  deleteEnemyPlayers();
  _enemiesDestroyed = false;

  _fog = true;
  _viewMode = View_Cockpit;
  _hudMode = Hud_Full;
  _outsideViewAnglesAcc = _outsideViewAngles = Vector3D();
  _outsideViewZoom = 30.0f;
  if (_player->model() != NULL)
    _outsideViewZoom = 2.5f * _player->model()->boundingBoxDiagonal();

  loadSettings();

  deleteBullets();
}

void Simulation::resetTimers()
{
  _updateTimer.reset();
  _player->resetTimers();
  if (_simulationType == Simulation_Game)
  {
    for (list<Player*>::iterator it = _enemyPlayers.begin();
         it != _enemyPlayers.end();
         ++it)
    {
      (*it)->resetTimers();
    }
  }
}

void Simulation::addEnemies(int count, int aiActions)
{
  for (int i = 1; i <= count; ++i)
  {
    Player *enemy = new Player(_map);
    enemy->setTeam(Player::Team_Red);
    enemy->setName(string(_("Computer ")) + toString<int>(i));
    enemy->setAI(true);
    enemy->setAIActions(aiActions);

    int mapPosX = 1 + rand() % 2;
    if (rand() % 2 == 0)
      mapPosX *= -1;

    int mapPosZ = 1 + rand() % 2;
    if (rand() % 2 == 0)
      mapPosZ *= -1;

    enemy->setMapPosition(mapPosX, mapPosZ);

    Vector3D pos;
    pos.x= -0.5f * _map->quadSize().x + rand() % ((int)_map->quadSize().x);
    pos.y = _player->height();
    pos.z = -0.5f * _map->quadSize().z + rand() % ((int)_map->quadSize().z);

    enemy->setPositionOffset(pos);

    float heading = rand() % 360;
    enemy->setHeading(heading);

    _enemyPlayers.push_back(enemy);
  }
}

void Simulation::init()
{
  _map->createWorkerThread();

  float lightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
  float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  float lightSpecular[] = { 0.4f, 0.4f, 0.4f, 1.0f };

  glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);

  glFogfv(GL_FOG_COLOR, Decorator::instance()->getColor(C_SimulationBackground));
}

void Simulation::render()
{
  if (_initializing)
  {
    Decorator::instance()->renderFrame(_initializingFrame);

    float p = _map->initProgress();
    Rect progressBar;
    progressBar.x = _initializingFrame.x + Decorator::instance()->getDefaultMargin();
    progressBar.y = _initializingFrame.yMid();
    progressBar.w = p * (_initializingFrame.w - 2.0f * Decorator::instance()->getDefaultMargin());
    progressBar.h = 0.2f * _initializingFrame.h;

    glColor3fv(Decorator::instance()->getColor(C_Text));
    glRectf(progressBar.x, progressBar.y, progressBar.x2(), progressBar.y2());

    return;
  }

  Render::instance()->begin3D(_fov, 0.1f, VISIBLE_RANGE);

  glLoadIdentity();

  // Matrix for switching Z-axis direction
  const float switchZMatrix[16] =
  {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f,-1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };

  glMultMatrixf(switchZMatrix);

  if (_viewMode == View_Outside)
  {
    // Translation of outside view
    glTranslatef(0.0f, 0.0f, _outsideViewZoom);

    // Rotation of outside view
    glRotatef(_outsideViewAngles.x, 1.0f, 0.0f, 0.0f);
    glRotatef(_outsideViewAngles.y, 0.0f, 1.0f, 0.0f);
  }

  // Rotation inverse to the player ("ground" turns the opposite direction to the player)
  float rotationMatrix[16] = { 0.0f };
  _player->rotation().toGLMatrix(rotationMatrix);
  glMultMatrixf(rotationMatrix);

  // Player translation
  glTranslatef(-_player->positionOffset().x,
               -_player->positionOffset().y,
               -_player->positionOffset().z);

  glColor3f(1.0f, 1.0f, 1.0f);

  // Light is a "sun" coming from fixed direction
  float lightPosition[] = { -50.0f, 50.0f, 0.0f, 0.0f };
  glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);

  // Ground material
  const float mapSpecular[] = { 0.05f, 0.11f, 0.0f, 0.04f };
  glMaterialfv(GL_FRONT, GL_SPECULAR, mapSpecular);

  const float mapDiffuse[] = { 0.15f, 0.38f, 0.16f, 1.0f };
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mapDiffuse);


  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT1);

  if (_fog)
  {
    float f = min(_map->quadSize().x, _map->quadSize().z);
    float h = _player->height() - _map->quadSize().y;
    float fogMin = sqrt(0.25f * f*f + h*h);
    float fogMax = 2.0f * f;

    glFogf(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, fogMin);
    glFogf(GL_FOG_END, fogMax);

    glEnable(GL_FOG);
  }

  /* Only 25 fields of the map are rendered:

      2 2 2 2 2
      2 1 1 1 2
      2 1 0 1 2
      2 1 1 1 2
      2 2 2 2 2

      0 - "zero" field
   */


  // Field coordinates to be drawn:

  // Level "1"
  const int LEVEL_1[][2] =
  {
    { 1, 0}, { 1,  1}, {0,  1}, {-1,  1},
    {-1, 0}, {-1, -1}, {0, -1}, { 1, -1},
  };
  const int LEVEL_1_SIZE = sizeof(LEVEL_1) / sizeof(*LEVEL_1);

  // Level "2"
  const int LEVEL_2[][2] =
  {
    { 2,  0}, { 2,  1}, { 2,  2}, { 1,  2},
    { 0,  2}, {-1,  2}, {-2,  2}, {-2,  1},
    {-2,  0}, {-2, -1}, {-2, -2}, {-1, -2},
    { 0, -2}, { 1, -2}, { 2, -2}, { 2, -1}
  };
  const int LEVEL_2_SIZE = sizeof(LEVEL_2) / sizeof(*LEVEL_2);


  Map::DetailLevel level0Detail = Map::DetailHigh;
  if (_displayQuality == Quality_Low)
    level0Detail = Map::DetailMedium;

  _map->renderQuad(_player->mapPositionX(), _player->mapPositionZ(), level0Detail);


  Vector3D s = _map->quadSize();

  Map::DetailLevel level1Detail = Map::DetailMedium;
  if ((_displayQuality == Quality_High) ||
      (_displayQuality == Quality_VeryHigh))
    level1Detail = Map::DetailHigh;

  for (int i = 0; i < LEVEL_1_SIZE; ++i)
  {
    glPushMatrix();
    {
      glTranslatef(s.x * LEVEL_1[i][0], 0.0f, s.z * LEVEL_1[i][1]);
      _map->renderQuad(_player->mapPositionX() + LEVEL_1[i][0],
                       _player->mapPositionZ() + LEVEL_1[i][1], level1Detail);
    }
    glPopMatrix();
  }

  Map::DetailLevel level2Detail = Map::DetailLow;
  if (_displayQuality == Quality_VeryHigh)
    level2Detail = Map::DetailHigh;

  for (int i = 0; i < LEVEL_2_SIZE; ++i)
  {
    glPushMatrix();
    {
      glTranslatef(s.x * LEVEL_2[i][0], 0.0f, s.z * LEVEL_2[i][1]);
      _map->renderQuad(_player->mapPositionX() + LEVEL_2[i][0],
                       _player->mapPositionZ() + LEVEL_2[i][1], level2Detail);
    }
    glPopMatrix();
  }

  if (_fog)
    glDisable(GL_FOG);


  // Drawing other element beside the map
  glPushMatrix();
  {
    Vector3D mapOffset = _player->mapOffset();
    glTranslatef(-mapOffset.x, -mapOffset.y, -mapOffset.z);

    // Model of the player's plane in outside view
    if (_viewMode == View_Outside)
    {
      _player->render();
    }

    // Enemies and bullets
    if (_simulationType == Simulation_Game)
    {
      for (list<Player*>::iterator it = _enemyPlayers.begin();
           it != _enemyPlayers.end(); ++it)
      {
        Vector3D deltaPos = (*it)->actualPosition() - _player->actualPosition();
        if (deltaPos.length() < VISIBLE_RANGE)
        {
          (*it)->render(-(*it)->rotation().heading() + _player->rotation().heading());
        }
      }

      glDisable(GL_LIGHT1);
      glDisable(GL_LIGHTING);

      for (list<Bullet*>::iterator it = _bullets.begin();
           it != _bullets.end(); ++it)
      {
        (*it)->render();
      }

    }
  }
  glPopMatrix();

  glDisable(GL_LIGHT1);
  glDisable(GL_LIGHTING);

  Render::instance()->end3D();

  // HUD
  renderHud();
}

void Simulation::renderHud()
{
  if ((_hudMode == Hud_None) || (_viewMode == View_Outside))
    return;

  float margin = Decorator::instance()->getDefaultMargin();
  glColor3f(0.0f, 1.0f, 0.0f);

  // Crosshair at the center
  glPushMatrix();
  {
    glTranslatef(0.5f * geometry().w, 0.5f * geometry().h, 0.0f);

    glBegin(GL_LINES);
    {
      glVertex2f(-4.5f * margin, 0.0f);
      glVertex2f(-3.0f * margin, 0.0f);

      glVertex2f(-3.0f * margin, 3.0f * margin);
      glVertex2f(0.0f, 0.0f);

      glVertex2f(0.0f, 0.0f);
      glVertex2f(3.0f * margin, 3.0f * margin);

      glVertex2f(3.0f * margin, 0.0f);
      glVertex2f(4.5f * margin, 0.0f);
    }
    glEnd();

    // Labels around the center

    FontMetrics bigMetrics(_bigHudFont);

    int w = bigMetrics.width(_heightString);
    _bigHudFont->renderText(_heightString, Point(-12.5f * margin - w, -1.5f * bigMetrics.height()));

    w = bigMetrics.width(_altitudeString);
    _bigHudFont->renderText(_altitudeString, Point(-12.5f * margin - w, 0.5f * bigMetrics.height()));

    _bigHudFont->renderText(_velocityString, Point(12.5f * margin, -1.5f * bigMetrics.height()));

    if (_simulationType == Simulation_Game)
      _bigHudFont->renderText(_ammoString, Point(12.5f * margin, 0.5f * bigMetrics.height()));
  }
  glPopMatrix();

  if (_hudMode == Hud_Minimal)
    return;

  FontMetrics metrics(_hudFont);
  int fontHeight = metrics.height();

  // Direction markers

  glPushMatrix();
  {
    glTranslatef(0.0f, 2.0f * margin, 0.0f);
    float fovX =  geometry().w * _fov / geometry().h;
    int minH = (int)floor(_player->rotation().heading() - 0.25f * fovX);
    int maxH = (int)ceil(_player->rotation().heading() + 0.25f * fovX);
    float dx = geometry().w / fovX;
    float x = 0.5f * geometry().w - (_player->rotation().heading() - minH) * dx;

    glBegin(GL_LINES);
    {
      for (int h = minH; h <= maxH; ++h)
      {
        glVertex2f(x, 0.0f);
        if (h % 10 == 0)
          glVertex2f(x, 2.0f * margin);
        else
          glVertex2f(x, margin);

       x += dx;
      }

      glVertex2f(0.5f * geometry().w - 0.5f * margin, -margin);
      glVertex2f(0.5f * geometry().w, -0.5f * margin);

      glVertex2f(0.5f * geometry().w, -0.5f * margin);
      glVertex2f(0.5f * geometry().w + 0.5f * margin, -margin);
    }
    glEnd();

    x = geometry().w / 2.0f - (_player->rotation().heading() - minH) * dx;
    for (int h = minH; h <= maxH; ++h)
    {
      if (h % 10 == 0)
      {
        int hValue = h;
        if (h >= 360)
          hValue = h - 360;
        else if (h < 0)
          hValue = 360 + h;

        stringstream s;
        s.fill('0');
        s.width(2);
        s << hValue / 10;
        int w = metrics.width(s.str());
        _hudFont->renderText(s.str(), Point(x - w / 2.0f, 2.5f * margin));
      }
      x += dx;
    }
  }
  glPopMatrix();

  // Pitch markers

  glPushMatrix();
  {
    glTranslatef(0.5f * geometry().w, 0.5f * geometry().h, 0.0f);
    glRotatef(-_player->rotation().roll(), 0.0f, 0.0f, 1.0f);

    int minP = (int)floor(_player->rotation().pitch() - 0.4f * _fov);
    int maxP = (int)ceil(_player->rotation().pitch() + 0.4f * _fov);
    float dy = geometry().h / _fov;
    float y = (_player->rotation().pitch() - minP) * dy;

    glBegin(GL_LINES);
    {
      for (int p = minP; p <= maxP; ++p)
      {
        if (p % 10 == 0)
        {
          glVertex2f(-12.0f * margin, y);
          glVertex2f(-2.0f * margin, y);

          glVertex2f( 2.0f * margin, y);
          glVertex2f( 12.0f * margin, y);

          if (p > 0)
          {
            glVertex2f(-12.0f * margin, y);
            glVertex2f(-12.0f * margin, y + margin);

            glVertex2f( 12.0f * margin, y);
            glVertex2f( 12.0f * margin, y + margin);
          }
          else if (p < 0)
          {
            glVertex2f(-12.0f * margin, y);
            glVertex2f(-12.0f * margin, y - margin);

            glVertex2f( 12.0f * margin, y);
            glVertex2f( 12.0f * margin, y - margin);
          }
        }

        y -= dy;
      }
    }
    glEnd();

    y = (_player->rotation().pitch() - minP) * dy;
    for (int p = minP; p <= maxP; ++p)
    {
      if (p % 10 == 0)
      {
        int pValue = p;
        if (p > 90)
          pValue = 90 - p;
        else if (p < -90)
          pValue = -90 - p;

        stringstream s;
        s.fill('0');
        s.width(2);
        s.setf(ios_base::showpos);
        s << pValue;
        int w = metrics.width(s.str());
        _hudFont->renderText(s.str(), Point(-0.5f * w, y - 0.5f * fontHeight));
      }
      y -= dy;
    }
  }
  glPopMatrix();

  if (_simulationType == Simulation_Normal)
    return;

  // Radar
  glPushMatrix();
  {
    float radarSize = min(0.1f * geometry().w, 0.1f * geometry().h);

    float margin = Decorator::instance()->getDefaultMargin();

    glScissor((int)(geometry().w - 2.0f * radarSize - margin),
              (int)(margin), (int)(2.0f * radarSize), (int)(2.0f * radarSize));

    glEnable(GL_SCISSOR_TEST);

    glTranslatef(geometry().w - 2.0f * radarSize - margin,
                 geometry().h - 2.0f * radarSize - margin,
                 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 1.0f, 0.0f, 0.5f);

    glBegin(GL_QUADS);
    {
      glVertex2f(0.0f, 0.0f);
      glVertex2f(2.0f * radarSize, 0.0f);
      glVertex2f(2.0f * radarSize, 2.0f * radarSize);
      glVertex2f(0.0f, 2.0f * radarSize);
    }
    glEnd();

    glTranslatef(radarSize, radarSize, 0.0f);

    glPushMatrix();
    {
      Vector3D qs = _map->quadSize();

      Vector3D offset = _player->positionOffset() * (radarSize / RADAR_RANGE);

      // Grid
      glPushMatrix();
      {
        glRotatef(-_player->rotation().heading(), 0.0f, 0.0f, 1.0f);
        glTranslatef(-offset.x, offset.z, 0.0f);

        int nGrids = 1 + (int)max(1.5f * ceil(RADAR_RANGE / qs.x),
                                  1.5f * ceil(RADAR_RANGE / qs.z));
        if (nGrids % 2 == 1)
          ++nGrids;

        glColor3f(0.5f, 0.5f, 0.5f);

        glBegin(GL_LINES);
        {
          for (int grid = -nGrids / 2; grid <= nGrids / 2; ++grid)
          {
            glVertex2f(grid * qs.x * (radarSize / RADAR_RANGE), -2.0f * radarSize);
            glVertex2f(grid * qs.x * (radarSize / RADAR_RANGE),  2.0f * radarSize);

            glVertex2f(-2.0f * radarSize, grid * qs.z * (radarSize / RADAR_RANGE));
            glVertex2f( 2.0f * radarSize, grid * qs.z * (radarSize / RADAR_RANGE));
          }
        }
        glEnd();
      }
      glPopMatrix();

      // Player's dot
      glColor3fv(_player->color());

      glPointSize(4.0f);

      glBegin(GL_POINTS);
      {
        glVertex2d(0.0f, 0.0f);
      }
      glEnd();

      // Enemy positions
      glPushMatrix();
      {
        for (list<Player*>::iterator it = _enemyPlayers.begin();
           it != _enemyPlayers.end(); ++it)
        {
          Vector3D deltaPos = (*it)->actualPosition() - _player->actualPosition();
          deltaPos.y = 0.0f;
          deltaPos.rotate(-_player->rotation().heading(), Vector3D(0.0f, 1.0f, 0.0f));

          glColor3fv((*it)->color());

          deltaPos *= (radarSize / RADAR_RANGE);

          if ((fabs(deltaPos.x) < radarSize) && (fabs(deltaPos.z) < radarSize))
          {
            glBegin(GL_POINTS);
            {
              glVertex2f(deltaPos.x, -deltaPos.z);
            }
            glEnd();
          }
          else
          {
            deltaPos.clamp(Vector3D(-radarSize, 0.0f, -radarSize),
                           Vector3D(radarSize, 0.0f, radarSize));
            Vector3D dir = Vector3D::normalize(deltaPos);
            Vector3D side = dir.crossProduct(Vector3D(0.0f, 1.0f, 0.0f));
            side.normalize();

            Vector3D v1 = deltaPos - 2.5f * side;
            Vector3D v2 = deltaPos + 2.5f * side;
            Vector3D v3 = deltaPos + 5.0f * dir;

            glDisable(GL_SCISSOR_TEST);

            glBegin(GL_TRIANGLES);
            {
              glVertex2f(v1.x, -v1.z);
              glVertex2f(v2.x, -v2.z);
              glVertex2f(v3.x, -v3.z);
            }
            glEnd();

            glEnable(GL_SCISSOR_TEST);
          }
        }
      }
      glPopMatrix();

      glPointSize(1.0f);
    }
    glPopMatrix();

    glDisable(GL_SCISSOR_TEST);
  }
  glPopMatrix();
}

void Simulation::update()
{
  if (_initializing)
  {
    if (_map->init())
    {
      _initializingLabel->hide();
      resetTimers();
      _initializing = false;
    }
  }

  _map->update();

  if ((_initializing) || (!_updateTimer.enabled()))
    return;

  _player->update();

  // Update of enemies
  if (_simulationType == Simulation_Game)
  {
    for (list<Player*>::iterator it = _enemyPlayers.begin();
          it != _enemyPlayers.end();
          ++it)
    {
      (*it)->update();
    }
  }

  if (_updateTimer.checkTimeout())
  {
    float delta = _updateTimer.timeoutDifference() / 1e9f;

    stringstream hs;
    hs << "H: " << fixed << setprecision(2) << _player->height();
    _heightString = hs.str();

    stringstream as;
    as << "A: " << fixed << setprecision(2) << _player->altitude();
    _altitudeString = as.str();

    stringstream vs;
    vs << "V: " << fixed << setprecision(2) << _player->velocity();
    _velocityString = vs.str();

    if (_simulationType == Simulation_Game)
    {
      stringstream amms;
      amms << "Am: ";
      if (_player->ammo() == -1)
        amms << "inf.";
      else
        amms << _player->ammo();
      _ammoString = amms.str();
    }

    // Update of view angles
    _outsideViewAngles += delta * _outsideViewAnglesAcc;

    // Collision detection
    if (_player->height() <= _map->quadSize().y)
    {
      if (_player->altitude() <= 0.0f)
      {
        _collisionLabel->show();
        _menu->show();
        _updateTimer.setEnabled(false);
      }
    }

    if (_simulationType == Simulation_Game)
    {
      // Adding new bullets
      vector<Bullet*> newBullets = _player->createdBullets();
      for (unsigned int i = 0; i < newBullets.size(); ++i)
        _bullets.push_back(newBullets[i]);

      // Update of bullets
      for (list<Bullet*>::iterator it = _bullets.begin();
           it != _bullets.end(); ++it)
      {
        if ((*it)->decayed())
        {
          delete *it;
          it = _bullets.erase(it);
        }
        else
        {
          (*it)->update(delta);
          _player->checkHit(*it);

          for (list<Player*>::iterator jt = _enemyPlayers.begin();
               jt != _enemyPlayers.end(); ++jt)
          {
            (*jt)->checkHit(*it);
          }
        }
      }

      // Update of enemy destruction
      for (list<Player*>::iterator jt = _enemyPlayers.begin();
          jt != _enemyPlayers.end(); ++jt)
      {
        if ((*jt)->destroyed())
        {
          displayMessage(replace(string(_("Enemy %1 shot down!")), "%1", (*jt)->name()));

          delete *jt;
          jt = _enemyPlayers.erase(jt);
        }
      }

      if (_enemyPlayers.empty() && (!_enemiesDestroyed))
      {
        _enemiesDestroyed = true;
        displayMessage(_("You have eliminated all enemies. Congratulations!"));
      }
    }
  }

  if (_messageTimer.checkTimeout())
  {
    Color newColor = _messageLabel->color();
    newColor.a -= (_messageTimer.timeoutDifference() / 1e9f) * 0.2f;
    if (newColor.a < 0.0f)
    {
      newColor.a = 1.0f;
      _messageLabel->hide();
      _messageTimer.setEnabled(false);
    }

    _messageLabel->setColor(newColor);
  }
}

void Simulation::resizeEvent()
{
  _initializingFrame.w = geometry().w * 0.4f;
  _initializingFrame.h = geometry().h * 0.25f;
  _initializingFrame.x = geometry().x + 0.5f * (geometry().w - _initializingFrame.w);
  _initializingFrame.y = geometry().y + 0.5f * (geometry().h - _initializingFrame.h);

  Rect labelGeometry = _initializingFrame;
  labelGeometry.h *= 0.5f;
  _initializingLabel->setGeometry(labelGeometry);

  _collisionLabel->setGeometry(geometry());
  _messageLabel->setGeometry(geometry());

  Rect menuGeometry;
  menuGeometry.w = geometry().w * 0.5f;
  menuGeometry.h = geometry().h * 0.5f;
  menuGeometry.x = geometry().x + 0.5f * (geometry().w - menuGeometry.w);
  menuGeometry.y = geometry().y + 0.5f * (geometry().h - menuGeometry.h);
  _menu->setGeometry(menuGeometry);
}

void Simulation::showEvent()
{
  Color c = Decorator::instance()->getColor(C_SimulationBackground);
  glClearColor(c.r, c.g, c.b, c.a);

  resetTimers();
}

void Simulation::hideEvent()
{
  Color c = Decorator::instance()->getColor(C_InterfaceBackground);
  glClearColor(c.r, c.g, c.b, c.a);
}

void Simulation::keyboardDownEvent(KeyboardDownEvent* e)
{
  if (_initializing)
    return;

  BindingManager *b = BindingManager::instance();
  int keysym = e->event().keysym.sym;
  if (b->findKey("RollNegative").check(keysym))
  {
    Vector3D angularControl = _player->angularControl();
    angularControl.z = -_player->maximumAngularControl().z;

    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findKey("RollPositive").check(keysym))
  {
    Vector3D angularControl = _player->angularControl();
    angularControl.z = _player->maximumAngularControl().z;

    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findKey("PitchNegative").check(keysym))
  {
    Vector3D angularControl = _player->angularControl();
    angularControl.x = -_player->maximumAngularControl().x;

    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findKey("PitchPositive").check(keysym))
  {
    Vector3D angularControl = _player->angularControl();
    angularControl.x = _player->maximumAngularControl().x;

    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findKey("YawNegative").check(keysym))
  {
    Vector3D angularControl = _player->angularControl();
    angularControl.y = -_player->maximumAngularControl().y;
    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findKey("YawPositive").check(keysym))
  {
    Vector3D angularControl = _player->angularControl();
    angularControl.y = _player->maximumAngularControl().y;

    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findKey("Accelerate").check(keysym))
  {
    _player->setControl( _player->maximumAccelerationControl(),
                         _player->angularControl());
  }
  else if (b->findKey("Decelerate").check(keysym))
  {
    _player->setControl(-_player->maximumAccelerationControl(),
                         _player->angularControl());
  }
  else if (b->findKey("ViewYNegative").check(keysym))
  {
    _outsideViewAnglesAcc.y = -30.0f;
  }
  else if (b->findKey("ViewYPositive").check(keysym))
  {
    _outsideViewAnglesAcc.y = 30.0f;
  }
  else if (b->findKey("ViewXNegative").check(keysym))
  {
    _outsideViewAnglesAcc.x = -30.0f;
  }
  else if (b->findKey("ViewXPositive").check(keysym))
  {
    _outsideViewAnglesAcc.x = 30.0f;
  }
  else if (b->findKey("Fog").check(keysym))
  {
    _fog = !_fog;
    if (_fog)
      displayMessage(_("Fog: on"));
    else
      displayMessage(_("Fog: off"));
  }
  else if (b->findKey("Hud").check(keysym))
  {
    if (_hudMode == Hud_Full)
    {
      displayMessage(_("HUD: minimal"));
      _hudMode = Hud_Minimal;
    }
    else if (_hudMode == Hud_Minimal)
    {
      displayMessage(_("HUD: off"));
      _hudMode = Hud_None;
    }
    else if (_hudMode == Hud_None)
    {
      displayMessage(_("HUD: full"));
      _hudMode = Hud_Full;
    }
  }
  else if (b->findKey("View").check(keysym))
  {
    if (_viewMode == View_Cockpit)
    {
      displayMessage(_("View: outside"));
      _viewMode = View_Outside;
    }
    else if (_viewMode == View_Outside)
    {
      displayMessage(_("View: cockpit"));
      _viewMode = View_Cockpit;
    }
  }
  else if (b->findKey("GameMenu").check(keysym))
  {
    _menu->show();

    _updateTimer.setEnabled(false);

    e->stop();
  }

  if (_simulationType == Simulation_Game)
  {
    if (b->findKey("Fire").check(keysym))
    {
      _player->setFiring(true);
    }
  }
}

void Simulation::keyboardUpEvent(KeyboardUpEvent* e)
{
  if (_initializing || _menu->visible())
    return;

  BindingManager *b = BindingManager::instance();
  int keysym = e->event().keysym.sym;
  if (b->findKey("RollNegative").check(keysym) ||
      b->findKey("RollPositive").check(keysym))
  {
    Vector3D angularControl = _player->angularControl();
    angularControl.z = 0.0f;
    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findKey("PitchNegative").check(keysym) ||
           b->findKey("PitchPositive").check(keysym))
  {
    Vector3D angularControl = _player->angularControl();
    angularControl.x = 0.0f;
    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findKey("YawNegative").check(keysym) ||
           b->findKey("YawPositive").check(keysym))
  {
    Vector3D angularControl = _player->angularControl();
    angularControl.y = 0.0f;
    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findKey("Accelerate").check(keysym) ||
           b->findKey("Decelerate").check(keysym))
  {
    _player->setControl(0.0f, _player->angularControl());
  }
  else if (b->findKey("ViewZoomIn").check(keysym))
  {
    _outsideViewZoom -= 1.0f;
  }
  else if (b->findKey("ViewZoomOut").check(keysym))
  {
    _outsideViewZoom += 1.0f;
  }
  else if (b->findKey("ViewYNegative").check(keysym) ||
           b->findKey("ViewYPositive").check(keysym))
  {
    _outsideViewAnglesAcc.y = 0.0f;
  }
  else if (b->findKey("ViewXNegative").check(keysym) ||
           b->findKey("ViewXPositive").check(keysym))
  {
    _outsideViewAnglesAcc.x = 0.0f;
  }

  if (_simulationType == Simulation_Game)
  {
    if (b->findKey("Fire").check(keysym))
    {
      _player->setFiring(false);
    }
  }
}

void Simulation::joystickAxisMotionEvent(JoystickAxisMotionEvent* e)
{
  if (_initializing || _menu->visible())
    return;

  float value = e->event().value;
  if (abs(e->event().value) < 1500)
    value = 0;

  value /= 32768.0f;

  BindingManager *b = BindingManager::instance();
  if (b->findJoystickAxis("Roll").axis() == e->event().axis)
  {
    if (b->findJoystickAxis("Roll").inverted())
      value = -value;
    Vector3D angularControl = _player->angularControl();
    angularControl.z = value * _player->maximumAngularControl().z;
    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findJoystickAxis("Pitch").axis() == e->event().axis)
  {
    if (b->findJoystickAxis("Pitch").inverted())
      value = -value;
    Vector3D angularControl = _player->angularControl();
    angularControl.x = value * _player->maximumAngularControl().x;
    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findJoystickAxis("Yaw").axis() == e->event().axis)
  {
    if (b->findJoystickAxis("Yaw").inverted())
      value = -value;
    Vector3D angularControl = _player->angularControl();
    angularControl.y = value * _player->maximumAngularControl().y;
    _player->setControl(_player->accelerationControl(),
                        angularControl);
  }
  else if (b->findJoystickAxis("Acceleration").axis() == e->event().axis)
  {
    if (b->findJoystickAxis("Pitch").inverted())
      value = -value;
    _player->setControl(value * _player->maximumAccelerationControl(),
                        _player->angularControl());
  }
}

void Simulation::joystickButtonDownEvent(JoystickButtonDownEvent* e)
{
  if (_initializing || _menu->visible())
    return;

  if (_simulationType == Simulation_Game)
  {
    BindingManager *b = BindingManager::instance();
    if (b->findJoystickButton("Fire").button() == e->event().button)
    {
      _player->setFiring(true);
    }
  }
}

void Simulation::joystickButtonUpEvent(JoystickButtonUpEvent* e)
{
  if (_initializing || _menu->visible())
    return;

  if (_simulationType == Simulation_Game)
  {
    BindingManager *b = BindingManager::instance();
    if (b->findJoystickButton("Fire").button() == e->event().button)
    {
      _player->setFiring(false);
    }
  }
}

void Simulation::joystickHatEvent(JoystickHatEvent* e)
{
  if (_initializing || _menu->visible())
    return;

  if (e->event().value == SDL_HAT_CENTERED)
  {
    _outsideViewAnglesAcc.x = _outsideViewAnglesAcc.y = 0.0f;
  }
  else if (e->event().value & SDL_HAT_RIGHT)
  {
    _outsideViewAnglesAcc.y = 30.0f;
  }
  else if (e->event().value & SDL_HAT_LEFT)
  {
    _outsideViewAnglesAcc.y = -30.0f;
  }
  else if (e->event().value & SDL_HAT_UP)
  {
    _outsideViewAnglesAcc.x = 30.0f;
  }
  else if (e->event().value & SDL_HAT_DOWN)
  {
    _outsideViewAnglesAcc.x = -30.0f;
  }
}

void Simulation::settingsDialogFinished()
{
  loadSettings();
  _updateTimer.setEnabled(true);
  resetTimers();
}

void Simulation::childEvent(Widget *sender, int parameter)
{
  if (sender == _menu)
  {
    if ( (((parameter == Menu::ItemChosen) && (_menu->index() == 0)) || (parameter == Menu::EscapePressed)) &&
        (!_collisionLabel->visible()) )
    {
      _menu->hide();
      _updateTimer.setEnabled(true);
      resetTimers();
    }
    else if (parameter == Menu::ItemChosen)
    {
      if (_menu->index() == 1)
      {
        _menu->hide();
        _updateTimer.setEnabled(false);
        sendChildEvent(Action_Settings);
      }
      else if (_menu->index() == 2)
      {
        _menu->hide();
        _updateTimer.setEnabled(false);
        sendChildEvent(Action_EndGame);
      }
    }
  }
}

void Simulation::displayMessage(const std::string& message)
{
  _messageLabel->setText(message);
  _messageLabel->show();
  Color c = _messageLabel->color();
  c.a = 1.0f;
  _messageLabel->setColor(c);
  _messageTimer.reset();
  _messageTimer.setEnabled(true);
}
