#pragma once

float clampf(float a, float b, float c)
{
	if (a < b) a = b; 
	if (a > c) a = c;
	return a;
}