/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* map.cpp
    Zawiera implementację klasy Map i jej klas wewnętrznych. */

#include "map.h"

#include "fractal.h"

#include <cmath>
#include <cstring>
#include <sstream>
#include <cassert>

#include <iostream>

#include <GL/gl.h>
#include <GL/glext.h>

#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4

using namespace std;

PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;

void Map::Quad::filterValues(float v0, float &v1, float &v2, float &v3, float v4)
{
  float dv = v4 - v0;
  v1 = v0 + 0.25f * dv;
  v2 = v0 + 0.50f * dv;
  v3 = v0 + 0.75f * dv;
}

void Map::Quad::generate(Quad* neighbors[4], const Vector3D &scale,
                         Fractal *fractal)
{
  fractal->clear();

  if (neighbors[0] != NULL)
  {
    for (int i = 0; i < 1+DETAIL_HIGH_COUNT; ++i)
      fractal->setValue(i, 0, neighbors[0]->_values[i][DETAIL_HIGH_COUNT]);
  }
  if (neighbors[1] != NULL)
  {
    for (int i = 0; i < 1+DETAIL_HIGH_COUNT; ++i)
      fractal->setValue(DETAIL_HIGH_COUNT, i, neighbors[1]->_values[0][i]);
  }
  if (neighbors[2] != NULL)
  {
    for (int i = 0; i < 1+DETAIL_HIGH_COUNT; ++i)
      fractal->setValue(i, DETAIL_HIGH_COUNT, neighbors[2]->_values[i][0]);
  }
  if (neighbors[3] != NULL)
  {
    for (int i = 0; i < 1+DETAIL_HIGH_COUNT; ++i)
      fractal->setValue(0, i, neighbors[3]->_values[DETAIL_HIGH_COUNT][i]);
  }

  fractal->generate(seed());

  assert(fractal->size() == 1+DETAIL_HIGH_COUNT);

  for (int x = 0; x < 1+DETAIL_HIGH_COUNT; ++x)
  {
    for (int z = 0; z < 1+DETAIL_HIGH_COUNT; ++z)
    {
      _values[x][z] = fractal->value(x, z);
    }
  }

  // Wygładzanie dla nowo wygenerowanych krawędzi

  if (neighbors[0] == NULL)
  {
    for (int i = 0; i+4 < 1+DETAIL_HIGH_COUNT; i += 4)
    {
      filterValues(_values[i  ][0],
                   _values[i+1][0],
                   _values[i+2][0],
                   _values[i+3][0],
                   _values[i+4][0]);
    }
  }

  if (neighbors[1] == NULL)
  {
    for (int i = 0; i+4 < 1+DETAIL_HIGH_COUNT; i += 4)
    {
      filterValues(_values[DETAIL_HIGH_COUNT][i  ],
                   _values[DETAIL_HIGH_COUNT][i+1],
                   _values[DETAIL_HIGH_COUNT][i+2],
                   _values[DETAIL_HIGH_COUNT][i+3],
                   _values[DETAIL_HIGH_COUNT][i+4]);
    }
  }

  if (neighbors[2] == NULL)
  {
    for (int i = 0; i+4 < 1+DETAIL_HIGH_COUNT; i += 4)
    {
      filterValues(_values[i  ][DETAIL_HIGH_COUNT],
                   _values[i+1][DETAIL_HIGH_COUNT],
                   _values[i+2][DETAIL_HIGH_COUNT],
                   _values[i+3][DETAIL_HIGH_COUNT],
                   _values[i+4][DETAIL_HIGH_COUNT]);
    }
  }

  if (neighbors[3] == NULL)
  {
    for (int i = 0; i+4 < 1+DETAIL_HIGH_COUNT; i += 4)
    {
      filterValues(_values[0][i  ],
                   _values[0][i+1],
                   _values[0][i+2],
                   _values[0][i+3],
                   _values[0][i+4]);
    }
  }

  const int sH = DETAIL_HIGH_COUNT;

  float dx = -0.5f * scale.x * sH;
  float dz = -0.5f * scale.z * sH;

  for (int x = 0; x < sH; ++x)
  {
    for (int z = 0; z < sH; ++z)
    {
      Vector3D v1(dx + scale.x * x,     scale.y * _values[x][z],     dz + scale.z * z);
      Vector3D v2(dx + scale.x * (x+1), scale.y * _values[x+1][z],   dz + scale.z * z);
      Vector3D v3(dx + scale.x * (x+1), scale.y * _values[x+1][z+1], dz + scale.z * (z+1));
      Vector3D v4(dx + scale.x * x,     scale.y * _values[x][z+1],   dz + scale.z * (z+1));

      _detailHighVertices[6 * (x * sH + z)]     = v1;
      _detailHighVertices[6 * (x * sH + z) + 1] = v2;
      _detailHighVertices[6 * (x * sH + z) + 2] = v3;

      _detailHighVertices[6 * (x * sH + z) + 3] = v3;
      _detailHighVertices[6 * (x * sH + z) + 4] = v4;
      _detailHighVertices[6 * (x * sH + z) + 5] = v1;
    }
  }

  const int sM = DETAIL_MEDIUM_COUNT;

  for (int x = 0; x < sM; ++x)
  {
    for (int z = 0; z < sM; ++z)
    {
      _detailMediumVertices[6 * (x * sM + z)    ] = _detailHighVertices[6 * ( 2*x    * sH + 2*z  )    ];
      _detailMediumVertices[6 * (x * sM + z) + 1] = _detailHighVertices[6 * ((2*x+1) * sH + 2*z  ) + 1];
      _detailMediumVertices[6 * (x * sM + z) + 2] = _detailHighVertices[6 * ((2*x+1) * sH + 2*z+1) + 2];

      _detailMediumVertices[6 * (x * sM + z) + 3] = _detailHighVertices[6 * ((2*x+1) * sH + 2*z+1) + 3];
      _detailMediumVertices[6 * (x * sM + z) + 4] = _detailHighVertices[6 * ( 2*x    * sH + 2*z+1) + 4];
      _detailMediumVertices[6 * (x * sM + z) + 5] = _detailHighVertices[6 * ( 2*x    * sH + 2*z  ) + 5];
    }
  }

  const int sL = DETAIL_LOW_COUNT;

  for (int x = 0; x < sL; ++x)
  {
    for (int z = 0; z < sL; ++z)
    {
      _detailLowVertices[6 * (x * sL + z)    ] = _detailHighVertices[6 * ( 4*x    * sH + 4*z  )    ];
      _detailLowVertices[6 * (x * sL + z) + 1] = _detailHighVertices[6 * ((4*x+3) * sH + 4*z  ) + 1];
      _detailLowVertices[6 * (x * sL + z) + 2] = _detailHighVertices[6 * ((4*x+3) * sH + 4*z+3) + 2];

      _detailLowVertices[6 * (x * sL + z) + 3] = _detailHighVertices[6 * ((4*x+3) * sH + 4*z+3) + 3];
      _detailLowVertices[6 * (x * sL + z) + 4] = _detailHighVertices[6 * ( 4*x    * sH + 4*z+3) + 4];
      _detailLowVertices[6 * (x * sL + z) + 5] = _detailHighVertices[6 * ( 4*x    * sH + 4*z  ) + 5];
    }
  }
}

