/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* object.cpp
    Contains the implemenation of classes Object and IdManager. */

#include "object.h"
#include "application.h"

#include <sstream>
#include <cassert>

using namespace std;


int Object::_lastId = 0;

Object::Object(const string &pName)
  : _name(pName), _id(++_lastId)
{
  // ID = 1 is reserved for IdManager object
  if (_id == 1)
    return;

  IdDatabase::instance()->add(_id, this);
}

Object::~Object()
{
  // ID = 1 is reserved for IdManager object
  if (_id == 1)
    return;

  IdDatabase::instance()->remove(_id);
}

string Object::genericName(const string &prefix)
{
  static int genericCount = 0;
  ostringstream stream;
  stream << prefix << ++genericCount;
  return stream.str();
}

void Object::print(const std::string &message) const
{
  Application::instance()->print(_name, message);
}


IdDatabase* IdDatabase::_instance = NULL;

IdDatabase::IdDatabase() : Object("IdDatabase")
{
  assert(_instance == NULL);
  _instance = this;
}

IdDatabase::~IdDatabase()
{
  for (IdIterator it = _idMap.begin(); it != _idMap.end(); ++it)
    delete (*it).second;
 
  _instance = NULL;
}

bool IdDatabase::add(int id, Object *object)
{
  IdIterator it = _idMap.find(id);

  if (it != _idMap.end())
    return false;

  _idMap.insert(make_pair<int, Object*>(id, object));
  return true;
}

bool IdDatabase::remove(int id)
{
  IdIterator it = _idMap.find(id);

  if (it == _idMap.end())
    return false;

  _idMap.erase(it);
  return true;
}

Object* IdDatabase::searchById(int id) const
{
  ConstIdIterator it = _idMap.find(id);

  if (it == _idMap.end())
    return false;

  return (*it).second;
}

int IdDatabase::searchByPointer(Object *object) const
{
  for (ConstIdIterator it = _idMap.begin(); it != _idMap.end(); ++it)
  {
    if ((*it).second == object)
      return (*it).first;
  }

  return -1;
}
