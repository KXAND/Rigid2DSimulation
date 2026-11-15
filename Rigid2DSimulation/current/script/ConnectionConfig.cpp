#include "ConnectionConfig.h"

#include "Connection.h"
#include "Value.h"
#include "ConfigUtils.h"

void ApplyConnectionConfig(Connection& connection, Value const& configuration)
{
    TryReadConfigNumber(configuration, L"length", connection.length);
    TryReadConfigNumber(configuration, L"springConstant", connection.springConstant);
    if (auto typeValue = FindConfigValue(configuration, L"type"))
    {
        if (typeValue->type == L"str")
        {
            auto typeName = typeValue->stringValue;
            if (typeName == L"rope")
            {
                connection.type = kRopeConnection;
            }
            else if (typeName == L"link")
            {
                connection.type = kLinkConnection;
            }
            else if (typeName == L"spring")
            {
                connection.type = kSpringConnection;
            }
            else if (typeName == L"cord")
            {
                connection.type = kCordConnection;
            }
        }
        else
        {
            connection.type = typeValue->number;
        }
    }
}