void Map::Quad::calculateNormals(Quad* neighbors[4], const Vector3D &scale)
{
  Vector3D normalsTable[1+DETAIL_HIGH_COUNT][1+DETAIL_HIGH_COUNT];

  const int sH = DETAIL_HIGH_COUNT;

  for (int x = 0; x < sH; ++x)
  {
    for (int z = 0; z < sH; ++z)
    {
      Vector3D v1(scale.x *  x,    scale.y * _values[x][z],     scale.z *  z);
      Vector3D v2(scale.x * (x+1), scale.y * _values[x+1][z],   scale.z *  z);
      Vector3D v3(scale.x * (x+1), scale.y * _values[x+1][z+1], scale.z * (z+1));
      Vector3D v4(scale.x *  x,    scale.y * _values[x][z+1],   scale.z * (z+1));

      Vector3D n1 = Vector3D::crossProduct(v1 - v2, v3 - v2);
      Vector3D n2 = Vector3D::crossProduct(v3 - v4, v1 - v4);

      normalsTable[x][z]     += n1 + n2;
      normalsTable[x+1][z]   += n1;
      normalsTable[x+1][z+1] += n1 + n2;
      normalsTable[x][z+1]   += n2;
    }
  }

  // Krawędzie z sąsiednich pól

  if (neighbors[0] != NULL)
  {
    for (int x = 0; x < sH; ++x)
    {
      int z = sH-1;

      Vector3D v1(scale.x *  x,    scale.y * neighbors[0]->_values[x][z],     scale.z *  z);
      Vector3D v2(scale.x * (x+1), scale.y * neighbors[0]->_values[x+1][z],   scale.z *  z);
      Vector3D v3(scale.x * (x+1), scale.y * neighbors[0]->_values[x+1][z+1], scale.z * (z+1));
      Vector3D v4(scale.x *  x,    scale.y * neighbors[0]->_values[x][z+1],   scale.z * (z+1));

      Vector3D n1 = Vector3D::crossProduct(v1 - v2, v3 - v2);
      Vector3D n2 = Vector3D::crossProduct(v3 - v4, v1 - v4);

      normalsTable[x  ][0] += n2;
      normalsTable[x+1][0] += n1 + n2;
    }
  }

  if (neighbors[1] != NULL)
  {
    for (int z = 0; z < sH; ++z)
    {
      int x = 0;

      Vector3D v1(scale.x *  x,    scale.y * neighbors[1]->_values[x][z],     scale.z *  z);
      Vector3D v2(scale.x * (x+1), scale.y * neighbors[1]->_values[x+1][z],   scale.z *  z);
      Vector3D v3(scale.x * (x+1), scale.y * neighbors[1]->_values[x+1][z+1], scale.z * (z+1));
      Vector3D v4(scale.x *  x,    scale.y * neighbors[1]->_values[x][z+1],   scale.z * (z+1));

      Vector3D n1 = Vector3D::crossProduct(v1 - v2, v3 - v2);
      Vector3D n2 = Vector3D::crossProduct(v3 - v4, v1 - v4);

      normalsTable[sH][z  ] += n1 + n2;
      normalsTable[sH][z+1] += n2;
    }
  }

  if (neighbors[2] != NULL)
  {
    for (int x = 0; x < sH; ++x)
    {
      int z = 0;

      Vector3D v1(scale.x *  x,    scale.y * neighbors[2]->_values[x][z],     scale.z *  z);
      Vector3D v2(scale.x * (x+1), scale.y * neighbors[2]->_values[x+1][z],   scale.z *  z);
      Vector3D v3(scale.x * (x+1), scale.y * neighbors[2]->_values[x+1][z+1], scale.z * (z+1));
      Vector3D v4(scale.x *  x,    scale.y * neighbors[2]->_values[x][z+1],   scale.z * (z+1));

      Vector3D n1 = Vector3D::crossProduct(v1 - v2, v3 - v2);
      Vector3D n2 = Vector3D::crossProduct(v3 - v4, v1 - v4);

      normalsTable[x  ][sH] += n1 + n2;
      normalsTable[x+1][sH] += n1;
    }
  }

  if (neighbors[3] != NULL)
  {
    for (int z = 0; z < sH; ++z)
    {
      int x = sH-1;

      Vector3D v1(scale.x *  x,    scale.y * neighbors[3]->_values[x][z],     scale.z *  z);
      Vector3D v2(scale.x * (x+1), scale.y * neighbors[3]->_values[x+1][z],   scale.z *  z);
      Vector3D v3(scale.x * (x+1), scale.y * neighbors[3]->_values[x+1][z+1], scale.z * (z+1));
      Vector3D v4(scale.x *  x,    scale.y * neighbors[3]->_values[x][z+1],   scale.z * (z+1));

      Vector3D n1 = Vector3D::crossProduct(v1 - v2, v3 - v2);
      Vector3D n2 = Vector3D::crossProduct(v3 - v4, v1 - v4);

      normalsTable[0][z  ] += n1;
      normalsTable[0][z+1] += n1 + n2;
    }
  }

  for (int x = 0; x < 1+sH; ++x)
  {
    for (int z = 0; z < 1+sH; ++z)
      normalsTable[x][z].normalize();
  }

  for (int x = 0; x < sH; ++x)
  {
    for (int z = 0; z < sH; ++z)
    {
      _detailHighNormals[6 * (x * sH + z)    ] = normalsTable[x  ][z  ];
      _detailHighNormals[6 * (x * sH + z) + 1] = normalsTable[x+1][z  ];
      _detailHighNormals[6 * (x * sH + z) + 2] = normalsTable[x+1][z+1];

      _detailHighNormals[6 * (x * sH + z) + 3] = normalsTable[x+1][z+1];
      _detailHighNormals[6 * (x * sH + z) + 4] = normalsTable[x  ][z+1];
      _detailHighNormals[6 * (x * sH + z) + 5] = normalsTable[x  ][z  ];
    }
  }

  const int sM = DETAIL_MEDIUM_COUNT;

  for (int x = 0; x < sM; ++x)
  {
    for (int z = 0; z < sM; ++z)
    {
      _detailMediumNormals[6 * (x * sM + z)    ] = _detailHighNormals[6 * ( 2*x    * sH + 2*z      )];
      _detailMediumNormals[6 * (x * sM + z) + 1] = _detailHighNormals[6 * ((2*x+1) * sH + 2*z  ) + 1];
      _detailMediumNormals[6 * (x * sM + z) + 2] = _detailHighNormals[6 * ((2*x+1) * sH + 2*z+1) + 2];

      _detailMediumNormals[6 * (x * sM + z) + 3] = _detailHighNormals[6 * ((2*x+1) * sH + 2*z+1) + 3];
      _detailMediumNormals[6 * (x * sM + z) + 4] = _detailHighNormals[6 * ( 2*x    * sH + 2*z+1) + 4];
      _detailMediumNormals[6 * (x * sM + z) + 5] = _detailHighNormals[6 * ( 2*x    * sH + 2*z  ) + 5];
    }
  }

  const int sL = DETAIL_LOW_COUNT;

  for (int x = 0; x < sL; ++x)
  {
    for (int z = 0; z < sL; ++z)
    {
      _detailLowNormals[6 * (x * sL + z)    ] = _detailHighNormals[6 * ( 4*x    * sH + 4*z  )    ];
      _detailLowNormals[6 * (x * sL + z) + 1] = _detailHighNormals[6 * ((4*x+3) * sH + 4*z  ) + 1];
      _detailLowNormals[6 * (x * sL + z) + 2] = _detailHighNormals[6 * ((4*x+3) * sH + 4*z+3) + 2];

      _detailLowNormals[6 * (x * sL + z) + 3] = _detailHighNormals[6 * ((4*x+3) * sH + 4*z+3) + 3];
      _detailLowNormals[6 * (x * sL + z) + 4] = _detailHighNormals[6 * ( 4*x    * sH + 4*z+3) + 4];
      _detailLowNormals[6 * (x * sL + z) + 5] = _detailHighNormals[6 * ( 4*x    * sH + 4*z  ) + 5];
    }
  }
}

