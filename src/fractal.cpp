/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* fractal.cpp
    Contains the implementation of the Fractal class. */

#include "fractal.h"

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

// Ugly hack
#if defined(WIN32) || defined(_WIN32)
int rand_r(unsigned int *seedp)
{
  srand(*seedp);
  int result = rand();
  *seedp = 13 + result << 11 | result >> (32 - 11);
  return result;
}
#endif

FractalOptions::FractalOptions()
{
  size = 7;

  distribution = DT_Normal;
  distributionUniformMin = 0.0f;
  distributionUniformMax = 1.0f;
  distributionNormalMean = 0.8f;
  distributionNormalVariance = 1.0f;
  distributionWeibullScale = 1.0f;
  distributionWeibullShape = 5.0f;

  clamping = CM_Loop;
  clampingMin = -2.5f;
  clampingMax = 2.0f;

  mixing = MM_Linear;
  mixingTwopointGaussCutoff = 2.5f;
  mixingFourpointGaussCutoff = 2.1f;
  mixingTwopointLinearMin = 0.05f;
  mixingFourpointLinearMin = 0.01f;
}

Fractal::Fractal()
{
  _rand = 0;
  _valuesSize = 1 + (1 << _options.size);

  newValues();
  clear();
}

Fractal::~Fractal()
{
  if (_values != NULL)
    deleteValues();
}

void Fractal::newValues()
{
  _values = new float*[_valuesSize];
  for (int i = 0; i < _valuesSize; ++i)
    _values[i] = new float[_valuesSize];
}

void Fractal::deleteValues()
{
  for (int i = 0; i < _valuesSize; ++i)
  {
    delete[] _values[i];
    _values[i] = NULL;
  }
  delete[] _values;
  _values = NULL;
}

void Fractal::setOptions(const FractalOptions& options)
{
  if ((options.size < 3) || (options.size > 11))
    return;

  if (options.distributionUniformMax <= options.distributionUniformMin)
    return;

  if (options.distributionNormalVariance <= 0)
    return;

  if ((options.distributionWeibullScale <= 0) || (options.distributionWeibullShape <= 0))
    return;

  if (options.clampingMax <= options.clampingMin)
    return;

  if ((options.mixingTwopointGaussCutoff <= 0) || (options.mixingFourpointGaussCutoff <= 0))
    return;

  if ((options.mixingTwopointLinearMin <= 0) || (options.mixingTwopointLinearMin >= 0.5))
    return;

  if ((options.mixingFourpointLinearMin <= 0) || (options.mixingFourpointLinearMin >= 0.25))
    return;

  if (_options.size != options.size)
  {
    deleteValues();
    _valuesSize = 1 + (1 << options.size);
    newValues();
  }

  _options = options;
}

float Fractal::value(int x, int y) const
{
  if ((x < 0) || (x >= _valuesSize) || (y < 0) || (y >= _valuesSize))
    return 0.0f;

  return _values[x][y];
}

void Fractal::setValue(int x, int y, float value)
{
  if ((x < 0) || (x >= _valuesSize) || (y < 0) || (y >= _valuesSize))
    return;

  _values[x][y] = value;
}

void Fractal::clear()
{
  for (int x = 0; x < _valuesSize; ++x)
  {
    for (int z = 0; z < _valuesSize; ++z)
    {
      _values[x][z] = -2.0f;
    }
  }
}

float Fractal::gauss(float x)
{
  return exp(-x * x);
}

