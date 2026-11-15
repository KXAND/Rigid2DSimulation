#include "Creator.h"
#include "Value.h"
#include "ConfigUtils.h"

// 暂不参与应用构建，保留原 DSL 适配实现；恢复接入时需适配新的强类型配置。
CreatorConfig ParseCreatorConfig(Value const& config)
{
    CreatorConfig result;
    if (auto modeValue = FindConfigValue(config, L"mode"))
    {
        if (modeValue->type == L"str")
        {
            auto modeName = modeValue->stringValue;
            if (modeName == L"box")
            {
                result.mode = kCreateBox;
            }
            else if (modeName == L"ball")
            {
                result.mode = kCreateBall;
            }
            else if (modeName == L"plate")
            {
                result.mode = kCreatePlate;
            }
            else if (modeName == L"connection")
            {
                result.mode = kCreateConnection;
            }
            else if (modeName == L"point")
            {
                result.mode = kCreatePoint;
            }
            else if (modeName == L"nail")
            {
                result.mode = kCreateNail;
            }
            else if (modeName == L"particle")
            {
                result.mode = kCreateParticle;
            }
        }
        else
        {
            result.mode = modeValue->number;
        }
    }
    TryReadConfigNumber(config, L"thickness", result.thickness);
    TryReadConfigNumber(config, L"radius", result.radius);
    if (auto value = FindConfigValue(config, L"bodyConfiguration"))
    {
        result.bodyConfiguration = std::make_shared<Value>(*value);
    }
    if (auto value = FindConfigValue(config, L"connectionConfiguration"))
    {
        result.connectionConfiguration = std::make_shared<Value>(*value);
    }
    return result;
}
