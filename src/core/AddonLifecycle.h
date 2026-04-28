#ifndef ADDON_LIFECYCLE_H
#define ADDON_LIFECYCLE_H
#include "Nexus.h"

void AddonLoad(AddonAPI_t *aApi);
void AddonUnload();
void AddonRender();

#endif