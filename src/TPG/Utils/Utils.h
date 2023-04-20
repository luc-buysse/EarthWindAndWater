#pragma once

#ifndef UTILS
#define UTILS

#include <vector>
#include <cgp/cgp.hpp>
#include <functional>
#include <chrono>
#include <queue>
#include <random>
#include <map>
#include <stack>
#include "KdTree.h"

#define DEBUG false

using namespace cgp;
using namespace std;

bool operator ==(vec3 a, vec3 b);

vec3 randDir();

#endif