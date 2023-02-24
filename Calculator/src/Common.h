#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include "CalcIOStreamObj.h"
#include <string>
#include <imgui_internal.h>
#include <sstream>

// Declare functions from CalcFunc.cpp
void OpAdd(float amt, float& value);
void OpSubtract(float amt, float& value);
void OpDivide(float by, float& value);
void OpMultiply(float by, float& value);

