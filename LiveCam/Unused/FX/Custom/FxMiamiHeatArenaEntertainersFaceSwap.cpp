#include <fx/FxMiamiHeatArenaEntertainersFaceSwap.h>
#include <utils/DrawUtil.h>
#include <iostream>
#include "utils/Resolutions.h"

extern FaceTracker tracker;

FxMiamiHeatArenaEntertainersFaceSwap::FxMiamiHeatArenaEntertainersFaceSwap()
{
	loadFromJSON("./assets/fx/miamiHeat/miami-heat-entertainers-face-swap-modules.json");
}

FxMiamiHeatArenaEntertainersFaceSwap::~FxMiamiHeatArenaEntertainersFaceSwap()
{
}
