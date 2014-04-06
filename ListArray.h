#pragma once

#include "StaticArray\StaticArray1.hpp"
#include "List\List1.hpp"

enum {low = 0, high = 2};

typedef StaticArray1 <List1<Integer>, lowerBound, upperBound> ListArray;