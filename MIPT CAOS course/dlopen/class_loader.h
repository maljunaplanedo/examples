#pragma once

#include <dlfcn.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <variant>
#include <vector>

class AbstractClass {
    friend class ClassLoader;

public:
    AbstractClass();

    ~AbstractClass();

protected:
    void* newInstanceWithSize(size_t sizeofClass);

    struct ClassImpl* pImpl;
};

template<class T>
class Class
        : public AbstractClass {
public:
    T* newInstance() {
        size_t classSize = sizeof(T);
        void* rawPtr = newInstanceWithSize(classSize);
        return reinterpret_cast<T*>(rawPtr);
    }
};

enum class ClassLoaderError {
    NoError = 0,
    FileNotFound,
    LibraryLoadError,
    NoClassInLibrary
};


class ClassLoader {
public:
    ClassLoader();

    AbstractClass* loadClass(const std::string& fullyQualifiedName);

    ClassLoaderError lastError() const;

    ~ClassLoader();

private:
    struct ClassLoaderImpl* pImpl;
};

struct ClassImpl {
    void* newInstanceWithSize(size_t sizeofClass);

    void setNoArgsConstructorPointer(void* noArgsConstructorPointer);

private:
    void (* noArgsConstructor)(void*){nullptr};
};

struct ClassLoaderImpl {
private:
    static std::string getClassPathEnvVariable();

    static std::vector <std::string> parseNames(const std::string& name);

    static std::string makeLibPath(const std::vector <std::string>& names);

    static std::string
    makeNoArgsConstructorName(const std::vector <std::string>& names);

public:
    void*
    loadNoArgsConstructorPointer(const std::string& name);

    ClassLoaderError lastError();

    ~ClassLoaderImpl();

private:
    std::unordered_map<std::string, void*> loadedLibraries;
    ClassLoaderError error{ClassLoaderError::NoError};
};
