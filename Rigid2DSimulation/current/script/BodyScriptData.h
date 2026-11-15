#pragma once
#include <memory>
#include <string>
#include "Value.h"

// 原脚本数据保留在脚本模块；当前物理对象不持有它。
struct BodyScriptData
{
    std::wstring scriptText, temporaryScriptText;
    std::shared_ptr<Program> positionProgram, angleProgram, angularVelocityProgram;
    bool hasPositionPreset = false;
    bool hasAnglePreset = false;
    bool hasAngularVelocityPreset = false;
};