void Map::Quad::createVBO()
{
  const int sH = DETAIL_HIGH_COUNT;
  const int sM = DETAIL_MEDIUM_COUNT;
  const int sL = DETAIL_LOW_COUNT;

  glGenBuffersARB(1, &_detailHighVerticesVBO);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailHighVerticesVBO);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, 6 * sH * sH * 3 * sizeof(float),
                  _detailHighVertices, GL_STATIC_DRAW_ARB);

  glGenBuffersARB(1, &_detailHighNormalsVBO);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailHighNormalsVBO);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, 6 * sH * sH * 3 * sizeof(float),
                  _detailHighNormals, GL_STATIC_DRAW_ARB);

  glGenBuffersARB(1, &_detailMediumVerticesVBO);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailMediumVerticesVBO);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, 6 * sM * sM * 3 * sizeof(float),
                  _detailMediumVertices, GL_STATIC_DRAW_ARB);

  glGenBuffersARB(1, &_detailMediumNormalsVBO);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailMediumNormalsVBO);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, 6 * sM * sM * 3 * sizeof(float),
                  _detailMediumNormals, GL_STATIC_DRAW_ARB);

  glGenBuffersARB(1, &_detailLowVerticesVBO);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailLowVerticesVBO);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, 6 * sL * sL * 3 * sizeof(float),
                  _detailLowVertices, GL_STATIC_DRAW_ARB);

  glGenBuffersARB(1, &_detailLowNormalsVBO);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailLowNormalsVBO);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, 6 * sL * sL * 3 * sizeof(float),
                  _detailLowNormals, GL_STATIC_DRAW_ARB);
}

