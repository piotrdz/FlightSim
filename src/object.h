/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* object.h
    Contains the classes Object - a base class for objects not displayed
    and an associated class IdManager, which stores unique IDs of each
    Objectk -- it allows for garbage collection. */

#pragma once

#include "config.h"

#include <string>
#include <map>


class Object
{
  private:
    Object() : _name(""), _id(-1) {}
    Object(const Object &) : _name(""), _id(-1) {}
    void operator=(const Object &) {}

  public:
    Object(const std::string &pName);
    virtual ~Object();

    inline int id() const
      { return _id; }
    inline std::string name() const
      { return _name; }

  private:
    const std::string _name;
    const int _id;
    static int _lastId;

  protected:
    static std::string genericName(const std::string &prefix);

    void print(const std::string &message) const;
};

class IdDatabase : public Object
{
  public:
    IdDatabase();
    virtual ~IdDatabase();

    bool add(int id, Object *object);
    bool remove(int id);

    inline static IdDatabase* instance()
      { return _instance; }

    inline int genericId()
      { return ++_nextGenericId; }

    Object* searchById(int id) const;
    int searchByPointer(Object *object) const;

  private:
    static IdDatabase *_instance;
    int _nextGenericId;
    std::map<int, Object*> _idMap;
    typedef std::map<int, Object*>::iterator IdIterator;
    typedef std::map<int, Object*>::const_iterator ConstIdIterator;
};
