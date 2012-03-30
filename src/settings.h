/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* settings.h
    Contains the Settings class, which is responsible for storing and
    loading program settings. */

#pragma once

#include "config.h"

#include "object.h"

#include "common.h"

#include <string>
#include <map>
#include <cassert>


class Settings : public Object
{
  public:
    Settings();
    virtual ~Settings();

    inline static Settings* instance()
      { return _instance; }

    inline bool settingsChanged() const
      { return _settingsChanged; }

    inline void clearSettingsChanged()
      { _settingsChanged = false; }

    template<class T>
    void registerSetting(const std::string &key,
                         const T &defaultValue)
    {
      bool ok = false;
      std::string vStr = toString<T>(defaultValue, &ok);
      assert(ok);
      registerSetting<std::string>(key, vStr);
    }

    template<class T>
    void setSetting(const std::string &key, const T &value)
    {
      bool ok = false;
      std::string vStr = toString<T>(value, &ok);
      assert(ok);
      setSetting<std::string>(key, vStr);
    }

    template<class T>
    T setting(const std::string &key) const
    {
      std::string vStr = setting<std::string>(key);
      bool ok = false;
      T v = fromString<T>(vStr, &ok);
      assert(ok);
      return v;
    }

    void load();
    void save();

  private:
    struct Setting
    {
      std::string value, defaultValue;
    };

  private:
    static Settings* _instance;
    bool _settingsChanged;
    std::map<std::string, Setting> _settingsMap;
};

template<>
void Settings::registerSetting<std::string>(const std::string &key,
                                            const std::string &defaultValue);

template<>
void Settings::setSetting<std::string>(const std::string &key,
                                       const std::string &value);

template<>
std::string Settings::setting<std::string>(const std::string &key) const;