void Map::Quad::render(DetailLevel detailLevel) const
{
  glEnable(GL_VERTEX_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  int arraySize = 0;
  if (detailLevel == DetailHigh)
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailHighVerticesVBO);
    glVertexPointer(3, GL_FLOAT, 0, NULL);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailHighNormalsVBO);
    glNormalPointer(GL_FLOAT, 0, NULL);

    arraySize = 6 * DETAIL_HIGH_COUNT * DETAIL_HIGH_COUNT;
  }
  else if (detailLevel == DetailMedium)
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailMediumVerticesVBO);
    glVertexPointer(3, GL_FLOAT, 0, NULL);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailMediumNormalsVBO);
    glNormalPointer(GL_FLOAT, 0, NULL);

    arraySize = 6 * DETAIL_MEDIUM_COUNT * DETAIL_MEDIUM_COUNT;
  }
  else
  {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailLowVerticesVBO);
    glVertexPointer(3, GL_FLOAT, 0, NULL);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _detailLowNormalsVBO);
    glNormalPointer(GL_FLOAT, 0, NULL);

    arraySize = 6 * DETAIL_LOW_COUNT * DETAIL_LOW_COUNT;
  }

  glDrawArrays(GL_TRIANGLES, 0, arraySize);

  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_VERTEX_ARRAY);
}

