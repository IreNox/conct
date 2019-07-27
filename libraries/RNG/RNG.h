#pragma once

#include <stddef.h>
#include <stdint.h>

class RNGClass
{
public:

    void rand(uint8_t *data, size_t len);
};

extern RNGClass RNG;
