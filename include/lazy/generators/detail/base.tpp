#pragma once

#include "lazy/generators/base.h"

// Дает базовому генератору виртуальное разрушение через указатель.
template <class T> Generator<T>::~Generator() {}
