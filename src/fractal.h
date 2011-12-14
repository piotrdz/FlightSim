/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* fractal.h
    Zawiera klasę Fractal, służącą do generowania fraktali plazmowych */

#pragma once

#include "config.h"

//! Rodzaj rozkładu losowego
enum DistributionType
{
  //! Jednostajny
  DT_Uniform,
  //! Normalny
  DT_Normal,
  //! Weibulla
  DT_Weibull
};

//! Funkcja mieszająca
enum MixingMode
{
  //! Gaussa
  MM_Gauss,
  //! Liniowa
  MM_Linear
};

//! Tryb obcinania do przedziału
enum ClampingMode
{
  //! Zaokrąglanie
  CM_Round,
  //! Ponowne losowanie
  CM_Loop
};

//! Struktura opisująca opcje fraktala
struct FractalOptions
{
  //! Rozmiar n (siatka o boku 1+2^n)
  int size;

  //! {@ Parametry rozkładu
  DistributionType distribution;
  float distributionUniformMin, distributionUniformMax;
  float distributionNormalMean, distributionNormalVariance;
  float distributionWeibullScale, distributionWeibullShape;
  //! @}

  //! {@ Parametry obcinania
  ClampingMode clamping;
  float clampingMin, clampingMax;
  //! @}

  //! @{ Parametry funkcji mieszających
  MixingMode mixing;
  float mixingTwopointGaussCutoff, mixingFourpointGaussCutoff;
  float mixingTwopointLinearMin, mixingFourpointLinearMin;
  //! @}

  FractalOptions();
};

//! Fraktal plazmowy
class Fractal
{
    //! @{ Zablokowane
    Fractal(const Fractal &f) { }
    const Fractal& operator=(const Fractal &f) { return f; }
    //!
  public:
    Fractal();
    virtual ~Fractal();

    //! @{ Akcesory
    void setOptions(const FractalOptions &options);

    inline FractalOptions options() const
      { return _options; }

    inline int size() const
      { return _valuesSize; }
    //! @}

    //! Ustawia wszystkie wartości na niezainicjalizowane (-2)
    void clear();

    //! Do ustawiania początkowych wartości
    void setValue(int x, int y, float value);
    //! Zwraca wartość lub 0.0f poza zakresem
    float value(int x, int y) const;

    //! Generuje fraktal z danym ziarnem
    void generate(int seed);

    //! Zwraca losową wartość z ustawionego w opcjach rozkładu (po obcięciu i normalizacji do [0, 1])
    float randomValue();

  private:
    unsigned int _rand;
    FractalOptions _options;
    float **_values;
    int _valuesSize;

    //! Funkcja rekurencyjna do generowania fraktala
    void generateRecursive(int x1, int y1, int x2, int y2);
    float gauss(float x);

    void newValues();
    void deleteValues();
};
