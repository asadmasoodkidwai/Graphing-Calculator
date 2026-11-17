#ifndef MYSTRING_H
#define MYSTRING_H

// A minimal hand-rolled string class so we never touch std::string.
// Stores a heap-allocated, null-terminated char buffer.
class MyString {
private:
    char* data;
    int len;

    void allocateAndCopy(const char* src);

public:
    MyString();                        // empty string
    MyString(const char* src);         // from C string
    MyString(const MyString& other);   // copy constructor
    MyString(char singleChar);         // from a single character
    ~MyString();

    MyString& operator=(const MyString& other);
    MyString operator+(const MyString& other) const;
    MyString& operator+=(const MyString& other);

    bool operator==(const MyString& other) const;
    bool operator!=(const MyString& other) const;

    char operator[](int index) const;

    int length() const;
    const char* c_str() const;
    bool isEmpty() const;

    // Returns substring [start, start+count)
    MyString substring(int start, int count) const;

    // Parses this string as a double. Assumes it is a valid number.
    double toDouble() const;

    static MyString fromDouble(double value, int precision = 4);
    static MyString fromInt(int value);
};

#endif
