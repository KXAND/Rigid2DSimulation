#pragma once
#include "framework/basic/Utils.h"

struct SceneParameter
{
    wstring name;
    double value = 0;
    bool isMarkedForDeletion = false;

    SceneParameter(wstring const& name = {});
};
