#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

// A minimal hand-rolled growable array so we never touch std::vector.
// Template must stay header-only (standard C++ rule for templates).
template <typename T>
class DynamicArray {
private:
    T* items;
    int count;
    int capacity;

    void grow() {
        int newCapacity = (capacity == 0) ? 4 : capacity * 2;
        T* newItems = new T[newCapacity];
        for (int i = 0; i < count; i++) newItems[i] = items[i];
        delete[] items;
        items = newItems;
        capacity = newCapacity;
    }

public:
    DynamicArray() {
        items = 0;
        count = 0;
        capacity = 0;
    }

    DynamicArray(const DynamicArray<T>& other) {
        count = other.count;
        capacity = other.capacity;
        if (capacity > 0) {
            items = new T[capacity];
            for (int i = 0; i < count; i++) items[i] = other.items[i];
        } else {
            items = 0;
        }
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        if (this == &other) return *this;
        delete[] items;
        count = other.count;
        capacity = other.capacity;
        if (capacity > 0) {
            items = new T[capacity];
            for (int i = 0; i < count; i++) items[i] = other.items[i];
        } else {
            items = 0;
        }
        return *this;
    }

    ~DynamicArray() {
        delete[] items;
    }

    void pushBack(const T& value) {
        if (count >= capacity) grow();
        items[count] = value;
        count++;
    }

    void removeAt(int index) {
        if (index < 0 || index >= count) return;
        for (int i = index; i < count - 1; i++) items[i] = items[i + 1];
        count--;
    }

    void clear() {
        count = 0;
    }

    T& operator[](int index) {
        return items[index];
    }

    const T& operator[](int index) const {
        return items[index];
    }

    int size() const {
        return count;
    }

    bool isEmpty() const {
        return count == 0;
    }
};

#endif
