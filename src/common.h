/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* common.h
    Zawiera wspólne struktury i definicje dla całego programu. */

#pragma once

#include "config.h"

#include <string>
#include <sstream>

#include <SDL/SDL.h>

//! Oznaczenie położenia.
enum Alignment
{
  AL_Left = 0x1,
  AL_Right = 0x2,
  AL_HCenter = AL_Left | AL_Right,
  AL_Top = 0x4,
  AL_Bottom = 0x8,
  AL_VCenter = AL_Top | AL_Bottom,
  AL_Center = AL_HCenter | AL_VCenter
};

//! Kolor RGBA
struct Color
{
  float r, g, b, a;

  explicit Color(float pR = 0.0f, float pG = 0.0f, float pB = 0.0f, float pA = 1.0f)
    : r(pR), g(pG), b(pB), a(pA) {}

  void fromChar(char pR, char pG, char pB, char pA);
  void fromFloat(float pR, float pG, float pB, float pA);
  void fromSDL_Color(const SDL_Color &color);

  void toChar(char &pR, char &pG, char &pB, char &pA) const;
  void toFloat(float &pR, float &pG, float &pB, float &pA) const;
  SDL_Color toSDL_Color() const;

  inline operator float*()
    { return (float*)(this); }

  inline operator const float*() const
    { return (const float*)(this); }
};

//! Punkt w przestrzeni 2D (x, y).
struct Point
{
  float x, y;

  explicit Point(float pX = 0.0f, float pY = 0.0f)
    : x(pX), y(pY) {}

  inline bool operator==(const Point &p) const
    { return x == p.x && y == p.y; }

  inline bool operator!=(const Point &p) const
    { return !operator==(p); }
};

//! Rozmiar 2D (wysokość i szerokość).
struct Size
{
  float w, h;

  explicit Size(float pWidth = 0.0f, float pHeight = 0.0f)
    : w(pWidth), h(pHeight) {}

  float diagonal() const;

  inline bool operator==(const Size &s) const
    { return w == s.w && h == s.h; }

  inline bool operator!=(const Size &s) const
    { return !operator==(s); }
};

//! Prostokąt (lewy górny wierzchołek + rozmiar)
struct Rect : public Point, public Size
{
  explicit Rect(float pX = 0.0f, float pY = 0.0f, float pWidth = 0.0f, float pHeight = 0.0f)
    : Point(pX, pY), Size(pWidth, pHeight) {}
  Rect(const Point &pPosition, const Size &pSize)
    : Point(pPosition), Size(pSize) {}

  inline float x1() const
    { return x; }
  inline float y1() const
    { return y; }

  inline float xMid() const
    { return x + 0.5f * w; }
  inline float yMid() const
    { return y + 0.5f * h; }

  inline float x2() const
    { return x + w; }
  inline float y2() const
    { return y + h; }

  inline Point position() const
    { return *this; }
  inline Size size() const
    { return *this; }

  bool pointInside(const Point &point) const;

  inline bool operator==(const Rect &r) const
    { return Point::operator==(r) && Size::operator==(r); }

  inline bool operator!=(const Rect &r) const
    { return !operator==(r); }
};

//! @{ Stałe używane w obliczeniach
extern const float TOLERANCE;
extern const float PI_180;
extern const float _180_PI;
//! @}

//! Wektor w przestrzeni 3D
struct Vector3D
{
  float x, y, z;

  explicit Vector3D(float pX = 0.0f, float pY = 0.0f, float pZ = 0.0f)
    : x(pX), y(pY), z(pZ) {}

  // Operatory dla wektorów i skalarów:

  Vector3D operator-() const;

  const Vector3D& operator+=(const Vector3D &vector);
  const Vector3D& operator-=(const Vector3D &vector);
  const Vector3D& operator*=(float value);
  const Vector3D& operator/=(float value);

  friend Vector3D operator+(const Vector3D &left, const Vector3D &right);
  friend Vector3D operator-(const Vector3D &left, const Vector3D &right);
  friend Vector3D operator*(float left, const Vector3D &right);
  friend Vector3D operator*(const Vector3D &left, float right);
  friend Vector3D operator/(const Vector3D &left, float right);

  // Do zamiany na wskaźniki:

  inline operator float*()
    { return (float*)(this); }

  inline operator const float*() const
    { return (const float*)(this); }

  //! Zwraca długość
  float length() const;

  //! Zwraca cos kąta między wektorem a danym innym wektorem
  float cosAngle(const Vector3D &right) const;

  //! Zwraca kąt między wektorem a danym innym wektorem
  float angle(const Vector3D &right) const;


  //! Zwraca iloczyn skalarny z danym wektorem
  float dotProduct(const Vector3D &right) const;

  //! Zwraca iloczyn skalarny 2 wektorów
  inline static float dotProduct(const Vector3D &left, const Vector3D &right)
    { return left.dotProduct(right); }

  //! Zwraca iloczyn wektorowy z danym wektorem
  Vector3D crossProduct(const Vector3D &right) const;

  //! Zwraca iloczyn wektorowy 2 wektorów
  inline static Vector3D crossProduct(const Vector3D &left, const Vector3D &right)
    { return left.crossProduct(right); }


  //! Normalizuje wektor
  void normalize();

