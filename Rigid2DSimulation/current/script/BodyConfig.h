#pragma once

#include "Body.h"

struct Value;

BodyProperties ParseBodyProperties(Value const& config, BodyProperties defaults = {});
