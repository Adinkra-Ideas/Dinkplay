#ifndef MYMPREMOTECC_H
#define MYMPREMOTECC_H

#include <QFile>
#include <QString>
// class QFile;

#include "Top.hpp"  // because we need accessor pointer to the cpp part of this app
class Top;          // Forward-declaring the cpp class is mandatory in order for the compiler to see it

void initMyMPRemoteCC(Top * cppObj);
void destroyMyMPRemoteCC();

void seizeControlOfInfoCenter();
void updateInfoCenter(const char * title,
                      const char * artist);

bool startAccessingSecuredLocation(const char * urlPath);

#endif

// These files and class will be renamed properly after I read and understand appdegelates and controllers
