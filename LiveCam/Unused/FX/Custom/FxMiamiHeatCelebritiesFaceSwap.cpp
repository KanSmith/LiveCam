#include <fx/FxMiamiHeatCelebritiesFaceSwap.h>
#include <utils/DrawUtil.h>
#include <iostream>
#include "utils/Resolutions.h"

extern FaceTracker tracker;

FxMiamiHeatCelebritiesFaceSwap::FxMiamiHeatCelebritiesFaceSwap()
{
	loadFromJSON("./assets/fx/miamiHeat/miami-heat-celebrities-face-swap-modules.json");
}

FxMiamiHeatCelebritiesFaceSwap::~FxMiamiHeatCelebritiesFaceSwap()
{
}
