/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

/* filemanager.cpp
   Zawiera implementację klasy FileManager. */

#include "filemanager.h"

#include "application.h"

#include <fstream>
#include <sstream>
#include <cassert>

using namespace std;


FileManager* FileManager::_instance = NULL;

FileManager::FileManager() : Object("FileManager")
{
  assert(_instance == NULL);
  _instance = this;
}

FileManager::~FileManager()
{
  _instance = NULL;
}

void FileManager::registerFile(const string& id, const string& path)
{
  map<string, string>::iterator it = _fileMap.find(id);
  if (it != _fileMap.end())
  {
    stringstream s;
    s << "Plik " << path << " nie może być zarejestrowany podwójnie (id1="
      << (*it).first << ", id2=" << (*it).second << ")";
    print(s.str());
    Application::instance()->quit(1);
    return;
  }

  _fileMap.insert(make_pair(id, path));
}

std::string FileManager::fileName(const std::string& id) const
{
  map<string, string>::const_iterator it = _fileMap.find(id);
  if (it == _fileMap.end())
  {
    stringstream s;
    s << "Plik o id=" << id << " nie jest zarejestrowany!";
    print(s.str());
    Application::instance()->quit(1);
    return "";
  }

  return (*it).second;
}

bool FileManager::canRead(const string& id) const
{
  string path = fileName(id);
  if (path.empty())
    return false;

  ifstream f(path.c_str());
  return f.good();
}

bool FileManager::canWrite(const string &id) const
{
  string path = fileName(id);
  if (path.empty())
    return false;

  ofstream f(path.c_str(), ios_base::out | ios_base::app | ios_base::ate);
  return f.good();
}

bool FileManager::ensureCanRead(const std::string &id) const
{
  bool result = canRead(id);

  if (!result)
  {
    print(string("Nie można odczytać pliku: ") + id + " = '" + fileName(id) + "'");
    Application::instance()->quit(1);
  }

  return result;
}

bool FileManager::ensureCanWrite(const std::string &id) const
{
  bool result = canWrite(id);

  if (!result)
  {
    print(string("Nie można zapisywać do pliku: ") + id + " = '" + fileName(id) + "'");
    Application::instance()->quit(1);
  }

  return result;
}
