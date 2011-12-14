/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* map.h
    Zawiera klasę Map (i jej klasy wewnętrzne) odpowiedzialną za generowanie
    i renderowanie mapy. */

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

//! Mapa generowana z fraktalu plazmowego
/** Klasa odpowiada za tworzenie, przechowywanie i rysowanie mapy złożonej
 * z kwadratowych pól, generowanych za pomocą danego fraktala plazmowego.
 *
 * Każde pole jest generowane z fraktala plazmowego w pełnym rozmiarze 128x128
 * (2^15 = 32 768 trójkątów), ale także w dwu- i czterokrotnie zmniejszonym:
 * 64x64 i 32x32. Pola są rysowane na płaszczyźnie XZ - i są identyfikowane
 * przez parę współrzędnych (x,z).
 *
 * Ponieważ tworzenie nowych pól jest czasochłonne, wykorzystywany jest do
 * tego osobny wątek. Jest to jednak przezroczyste z punktu widzenia
 * użytkownika klasy - przy żądaniu rysowania nieutworzonego pola, zostanie
 * ono zakolejkowane do utworzenia.
 *
 * Zagwarantowana jest spójność mapy, ponieważ każde nowe pole ma ustawiane
 * wartości krawędzi wspólnych z sąsiadami na wartości z sąsiada. Jeżeli
 * sąsiad nie jest obecnie dostępny, generowanie pola zostaje odłożone
 * na później.
 *
 * Pola są jednoznacznie wygenerowane na podstawie samych współrzędnych
 * (ziarno generatora losowego jest ustawiane na podstawie pary (x,z)).
 * Można więc usunąć nadmiar przechowywanych pól, ponieważ zawsze
 * można do nich "wrócić", generując je od nowa.
 */
class Map : public Object
{
  public:
    //! Jakość (poziom detali), czyli rozmiar fraktala
    enum DetailLevel
    {
      //! 128x128
      DetailHigh,
      //! 64x64
      DetailMedium,
      //! 32x32
      DetailLow
    };

  public:
    Map(Fractal *pFractal, const std::string &pName = "");
    virtual ~Map();

    //! Zwraca skalę
    inline Vector3D scale() const
      { return _scale; }

    //! Ustawia skalę
    inline void setScale(const Vector3D &pScale)
      { _scale = pScale; }

    //! Zwraca rozmiar pola po przeskalowaniu
    inline Vector3D quadSize() const
    {
      return Vector3D(_scale.x * DETAIL_HIGH_COUNT,
                      2.0f * _scale.y,
                      _scale.z * DETAIL_HIGH_COUNT);
    }

    //! Zwraca rozmiar pojedynczej płytki pola po przeskalowaniu
    inline Vector3D quadTileSize() const
    {
      return _scale;
    }

    //! Zwraca średnią wartość na danej płytce pola
    float tileValue(int quadPosX, int quadPosZ, int tilePosX, int tilePosZ);

    //! Inicjalizuje funkcje rozszerzenia VBO
    static void initFunctions();

    //! Tworzy wątek roboczy
    void createWorkerThread();

    //! Czyści mapę
    void clear();

    //! Funkcja asynchronicznej inicjalizacji
    /** Zwraca true, gdy inicjalizacja została zakończona; w przeciwnym razie false. */
    bool init();

    //! Zwraca postęp inicjalizacji (od 0 do 1)
    float initProgress() const;

    //! Rysuje dane pole
    void renderQuad(int x, int z, DetailLevel detailLevel);

    //! Funkcja asynchronicznej aktualizacji (obsługi wątku)
    void update();

  private:
    static const int DETAIL_HIGH_POW = 7;
    static const int DETAIL_HIGH_COUNT = 128;
    static const int DETAIL_MEDIUM_COUNT = 64;
    static const int DETAIL_LOW_COUNT = 32;

    //! Klasa pola mapy
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

        //! Generuje losowo pole z użyciem fraktala i skali
        void generate(Quad* neighbors[4], const Vector3D &scale,
                      Fractal *fractal);
        //! Wylicza wektory normalne do powierzchni
        void calculateNormals(Quad* neighbors[4], const Vector3D &scale);

        //! Tworzy bufory VBO
        void createVBO();

        //! Rysuje pole
        void render(DetailLevel detailLevel) const;

        //! Zwalnia bufory VBO
        void destroyVBO();

        //! Zwraca wartość na danej pozycji
        float value(int x, int z) const;

      private:
        //! Współrzędne
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

    //! Struktura opisująca zadanie wątku roboczego
    struct WorkerTask
    {
      //! Czy poprawne?
      bool valid;
      //! Współrzędne
      int x, z;
      //! Fraktal
      Fractal *fractal;
      //! Skala
      Vector3D scale;
      //! Pole (utworzone lub do utworzenia)
      Quad *quad;
      //! Sąsiedzi pola
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

    //! Klasa wątku roboczego
    /** Obsługuje zadania tworzenia pól. */
    class Worker
    {
      public:
        Worker();
        ~Worker();

        //! Kończy wątek z kodem wyjścia
        void kill(int exitCode = 0);
        //! Dodaje zadanie do kolejki zadań
        void scheduleTask(const WorkerTask &task);
        //! Pobiera zadanie z kolejki skończonych zadań
        WorkerTask finishedTask();

        //! Główna funkcja wątku
        static int run(void *data);

      private:
        //! Kolejki: zadań do wykonania, skończonych zadań
        std::queue<WorkerTask> _scheduledTasks, _finishedTasks;
        //! Mutexy na dostęp do zmiennych
        SDL_mutex *_exitCodeMutex, *_scheduledTaskMutex, *_finishedTaskMutex;
        //! Warunkowe oczekiwanie na nadchodzące zadanie
        SDL_cond *_scheduledTaskCond;
        //! Kod wyjścia
        int _exitCode;

        //! @{ Wewnętrzne funkcje do zmiany/pobierania zmiennych
        int getExitCode();
        WorkerTask getScheduledTask();
        void addFinishedTask(const WorkerTask &task);
        //! @}
    };

    //! Do porównywania par współrzędnych w mapach/zbiorach
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

    //! Fraktal
    Fractal *_fractal;
    //! Mapa pól
    QuadMap _map;
    //! Skala mapy
    Vector3D _scale;
    //! Wątek
    SDL_Thread *_workerThread;
    //! Obiekt, na którym pracuje wątek
    Worker *_worker;
    //! Mutex na dostęp do _map
    SDL_mutex *_mapMutex;
    //! Zadania nieukończone
    std::set< std::pair<int, int>, PairComparator > _unfinishedTasks;
    //! Kolejka utworzonych pól - wykorzystywana do usuwania nadmiaru
    std::queue< std::pair<int, int> > _createdQuads;
    //! Czy mapa jest inicjowana?
    bool _initializing;
    //! Indeks podczas inicjalizacji
    int _initIndex;

    Quad* findQuad(int x, int z);
    void scheduleTask(int x, int z);
};
