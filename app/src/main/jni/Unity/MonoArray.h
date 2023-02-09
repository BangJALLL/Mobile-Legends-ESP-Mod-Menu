#pragma once

template <typename T>
struct monoArray {
    void* klass;
    void* monitor;
    void* bounds;
    int max_length;
    void* vector [1];

    int getLength() {
        return max_length;
    }

    T getPointer() {
        return (T)vector;
    }
};