void Map::Quad::destroyVBO()
{
  glDeleteBuffersARB(1, &_detailHighVerticesVBO);
  glDeleteBuffersARB(1, &_detailHighNormalsVBO);

  glDeleteBuffersARB(1, &_detailMediumVerticesVBO);
  glDeleteBuffersARB(1, &_detailMediumNormalsVBO);

  glDeleteBuffersARB(1, &_detailLowVerticesVBO);
  glDeleteBuffersARB(1, &_detailLowNormalsVBO);
}

float Map::Quad::value(int x, int z) const
{
  if ((x < 0) || (z < 0) || (x >= DETAIL_HIGH_COUNT) || (z >= DETAIL_HIGH_COUNT))
    return 0.0f;
  return _values[x][z];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Map::Worker::Worker()
{
  _exitCode = -1;

  _exitCodeMutex = SDL_CreateMutex();
  _scheduledTaskMutex = SDL_CreateMutex();
  _finishedTaskMutex = SDL_CreateMutex();

  _scheduledTaskCond = SDL_CreateCond();
}

Map::Worker::~Worker()
{
  SDL_mutexV(_exitCodeMutex);
  SDL_DestroyMutex(_exitCodeMutex);
  _exitCodeMutex = NULL;

  SDL_mutexV(_scheduledTaskMutex);
  SDL_DestroyMutex(_scheduledTaskMutex);
  _scheduledTaskMutex = NULL;

  SDL_mutexV(_finishedTaskMutex);
  SDL_DestroyMutex(_finishedTaskMutex);
  _finishedTaskMutex = NULL;

  SDL_DestroyCond(_scheduledTaskCond);
  _scheduledTaskCond = NULL;
}

void Map::Worker::kill(int exitCode)
{
  SDL_mutexP(_exitCodeMutex);
  {
    _exitCode = exitCode;
  }
  SDL_mutexV(_exitCodeMutex);
}

int Map::Worker::getExitCode()
{
  int value = 0;

  SDL_mutexP(_exitCodeMutex);
  {
    value = _exitCode;
  }
  SDL_mutexV(_exitCodeMutex);

  return value;
}

void Map::Worker::scheduleTask(const Map::WorkerTask &task)
{
  SDL_mutexP(_scheduledTaskMutex);
  {
    _scheduledTasks.push(task);

    SDL_CondSignal(_scheduledTaskCond);
  }
  SDL_mutexV(_scheduledTaskMutex);
}

Map::WorkerTask Map::Worker::getScheduledTask()
{
  WorkerTask result;

  SDL_mutexP(_scheduledTaskMutex);
  {
    if (!_scheduledTasks.empty())
    {
      result = _scheduledTasks.front();
      _scheduledTasks.pop();
    }
    else
    {
      int status = SDL_CondWaitTimeout(_scheduledTaskCond,
                                       _scheduledTaskMutex, 50);
      if (status == 0)
      {
        result = _scheduledTasks.front();
        _scheduledTasks.pop();
      }
    }
  }
  SDL_mutexV(_scheduledTaskMutex);

  return result;
}

Map::WorkerTask Map::Worker::finishedTask()
{
  WorkerTask result;

  SDL_mutexP(_finishedTaskMutex);
  {
    if (!_finishedTasks.empty())
    {
      result = _finishedTasks.front();
      _finishedTasks.pop();
    }
  }
  SDL_mutexV(_finishedTaskMutex);

  return result;
}

void Map::Worker::addFinishedTask(const Map::WorkerTask &task)
{
  SDL_mutexP(_finishedTaskMutex);
  {
    _finishedTasks.push(task);
  }
  SDL_mutexV(_finishedTaskMutex);
}

int Map::Worker::run(void *data)
{
  Worker *instance = (Worker*)(data);

  int exitCode = -1;

  for (;;)
  {
    exitCode = instance->getExitCode();
    if (exitCode != -1)
      break;

    WorkerTask task = instance->getScheduledTask();

    if (!task.valid)
      continue;

    Quad* neighbors[4] = { NULL };
    neighbors[0] = task.map->findQuad(task.x  , task.z-1);
    neighbors[1] = task.map->findQuad(task.x+1, task.z  );
    neighbors[2] = task.map->findQuad(task.x  , task.z+1);
    neighbors[3] = task.map->findQuad(task.x-1, task.z  );

    task.quad->generate(neighbors, task.scale, task.fractal);
    task.quad->calculateNormals(neighbors, task.scale);

    for (int i = 0; i < 4; ++i)
    {
      if (neighbors[i] != NULL)
      {
        Quad* nNeighbors[4] = { NULL };
        nNeighbors[0] = task.map->findQuad(neighbors[i]->x()  , neighbors[i]->z()-1);
        nNeighbors[1] = task.map->findQuad(neighbors[i]->x()+1, neighbors[i]->z()  );
        nNeighbors[2] = task.map->findQuad(neighbors[i]->x()  , neighbors[i]->z()+1);
        nNeighbors[3] = task.map->findQuad(neighbors[i]->x()-1, neighbors[i]->z()  );
        neighbors[i]->calculateNormals(nNeighbors, task.scale);
      }
    }

    instance->addFinishedTask(task);
  }

  return exitCode;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Map::Map(Fractal *pFractal, const std::string &pName)
  : Object(pName.empty() ? genericName("Map") : pName)
{
  _fractal = pFractal;

  FractalOptions o = _fractal->options();
  o.size = DETAIL_HIGH_POW;
  _fractal->setOptions(o);

  _worker = new Worker();

  _workerThread = NULL;

  _scale = Vector3D(10.0f, 10.0f, 10.0f);

  _initializing = false;
  _initIndex = 0;

  _mapMutex = SDL_CreateMutex();
}

Map::~Map()
{
  if (_workerThread != NULL)
  {
    _worker->kill();

    int status = 0;
    SDL_WaitThread(_workerThread, &status);

    stringstream p;
    p << "Wątek roboczy zakończony z kodem: " << status;
    print(p.str());

    _workerThread = NULL;
  }

  delete _worker;
  _worker = NULL;

  _fractal = NULL;

  clear();

  SDL_mutexV(_mapMutex);
  SDL_DestroyMutex(_mapMutex);
  _mapMutex = NULL;
}

float Map::tileValue(int quadPosX, int quadPosZ, int tilePosX, int tilePosZ)
{
  Quad *quad = findQuad(quadPosX, quadPosZ);
  if (quad == NULL)
    return 0.0f;

  float v1 = _scale.y * quad->value(tilePosX, tilePosZ);
  float v2 = _scale.y * quad->value(tilePosX + 1, tilePosZ);
  float v3 = _scale.y * quad->value(tilePosX + 1, tilePosZ + 1);
  float v4 = _scale.y * quad->value(tilePosX, tilePosZ + 1);
  return (v1 + v2 + v3 + v4) / 4.0f;
}

void Map::initFunctions()
{
  glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) SDL_GL_GetProcAddress("glGenBuffersARB");
  glBindBufferARB = (PFNGLBINDBUFFERARBPROC) SDL_GL_GetProcAddress("glBindBufferARB");
  glBufferDataARB = (PFNGLBUFFERDATAARBPROC) SDL_GL_GetProcAddress("glBufferDataARB");
  glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) SDL_GL_GetProcAddress("glDeleteBuffersARB");
}

