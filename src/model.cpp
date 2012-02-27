/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* model.cpp
    Contains the implementation of the Model class. */

#include "model.h"

#include <vector>

#include <fstream>
#include <sstream>

using namespace std;

struct Face
{
  int v1, v2, v3, v4;

  Face()
    {  v1 = v2 = v3 = v4 = -1; }
};


Model::Model(const std::string& pName)
  : Object(pName.empty() ? genericName("Model") : pName)
{
  _valid = false;
  _list = 0;
}

Model::~Model()
{
  if (_valid)
    destroy();
}

void Model::destroy()
{
  if (!_valid)
    return;

  glDeleteLists(_list, 1);
  _valid = false;
}

bool Model::load(const std::string& pFileName)
{
  ifstream f(pFileName.c_str());
  if (!f.good())
  {
    return false;
  }

  if (_valid)
    destroy();

  vector<Vector3D> vertices;
  vector<Vector3D> normals;
  vector<Face> faces4;
  vector<Face> faces3;

  int mode = 0;
  int vCount = 0;
  int vI = 0;
  int fCount = 0;
  int fI = 0;

  bool firstLine = true;

  while (!f.eof())
  {
    string line;
    getline(f, line);

    if (line.empty())
      continue;

    if (firstLine)
    {
      if (line != "ply")
        return false;

      firstLine = false;
    }

    if (mode == 0)
    {
      if (line == "end_header")
      {
        mode = 1;
      }
      else
      {
        stringstream s;
        s.str(line);

        string t;
        s >> t;
        if (t == "element")
        {
          s >> t;
          if (t == "vertex")
            s >> vCount;
          else if (t == "face")
            s >> fCount;
        }
      }
    }
    else if (mode == 1)
    {
      stringstream s;
      s.str(line);

      Vector3D v;
      s >> v.x >> v.y >> v.z;
      vertices.push_back(v);

      Vector3D n;
      s >> n.x >> n.y >> n.z;
      normals.push_back(n);

      if (vI == 0)
      {
        _boundMin = _boundMax = v;
      }
      else
      {
        if (v.x < _boundMin.x)
          _boundMin.x = v.x;
        if (v.x > _boundMax.x)
          _boundMax.x = v.x;

        if (v.y < _boundMin.y)
          _boundMin.y = v.y;
        if (v.y > _boundMax.y)
          _boundMax.y = v.y;

        if (v.z < _boundMin.z)
          _boundMin.z = v.z;
        if (v.z > _boundMax.z)
          _boundMax.z = v.z;
      }

      ++vI;
      if (vI >= vCount)
        mode = 2;
    }
    else if (mode == 2)
    {
      stringstream s;
      s.str(line);

      Face face;
      int n = 0;

      s >> n;
      s >> face.v1 >> face.v2 >> face.v3;
      if (n == 4)
      {
        s >> face.v4;
        faces4.push_back(face);
      }
      else
      {
        faces3.push_back(face);
      }

      ++fI;
      if (fI >= fCount)
        break;
    }
  }

  if (faces3.empty() && faces4.empty())
    return false;

  _list = glGenLists(1);

  glNewList(_list, GL_COMPILE);
  {
    glBegin(GL_TRIANGLES);
    {
      for (unsigned int i = 0; i < faces3.size(); ++i)
      {
        glNormal3f(normals[faces3[i].v1].x,
                  normals[faces3[i].v1].y,
                  normals[faces3[i].v1].z);
        glVertex3f(vertices[faces3[i].v1].x,
                  vertices[faces3[i].v1].y,
                  vertices[faces3[i].v1].z);

        glNormal3f(normals[faces3[i].v2].x,
                  normals[faces3[i].v2].y,
                  normals[faces3[i].v2].z);
        glVertex3f(vertices[faces3[i].v2].x,
                  vertices[faces3[i].v2].y,
                  vertices[faces3[i].v2].z);

        glNormal3f(normals[faces3[i].v3].x,
                  normals[faces3[i].v3].y,
                  normals[faces3[i].v3].z);
        glVertex3f(vertices[faces3[i].v3].x,
                  vertices[faces3[i].v3].y,
                  vertices[faces3[i].v3].z);
      }
    }
    glEnd();

    glBegin(GL_QUADS);
    {
      for (unsigned int i = 0; i < faces4.size(); ++i)
      {
        glNormal3f(normals[faces4[i].v1].x,
                   normals[faces4[i].v1].y,
                   normals[faces4[i].v1].z);
        glVertex3f(vertices[faces4[i].v1].x,
                   vertices[faces4[i].v1].y,
                   vertices[faces4[i].v1].z);

        glNormal3f(normals[faces4[i].v2].x,
                   normals[faces4[i].v2].y,
                   normals[faces4[i].v2].z);
        glVertex3f(vertices[faces4[i].v2].x,
                   vertices[faces4[i].v2].y,
                   vertices[faces4[i].v2].z);

        glNormal3f(normals[faces4[i].v3].x,
                   normals[faces4[i].v3].y,
                   normals[faces4[i].v3].z);
        glVertex3f(vertices[faces4[i].v3].x,
                   vertices[faces4[i].v3].y,
                   vertices[faces4[i].v3].z);

        glNormal3f(normals[faces4[i].v4].x,
                   normals[faces4[i].v4].y,
                   normals[faces4[i].v4].z);
        glVertex3f(vertices[faces4[i].v4].x,
                   vertices[faces4[i].v4].y,
                   vertices[faces4[i].v4].z);
      }
    }
    glEnd();
  }
  glEndList();

  _valid = true;

  print(string("Loaded model '") + pFileName + "'");
  print("Stats: " + toString<int>(vCount) + " vert " + toString<int>(faces3.size()) + " tri "
       + toString<int>(faces4.size()) + " quad");

  return true;
}

void Model::render()
{
  if (!_valid)
    return;

  glCallList(_list);
}