float Fractal::randomValue()
{
  float value = 0.0f;
  int count = 0;
  while (count < 1000)
  {
    ++count;

    if (_options.distribution == DT_Uniform)
    {
      float range = _options.distributionUniformMax - _options.distributionUniformMin;
      float x = ((float)rand_r(&_rand)) / ((float)RAND_MAX);
      value = _options.distributionUniformMin + x * range;
    }
    else if (_options.distribution == DT_Normal)
    {
      float x = ((float)rand_r(&_rand)) / (RAND_MAX + 1.0f);
      float y = ((float)rand_r(&_rand)) / (RAND_MAX + 1.0f);
      float normal = sqrt(-2.0f * log(x)) * cos(2.0f * M_PI * y);
      value = _options.distributionNormalMean + _options.distributionNormalVariance * normal;
    }
    else if (_options.distribution == DT_Weibull)
    {
      float alpha = pow(_options.distributionWeibullScale, -_options.distributionWeibullShape);
      float x = ((float)rand_r(&_rand)) / (1.0f + RAND_MAX);
      value = pow(-log(1.0f - x) / alpha, 1.0f / _options.distributionWeibullShape);
    }

    if (_options.clamping == CM_Round)
    {
      if (value < _options.clampingMin)
        value = _options.clampingMin;
      if (value > _options.clampingMax)
        value = _options.clampingMax;
      break;
    }
    else if (_options.clamping == CM_Loop)
    {
      if ((value >= _options.clampingMin) && (value <= _options.clampingMax))
        break;
    }
  }

  return (value - _options.clampingMin) / (_options.clampingMax - _options.clampingMin);
}

void Fractal::generateRecursive(int x1, int y1, int x2, int y2)
{
  int midX = (x1 + x2) / 2;
  int midY = (y1 + y2) / 2;
  float midScale = ((float)(x2 - x1)) / ((float)_valuesSize);

  float mixSingle = 0.0f;
  if (_options.mixing == MM_Gauss)
    mixSingle = 0.5f * gauss(midScale * _options.mixingTwopointGaussCutoff);
  else if (_options.mixing == MM_Linear)
    mixSingle = 0.5f - midScale * (0.5f - _options.mixingTwopointLinearMin);

  float v1 = (1.0f - 2.0f * mixSingle) * randomValue() +
               mixSingle * (_values[x1][y1] + _values[x2][y1]);
  if (_values[midX][y1] == -2.0f)
    _values[midX][y1] = v1;

  float v2 = (1.0f - 2.0f * mixSingle) * randomValue() +
               mixSingle * (_values[x1][y2] + _values[x2][y2]);
  if (_values[midX][y2] == -2.0f)
    _values[midX][y2] = v2;

  float v3 = (1.0f - 2.0f * mixSingle) * randomValue() +
               mixSingle * (_values[x1][y1] + _values[x1][y2]);
  if (_values[x1][midY] == -2.0f)
    _values[x1][midY] = v3;

  float v4 = (1.0f - 2.0f * mixSingle) * randomValue() +
               mixSingle * (_values[x2][y1] + _values[x2][y2]);
  if (_values[x2][midY] == -2.0f)
    _values[x2][midY] = v4;

  float mixFourpoint = 0.0f;
  if (_options.mixing == MM_Gauss)
    mixFourpoint = 0.25f * gauss(midScale * _options.mixingFourpointGaussCutoff);
  else if (_options.mixing == MM_Linear)
    mixFourpoint = 0.25f - midScale * (0.25f - _options.mixingFourpointLinearMin);

  float vM = (1.0f - 4.0f * mixFourpoint) * randomValue() +
               mixFourpoint * (_values[x1][y1] + _values[x1][y2] +
                               _values[x2][y1] + _values[x2][y2]);

  if (_values[midX][midY] == -2.0f)
    _values[midX][midY] = vM;

  if (midX == x1 + 1)
    return;

  generateRecursive(x1, y1, midX, midY);
  generateRecursive(midX, y1, x2, midY);
  generateRecursive(x1, midY, midX, y2);
  generateRecursive(midX, midY, x2, y2);
}

void Fractal::generate(int seed)
{
  _rand = seed;

  float v1 = randomValue();
  if (_values[0][0] == -2.0f)
    _values[0][0] = v1;

  float v2 = randomValue();
  if (_values[0][_valuesSize-1] == -2.0f)
    _values[0][_valuesSize-1] = v2;

  float v3 = randomValue();
  if (_values[_valuesSize-1][_valuesSize-1] == -2.0f)
    _values[_valuesSize-1][_valuesSize-1] = v3;

  float v4 = randomValue();
  if (_values[_valuesSize-1][0] == -2.0f)
    _values[_valuesSize-1][0] = v4;

  generateRecursive(0, 0, _valuesSize - 1, _valuesSize - 1);
}
