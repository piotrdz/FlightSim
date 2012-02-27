/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* main.cpp
    The main module with the main() function. */

#include "object.h"
#include "filemanager.h"
#include "application.h"

using namespace std;

int main(int argc, char **argv)
{
  IdDatabase *idDatabase = new IdDatabase();
  FileManager *fileManager = new FileManager();

  Application *application = new Application(argc, argv);

  int code = application->execute();

  delete application;
  delete fileManager;
  delete idDatabase;

  return code;
}
