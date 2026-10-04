#include "BodyConfig.h"

#include "Body.h"
#include "Value.h"
#include "current/common/utils.h"

void ApplyBodyConfig(Body& body, Value const& config)
{
    if (auto value = FindConfigValue(config, L"position"))
    {
        body.position = ToVector2(*value);
    }
    if (auto value = FindConfigValue(config, L"velocity"))
    {
        body.velocity = ToVector2(*value);
    }
    if (auto value = FindConfigValue(config, L"innerColor"))
    {
        body.appearance.innerColor = (Color)ToVector3(*value);
    }
    TryReadConfigNumber(config, L"radian", body.radian);
    TryReadConfigNumber(config, L"velocityAngular", body.velocityAngular);
    TryReadConfigNumber(config, L"elasticity", body.elasticity);
    TryReadConfigNumber(config, L"frictionStatic", body.frictionStatic);
    TryReadConfigNumber(config, L"frictionDynamic", body.frictionDynamic);
    TryReadConfigNumber(config, L"dampCoeff", body.dampCoeff);
    TryReadConfigNumber(config, L"dampCoeffAngular", body.dampCoeffAngular);
    TryReadConfigNumber(config, L"fixed", body.fixed);
    TryReadConfigNumber(config, L"density", body.density);
    TryReadConfigNumber(config, L"chargeDensity", body.chargeDensity);
}
