#include "MyString.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

void MyString::allocateAndCopy(const char* src) {
    len = 0;
    while (src[len] != '\0') len++;
    data = new char[len + 1];
    for (int i = 0; i <= len; i++) data[i] = src[i];
}

MyString::MyString() {
    len = 0;
    data = new char[1];
    data[0] = '\0';
}

MyString::MyString(const char* src) {
    if (src == 0) {
        len = 0;
        data = new char[1];
        data[0] = '\0';
    } else {
        allocateAndCopy(src);
    }
}

MyString::MyString(const MyString& other) {
    allocateAndCopy(other.data);
}

MyString::MyString(char singleChar) {
    len = 1;
    data = new char[2];
    data[0] = singleChar;
    data[1] = '\0';
}

MyString::~MyString() {
    delete[] data;
}

MyString& MyString::operator=(const MyString& other) {
    if (this == &other) return *this;
    delete[] data;
    allocateAndCopy(other.data);
    return *this;
}

MyString MyString::operator+(const MyString& other) const {
    char* buffer = new char[len + other.len + 1];
    for (int i = 0; i < len; i++) buffer[i] = data[i];
    for (int i = 0; i < other.len; i++) buffer[len + i] = other.data[i];
    buffer[len + other.len] = '\0';
    MyString result(buffer);
    delete[] buffer;
    return result;
}

MyString& MyString::operator+=(const MyString& other) {
    *this = *this + other;
    return *this;
}

bool MyString::operator==(const MyString& other) const {
    if (len != other.len) return false;
    for (int i = 0; i < len; i++) {
        if (data[i] != other.data[i]) return false;
    }
    return true;
}

bool MyString::operator!=(const MyString& other) const {
    return !(*this == other);
}

char MyString::operator[](int index) const {
    if (index < 0 || index >= len) return '\0';
    return data[index];
}

int MyString::length() const {
    return len;
}

const char* MyString::c_str() const {
    return data;
}

bool MyString::isEmpty() const {
    return len == 0;
}

MyString MyString::substring(int start, int count) const {
    if (start < 0) start = 0;
    if (start >= len) return MyString();
    if (start + count > len) count = len - start;
    if (count < 0) count = 0;

    char* buffer = new char[count + 1];
    for (int i = 0; i < count; i++) buffer[i] = data[start + i];
    buffer[count] = '\0';
    MyString result(buffer);
    delete[] buffer;
    return result;
}

double MyString::toDouble() const {
    return atof(data);
}

MyString MyString::fromDouble(double value, int precision) {
    char buffer[64];
    char format[16];
    // Build format string like "%.4f"
    format[0] = '%';
    format[1] = '.';
    int idx = 2;
    if (precision >= 10) {
        format[idx++] = '0' + (precision / 10);
    }
    format[idx++] = '0' + (precision % 10);
    format[idx++] = 'f';
    format[idx] = '\0';

    snprintf(buffer, sizeof(buffer), format, value);
    return MyString(buffer);
}

MyString MyString::fromInt(int value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return MyString(buffer);
}