void Map::createWorkerThread()
{
  _workerThread = SDL_CreateThread(Worker::run, (void*)(_worker));
  print("Utworzono wątek roboczy");
}

void Map::clear()
{
  SDL_mutexP(_mapMutex);
  {
    for (QuadMapIterator it = _map.begin(); it != _map.end(); ++it)
      delete (*it).second;

    _map.clear();
  }
  SDL_mutexV(_mapMutex);

  while (!_createdQuads.empty())
    _createdQuads.pop();

  while (!_unfinishedTasks.empty())
  {
    WorkerTask task = _worker->finishedTask();
    if (!task.valid)
      continue;
    _unfinishedTasks.erase(make_pair(task.x, task.z));
  }
}

float Map::initProgress() const
{
  return _initIndex / 25.0f;
}

bool Map::init()
{
  const int TASKS[][2] =
    {
      {0, 0},

      { 1, 0}, { 1,  1}, {0,  1}, {-1,  1},
      {-1, 0}, {-1, -1}, {0, -1}, { 1, -1},

      { 2,  0}, { 2,  1}, { 2,  2}, { 1,  2},
      { 0,  2}, {-1,  2}, {-2,  2}, {-2,  1},
      {-2,  0}, {-2, -1}, {-2, -2}, {-1, -2},
      { 0, -2}, { 1, -2}, { 2, -2}, { 2, -1}
    };
  const int TASKS_SIZE = sizeof(TASKS) / sizeof(*TASKS);

  if (!_initializing)
  {
    print("Inicjowanie mapy");

    clear();

    _initializing = true;
    _initIndex = 0;

    scheduleTask(TASKS[0][0], TASKS[0][1]);
    return false;
  }

  Quad *q = findQuad(TASKS[_initIndex][0], TASKS[_initIndex][1]);
  if (q == NULL)
    return false;

  ++_initIndex;

  if (_initIndex >= TASKS_SIZE)
  {
    print("Inicjowanie mapy zakończone");
    _initializing = false;

    return true;
  }

  scheduleTask(TASKS[_initIndex][0], TASKS[_initIndex][1]);

  return false;
}

