/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* map.h
    Contains the Map class (and its inner classes), responsible for
    generating and rendering of the world map. */

#pragma once

#include "config.h"

#include "object.h"
#include "common.h"

#include <map>
#include <queue>
#include <set>

#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>

class Fractal;

class Map : public Object
{
  public:
    enum DetailLevel
    {
      // 128x128
      DetailHigh,
      // 64x64
      DetailMedium,
      // 32x32
      DetailLow
    };

  public:
    Map(Fractal *pFractal, const std::string &pName = "");
    virtual ~Map();

    inline Vector3D scale() const
      { return _scale; }

    inline void setScale(const Vector3D &pScale)
      { _scale = pScale; }

    inline Vector3D quadSize() const
    {
      return Vector3D(_scale.x * DETAIL_HIGH_COUNT,
                      2.0f * _scale.y,
                      _scale.z * DETAIL_HIGH_COUNT);
    }

    inline Vector3D quadTileSize() const
    {
      return _scale;
    }

    float tileValue(int quadPosX, int quadPosZ, int tilePosX, int tilePosZ);

    static void initFunctions();

    void createWorkerThread();

    void clear();

    bool init();

    float initProgress() const;

    void renderQuad(int x, int z, DetailLevel detailLevel);

    void update();

  private:
    static const int DETAIL_HIGH_POW = 7;
    static const int DETAIL_HIGH_COUNT = 128;
    static const int DETAIL_MEDIUM_COUNT = 64;
    static const int DETAIL_LOW_COUNT = 32;

    class Quad
    {
      public:
        Quad(int x, int z) : _x(x), _z(z) {}
        ~Quad() {}

        inline int x() const
          { return _x; }
        inline int z() const
          { return _z; }

        inline int seed() const
          { return (_x * 0x1f1f1f1f) ^ _z; }

        void generate(Quad* neighbors[4], const Vector3D &scale,
                      Fractal *fractal);
        
        void calculateNormals(Quad* neighbors[4], const Vector3D &scale);

        void createVBO();

        void render(DetailLevel detailLevel) const;

        void destroyVBO();

        float value(int x, int z) const;

      private:
        const int _x, _z;

        float _values[1+DETAIL_HIGH_COUNT][1+DETAIL_HIGH_COUNT];

        Vector3D _detailHighVertices[6 * DETAIL_HIGH_COUNT * DETAIL_HIGH_COUNT];
        Vector3D _detailHighNormals[6 * DETAIL_HIGH_COUNT * DETAIL_HIGH_COUNT];

        Vector3D _detailMediumVertices[6 * DETAIL_MEDIUM_COUNT * DETAIL_MEDIUM_COUNT];
        Vector3D _detailMediumNormals[6 * DETAIL_MEDIUM_COUNT * DETAIL_MEDIUM_COUNT];

        Vector3D _detailLowVertices[6 * DETAIL_LOW_COUNT * DETAIL_LOW_COUNT];
        Vector3D _detailLowNormals[6 * DETAIL_LOW_COUNT * DETAIL_LOW_COUNT];

        unsigned int _detailHighVerticesVBO, _detailHighNormalsVBO;
        unsigned int _detailMediumVerticesVBO, _detailMediumNormalsVBO;
        unsigned int _detailLowVerticesVBO, _detailLowNormalsVBO;

        void filterValues(float v0, float &v1, float &v2, float &v3, float v4);
    };

    struct WorkerTask
    {
      bool valid;
      int x, z;
      Fractal *fractal;
      Vector3D scale;
      Quad *quad;
      Map *map;

      WorkerTask()
      {
        valid = false;
        x = z = 0;
        fractal = NULL;
        quad = NULL;
        map = NULL;
      }
    };

    class Worker
    {
      public:
        Worker();
        ~Worker();

        void kill(int exitCode = 0);
        void scheduleTask(const WorkerTask &task);
        WorkerTask finishedTask();

        static int run(void *data);

      private:
        std::queue<WorkerTask> _scheduledTasks, _finishedTasks;
        SDL_mutex *_exitCodeMutex, *_scheduledTaskMutex, *_finishedTaskMutex;
        SDL_cond *_scheduledTaskCond;
        int _exitCode;

        int getExitCode();
        WorkerTask getScheduledTask();
        void addFinishedTask(const WorkerTask &task);
    };

    class PairComparator
    {
      public:
        bool operator()(const std::pair<int, int> &p1,
                        const std::pair<int, int> &p2)
        {
          return (p1.first < p2.first) ||
                  ((p1.first == p2.first) && (p1.second < p2.second));
        }
    };

  private:
    typedef std::map< std::pair<int, int>,
                      Quad*, PairComparator > QuadMap;
    typedef std::map< std::pair<int, int>,
                      Quad*, PairComparator >::iterator QuadMapIterator;

    Fractal *_fractal;
    QuadMap _map;
    Vector3D _scale;
    SDL_Thread *_workerThread;
    Worker *_worker;
    SDL_mutex *_mapMutex;
    std::set< std::pair<int, int>, PairComparator > _unfinishedTasks;
    std::queue< std::pair<int, int> > _createdQuads;
    bool _initializing;
    int _initIndex;

    Quad* findQuad(int x, int z);
    void scheduleTask(int x, int z);
};