  //! Zwraca znormalizowany dany wektor
  inline static Vector3D normalize(const Vector3D &vec)
  {
    Vector3D result = vec;
    result.normalize();
    return result;
  }

  //! Obraca wektor, mnożący przez macierz obrotu wyznaczoną przez kąt (w stopniach) i oś
  void rotate(float angle, const Vector3D &axis);

  //! Zwraca obrócony wektor
  inline static Vector3D rotate(const Vector3D &vec, float angle, const Vector3D &axis)
  {
    Vector3D result = vec;
    result.rotate(angle, axis);
    return result;
  }

  //! Obcina wartości współrzędnych do podanego przedziału
  void clamp(const Vector3D &minValues, const Vector3D &maxValues);

  //! Zwraca wektor z obciętymi współrzędnymi
  inline static Vector3D clamp(const Vector3D &vec, const Vector3D &minValues,
                               const Vector3D &maxValues)
  {
    Vector3D result = vec;
    result.clamp(minValues, maxValues);
    return result;
  }

  //! Sprawdza, czy współrzędne wektora znajdują się w danym przedziale
  bool between(const Vector3D &minValues, const Vector3D &maxValues);
};

// Prywatne dane klasy Time
struct TimePrivate;


//! Klasa przechowująca informację o momencie czasu.
/**
 * Pozwala na pobranie obecnego momentu i obliczenie różnicy między dwoma momentami.
 *
 * Rozdzielczość jest zależna od systemu, ale zwracane wartości są zawsze w nanosekundach.
 *
 * Nie pozwala na bezpośredni dostęp do danych, ponieważ są one liczone według
 * różnych zegarów w zależności od systemu.
 */
class Time
{
  private:
    Time() : _private(NULL) {}

  public:
    ~Time();

    Time(const Time &t);
    const Time& operator=(const Time &t);

    //! Zwraca dostępną rozdzielczość pomiaru czasu w nanosekundach (ns)
    static int resolution();

    //! Zwraca obecny moment czasowy
    static Time* currentTime();

    //! Zwraca różnicę między momentami czasu w nanosekundach (ns)
    long long difference(Time *other);

  private:
    TimePrivate *_private;
};

//! Klasa timera do mierzenia upływu czasu.
/**
 * Klasa ta pozwala na sprawdzenie, kiedy upłynie dany odcinek czasu.
 */
class Timer
{
  public:
    //! Tworzy nowy timer z danym interwałem w nanosekundach (ns)
    explicit Timer(unsigned long long pInterval = 10000000);
    ~Timer();

    Timer(const Timer &t);
    const Timer& operator=(const Timer &t);

    //! Resetuje timer, czyli ustawia pomiar od obecnej chwili
    void reset();

    //! Sprawdza, czy upłynął już dany odcinek czasu
    bool checkTimeout();

    //! Zwraca ustawiony interwał w nanosekundach (ns)
    inline unsigned long long interval() const
      { return _interval; }

    //! Ustawia nowy interwał
    inline void setInterval(unsigned long long pInterval)
      { _interval = pInterval; }

    //! Ustawia nowy interwał w milisekundach
    inline void setIntervalMsec(unsigned long long pInterval)
      { _interval = pInterval * 1000000; }

    //! Zwraca właściwą miarę zmierzonego odcinka (zazwyczaj >= interwał)
    unsigned long long timeoutDifference();

    //! timeoutDifference() w milisekundach
    inline unsigned int timeoutDifferenceMsec()
      { return timeoutDifference() * 1000000; }

    //! Zwraca liczbę odmierzonych odcinków od początku
    inline unsigned int timeoutCount() const
      { return _timeoutCount; }

    //! Zwraca, czy timer włączony
    inline bool enabled() const
      { return _enabled; }

    //! Ustawia, czy timer włączony
    inline void setEnabled(bool pEnabled)
      { _enabled = pEnabled; }

  private:
    unsigned long long _interval;
    Time *_intervalBeginTime, *_intervalEndTime;
    unsigned int _timeoutCount;
    bool _enabled;
};

//! Zamienia wartość na string
template<class T>
std::string toString(T value, bool *ok = NULL)
{
  std::ostringstream s;
  s << value;
  if (ok != NULL)
    *ok = !s.fail();
  return s.str();
}

//! Zamienia string na wartość
template<class T>
T fromString(const std::string &str, bool *ok = NULL)
{
  std::istringstream s;
  s.str(str);
  T value;
  s >> value;
  if (ok != NULL)
    *ok = !s.fail();
  return value;
}

//! Zamienia znak Unicode na UTF-8
std::string unicodeCharToUtf8(Uint16 ch);

//! Zamienia string Unicode na UTF-8
std::string unicodeStringToUtf8(const std::wstring &str);

//! Zamienia znak UTF-8 na Unicode
Uint16 utf8CharToUnicode(const std::string &ch);

//! Zamienia tekst UTF-8 na Unicode
std::wstring utf8StringToUnicode(const std::string &str);

//! Zwraca długość następnego znaku UTF-8 na pozycji pos
int nextUtf8CharLength(const std::string &str, unsigned int pos);

//! Zwraca długość tekstu zakodowanego w UTF-8
size_t utf8StringLength(const std::string &str);