Map::Quad* Map::findQuad(int x, int z)
{
  Quad *quad = NULL;
  SDL_mutexP(_mapMutex);
  {
    QuadMapIterator it = _map.find(make_pair(x, z));
    if (it != _map.end())
      quad = (*it).second;
  }
  SDL_mutexV(_mapMutex);
  return quad;
}

void Map::renderQuad(int x, int z, DetailLevel detailLevel)
{
  Quad * quad = findQuad(x, z);
  if (quad != NULL)
  {
    quad->render(detailLevel);
  }
  else
  {
    scheduleTask(x, z);
  }
}

void Map::scheduleTask(int x, int z)
{
  if (_unfinishedTasks.find(make_pair(x, z)) != _unfinishedTasks.end())
    return;

  pair<int, int> neighbors[] =
    {
      make_pair(x, z - 1),
      make_pair(x + 1, z),
      make_pair(x, z + 1),
      make_pair(x - 1, z)
    };

  for (int i = 0; i < 4; ++i)
  {
    if (_unfinishedTasks.find(neighbors[i]) != _unfinishedTasks.end())
      return;
  }

  WorkerTask task;
  task.valid = true;
  task.x = x;
  task.z = z;
  task.quad = new Quad(x, z);
  task.fractal = _fractal;
  task.scale = _scale;
  task.map = this;

  {
    stringstream p;
    p << "Tworzenie pola: (" << x << ", " << z << ")";
    print(p.str());
  }

  _worker->scheduleTask(task);
  _unfinishedTasks.insert(make_pair(x, z));
}

