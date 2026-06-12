#pragma once

// По умолчанию генератор не привязывается к внешней истории.
template <class T> void Generator<T>::bind_source(const Sequence<T> *) {}

// Дает базовому генератору виртуальное разрушение через указатель.
template <class T> Generator<T>::~Generator() {}
