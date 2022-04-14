#include "class_loader.h"

void* ClassImpl::newInstanceWithSize(size_t sizeofClass) {
    void* newInstanceBuffer = new char[sizeofClass];
    noArgsConstructor(newInstanceBuffer);
    return newInstanceBuffer;
}

void ClassImpl::setNoArgsConstructorPointer(void* noArgsConstructorPointer) {
    this->noArgsConstructor =
            reinterpret_cast<void (*)(void*)>(noArgsConstructorPointer);
}

AbstractClass::AbstractClass() : pImpl(new ClassImpl) {}

AbstractClass::~AbstractClass() {
    delete pImpl;
}

void* AbstractClass::newInstanceWithSize(size_t sizeofClass) {
    return pImpl->newInstanceWithSize(sizeofClass);
}

ClassLoaderImpl::~ClassLoaderImpl() {
    for (const auto& item: loadedLibraries) {
        dlclose(item.second);
    }
}

std::string ClassLoaderImpl::getClassPathEnvVariable() {
    const char* classpath = getenv("CLASSPATH");
    if (classpath == nullptr) {
        return "";
    }
    return classpath;
}

std::vector <std::string> ClassLoaderImpl::parseNames(const std::string& name) {
    std::vector <std::string> result;
    size_t from = 0;
    size_t to;
    while ((to = name.find("::", from)) != std::string::npos) {
        result.push_back(name.substr(from, to - from));
        from = to + 2;
    }
    result.push_back(name.substr(from));

    return result;
}

std::string ClassLoaderImpl::makeLibPath(const std::vector <std::string>& names) {
    std::string result;

    std::string classpath = getClassPathEnvVariable();

    if (!classpath.empty() && classpath.back() != '/') {
        classpath += '/';
    }
    result = classpath;

    for (size_t i = 0; i < names.size() - 1; ++i) {
        result += names[i] + '/';
    }
    result += names.back() + ".so";
    return result;
}

std::string ClassLoaderImpl::makeNoArgsConstructorName(
        const std::vector <std::string>& names) {
    std::string result = "_ZN";
    for (const auto& name: names) {
        result += std::to_string(name.size());
        result += name;
    }
    result += "C1Ev";
    return result;
}

void*
ClassLoaderImpl::loadNoArgsConstructorPointer(const std::string& name) {
    std::vector <std::string> names = parseNames(name);
    std::string libPath = makeLibPath(names);
    std::string noArgsConstructorName = makeNoArgsConstructorName(names);

    void* library = nullptr;
    try {
        library = loadedLibraries.at(libPath);
    } catch (const std::out_of_range&) {
        if (access(libPath.c_str(), F_OK) != 0) {
            error = ClassLoaderError::FileNotFound;
            return nullptr;
        }

        library = dlopen(libPath.c_str(), RTLD_NOW);
        if (library == nullptr) {
            error = ClassLoaderError::LibraryLoadError;
            return nullptr;
        }

        loadedLibraries[libPath] = library;
    }

    void* noArgsConstructorPointer =
            dlsym(library, noArgsConstructorName.c_str());
    if (noArgsConstructorPointer == nullptr) {
        error = ClassLoaderError::NoClassInLibrary;
        return nullptr;
    }

    error = ClassLoaderError::NoError;
    return noArgsConstructorPointer;
}

ClassLoaderError ClassLoaderImpl::lastError() {
    return error;
}

ClassLoader::ClassLoader()
        : pImpl(new ClassLoaderImpl) {
}

AbstractClass* ClassLoader::loadClass(const std::string& fullyQualifiedName) {
    void* noArgsConstructorPointer;
    noArgsConstructorPointer =
            pImpl->loadNoArgsConstructorPointer(fullyQualifiedName);
    if (noArgsConstructorPointer == nullptr) {
        return nullptr;
    }

    auto abstractClass = new AbstractClass;
    abstractClass->pImpl->setNoArgsConstructorPointer(noArgsConstructorPointer);
    return abstractClass;
}

ClassLoader::~ClassLoader() {
    delete pImpl;
}

ClassLoaderError ClassLoader::lastError() const {
    return pImpl->lastError();
}
