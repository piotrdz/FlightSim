/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* settings.cpp
    Zawiera implementację klasy Settings. */

#include "settings.h"
#include "application.h"
#include "filemanager.h"

#include <fstream>
#include <iostream>

using namespace std;


Settings* Settings::_instance = NULL;

Settings::Settings() : Object("Settings")
{
  assert(_instance == NULL);
  _instance = this;

  _settingsChanged = false;

  FileManager::instance()->registerFile("SettingsFile", "data/settings.rc");
  if (!FileManager::instance()->canWrite("SettingsFile"))
  {
    print(string("Nie można otworzyć do zapisu pliku ustawień: '") +
          FileManager::instance()->fileName("SettingsFile") + "'");
    Application::instance()->quit(1);
  }
}

Settings::~Settings()
{
  _instance = NULL;
}

template<>
void Settings::registerSetting(const string &key,
                               const string &defaultValue)
{
  map<string, Setting>::iterator it = _settingsMap.find(key);
  if (it != _settingsMap.end())
  {
    print(string("Nie można zarejestrować dwa razy tego samego klucza: '") + key + "'!");
    Application::instance()->quit(1);
    return;
  }

  Setting setting;
  setting.value = setting.defaultValue = defaultValue;
  _settingsMap[key] = setting;
}

template<>
void Settings::setSetting(const string &key, const string &value)
{
  map<string, Setting>::iterator it = _settingsMap.find(key);
  if (it == _settingsMap.end())
  {
    print("Brak klucza: '" + key + "'!");
    Application::instance()->quit(1);
    return;
  }

  (*it).second.value = value;
  _settingsChanged = true;
}

template<>
string Settings::setting(const string &key) const
{
  map<string, Setting>::const_iterator it = _settingsMap.find(key);
  if (it == _settingsMap.end())
  {
    print("Brak klucza: '" + key + "'!");
    Application::instance()->quit(1);
    return "";
  }

  return (*it).second.value;
}

void Settings::load()
{
  for (map<string, Setting>::iterator it = _settingsMap.begin();
       it != _settingsMap.end(); ++it)
  {
    (*it).second.value = (*it).second.defaultValue;
  }

  string fileName = FileManager::instance()->fileName("SettingsFile");
  ifstream file(fileName.c_str());

  if (!file.good())
  {
    print(string("Nie można otworzyć pliku ustawień: '") + fileName + "'");
    print("Wczytywanie jedynie domyślnych wartości");
    return;
  }

  int lineNo = 0;

  while (!file.eof())
  {
    ++lineNo;
    string line;
    getline(file, line);

    if (line.empty() || (line[0] == '#'))
      continue;

    size_t pos = line.find('=');
    if (pos == string::npos)
    {
      print(string("Zły format linii ustawień (linia ") + toString<int>(lineNo) + ")");
      continue;
    }

    string key = line.substr(0, pos);
    string value = line.substr(pos + 1);
    map<string, Setting>::iterator it = _settingsMap.find(key);
    if (it == _settingsMap.end())
    {
      print(string("Nieznany klucz '" + key + "' (linia ") + toString<int>(lineNo) + ")");
      continue;
    }

    (*it).second.value = value;
  }
}

void Settings::save()
{
  string fileName = FileManager::instance()->fileName("SettingsFile");
  ofstream file(fileName.c_str());
  if (!file.good())
  {
    print(string("Nie można zapisać pliku ustawień: '") + fileName + "'!");
    Application::instance()->quit(1);
    return;
  }

  file << "# Ustawienia " << Application::instance()->applicationName() <<
          " " << Application::instance()->applicationVersion() << endl;

  for (map<string, Setting>::iterator it = _settingsMap.begin();
       it != _settingsMap.end(); ++it)
  {
    file << (*it).first << "=" << (*it).second.value << endl;
  }
}
