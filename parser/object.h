#pragma once

#include <memory>
#include <string>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    explicit Number(const int value);
    int GetValue() const;

    int value_;
};

class Bool : public Object {
public:
    explicit Bool(const bool value);
    bool GetValue() const;

    bool value_;
};

class Symbol : public Object {
public:
    explicit Symbol(const std::string& name);
    const std::string& GetName() const;

    std::string name_;
};

class Cell : public Object {
public:
    std::shared_ptr<Object> GetFirst() const;
    std::shared_ptr<Object> GetSecond() const;

    std::shared_ptr<Object> first_ = nullptr;
    std::shared_ptr<Object> second_ = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and conversion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    if (auto new_ptr = std::dynamic_pointer_cast<T>(obj)) {
        return new_ptr;
    }
    return nullptr;
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (auto new_ptr = std::dynamic_pointer_cast<T>(obj)) {
        return true;
    }
    return false;
}