void Map::update()
{
  for (;;)
  {
    WorkerTask task = _worker->finishedTask();
    if (!task.valid)
      break;

    {
      stringstream p;
      p << "Utworzono pole: (" << task.x << ", " << task.z << ")";
      print(p.str());
    }

    SDL_mutexP(_mapMutex);
    {
      _map[make_pair(task.x, task.z)] = task.quad;
    }
    SDL_mutexV(_mapMutex);

    task.quad->createVBO();

    Quad* neighbors[4] = { NULL };
    neighbors[0] = findQuad(task.x  , task.z-1);
    neighbors[1] = findQuad(task.x+1, task.z  );
    neighbors[2] = findQuad(task.x  , task.z+1);
    neighbors[3] = findQuad(task.x-1, task.z  );

    for (int i = 0; i < 4; ++i)
    {
      if (neighbors[i] != NULL)
      {
        neighbors[i]->destroyVBO();
        neighbors[i]->createVBO();
      }
    }

    _unfinishedTasks.erase(make_pair(task.x, task.z));

    _createdQuads.push(make_pair(task.x, task.z));
  }

  while (_createdQuads.size() > 100)
  {
    pair<int, int> q = _createdQuads.front();

    {
      stringstream p;
      p << "Usuwanie pola: (" << q.first << ", " << q.second << ")";
      print(p.str());
    }

    SDL_mutexP(_mapMutex);
    {
      QuadMapIterator it = _map.find(q);
      (*it).second->destroyVBO();
      delete (*it).second;
      _map.erase(it);
    }
    SDL_mutexV(_mapMutex);

    _createdQuads.pop();
  }
}
