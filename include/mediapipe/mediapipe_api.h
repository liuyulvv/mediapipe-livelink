#ifndef MEDIAPIPE_API_H_
#define MEDIAPIPE_API_H_

#include <memory>
#include "mediapipe_interface.h"

#ifndef DllExport
#define DllExport __declspec(dllexport)
#endif

DllExport MediapipeInterface* CreateMediapipeInterface();

#endif