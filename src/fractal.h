/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* fractal.h
    Contains the Fractal class, which generates plasma fractals. */

#pragma once

#include "config.h"

enum DistributionType
{
  DT_Uniform,
  DT_Normal,
  DT_Weibull
};

enum MixingMode
{
  MM_Gauss,
  MM_Linear
};

enum ClampingMode
{
  CM_Round,
  CM_Loop
};

struct FractalOptions
{
  int size;

  DistributionType distribution;
  float distributionUniformMin, distributionUniformMax;
  float distributionNormalMean, distributionNormalVariance;
  float distributionWeibullScale, distributionWeibullShape;

  ClampingMode clamping;
  float clampingMin, clampingMax;

  MixingMode mixing;
  float mixingTwopointGaussCutoff, mixingFourpointGaussCutoff;
  float mixingTwopointLinearMin, mixingFourpointLinearMin;

  FractalOptions();
};

class Fractal
{
    Fractal(const Fractal &f) { }
    const Fractal& operator=(const Fractal &f) { return f; }

  public:
    Fractal();
    virtual ~Fractal();

    void setOptions(const FractalOptions &options);

    inline FractalOptions options() const
      { return _options; }

    inline int size() const
      { return _valuesSize; }
  
    void clear();

    void setValue(int x, int y, float value);
    float value(int x, int y) const;

    void generate(int seed);

    float randomValue();

  private:
    unsigned int _rand;
    FractalOptions _options;
    float **_values;
    int _valuesSize;

    void generateRecursive(int x1, int y1, int x2, int y2);
    float gauss(float x);

    void newValues();
    void deleteValues();
};
