#include <fx/FxChicagoBullsFaceSwap.h>
#include <utils/DrawUtil.h>
#include <iostream>
#include "utils/Resolutions.h"

extern FaceTracker tracker;

FxChicagoBullsFaceSwap::FxChicagoBullsFaceSwap()
{
	loadFromJSON("./assets/fx/chicagoBulls/chicago-bulls-players-face-swap-modules.json");
}

FxChicagoBullsFaceSwap::~FxChicagoBullsFaceSwap()
{
}
