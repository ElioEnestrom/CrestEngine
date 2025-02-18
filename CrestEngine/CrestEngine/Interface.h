#pragma once
#include <iostream>

class IWritable 
{
public:
	virtual bool WriteTo(std::iostream& file) const = 0;
	virtual bool ReadFrom(std::iostream& file) = 0;
};