/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

/* filemanager.h
   Zawiera klasę FileManager, która pozwala na rejestrowanie i pobieranie
   ścieżek plików danych. */

#pragma once

#include "config.h"

#include "object.h"

#include <string>
#include <map>

class FileManager : public Object
{
  public:
    FileManager();
    virtual ~FileManager();

    inline static FileManager* instance()
      { return _instance; }

    void registerFile(const std::string &id, const std::string &path);
    std::string fileName(const std::string &id) const;

    bool canRead(const std::string &id) const;
    bool canWrite(const std::string &id) const;

    bool ensureCanRead(const std::string &id) const;
    bool ensureCanWrite(const std::string &id) const;

  private:
    static FileManager *_instance;
    std::map<std::string, std::string> _fileMap;
};

