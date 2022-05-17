#pragma once
#include "pch.h"

struct Name {
	char name[48];
	Name(const char* pname) {
		strcpy_s(this->name, pname);
	}
};

struct Light {
	float intensity = 100;
};