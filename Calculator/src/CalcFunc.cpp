#pragma once
#include "Common.h"

/// <summary>
/// Mathematical operations used by CalcIOStreamObj
/// </summary>

void OpAdd(float amt, float& value)
{
	value = value + amt;
}

void OpSubtract(float amt, float& value)
{
	value = value - amt;
}

void OpDivide(float amt, float& value)
{
	value = value / amt;
}

void OpMultiply(float amt, float& value)
{
	value = value * amt;
}


