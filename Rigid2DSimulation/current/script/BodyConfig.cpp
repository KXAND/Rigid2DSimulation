#include "BodyConfig.h"

#include "Value.h"
#include "current/common/utils.h"

BodyProperties ParseBodyProperties(Value const& config, BodyProperties defaults)
{
    auto properties = defaults;
    if (auto value = FindConfigValue(config, L"position"))
    {
        properties.position = ToVector2(*value);
    }
    if (auto value = FindConfigValue(config, L"velocity"))
    {
        properties.velocity = ToVector2(*value);
    }
    if (auto value = FindConfigValue(config, L"innerColor"))
    {
        properties.innerColor = (Color)ToVector3(*value);
    }
    TryReadConfigNumber(config, L"radian", properties.radian);
    TryReadConfigNumber(config, L"velocityAngular", properties.velocityAngular);
    TryReadConfigNumber(config, L"elasticity", properties.elasticity);
    TryReadConfigNumber(config, L"frictionStatic", properties.frictionStatic);
    TryReadConfigNumber(config, L"frictionDynamic", properties.frictionDynamic);
    TryReadConfigNumber(config, L"dampCoeff", properties.dampCoeff);
    TryReadConfigNumber(config, L"dampCoeffAngular", properties.dampCoeffAngular);
    TryReadConfigNumber(config, L"fixed", properties.fixed);
    TryReadConfigNumber(config, L"density", properties.density);
    TryReadConfigNumber(config, L"chargeDensity", properties.chargeDensity);
    return properties;
}
