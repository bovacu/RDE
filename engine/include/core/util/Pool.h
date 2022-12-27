//
// Created by borja on 9/8/22.
//

#ifndef RDE_POOL_H
#define RDE_POOL_H

#include <vector>
#include "core/util/Delegate.h"
#include "core/util/Functions.h"

namespace RDE {

    template<class T>
    class Pool {
        private:
            std::vector<T> pool {};
            int currentNumberOfInstances = 0;

        public:
            int initialNumberOfElements = -1;
            int numberOfNewElementsToCreateIfNotEnough = -1;
            int maxNumberOfElements = -1;
            UDelegate<T()> allocator {};

        public:
            void init(UDelegate<T()> _allocator, int _initialNumberOfElements = 100, int _numberOfNewElementsToCreateIfNotEnough = 25, int _maxNumberOfElements = 1000);
            T* getElement();
            void returnElement(T* _element);
            void clear();
    };

    template<class T>
    void Pool<T>::init(UDelegate<T()> _allocator, int _initialNumberOfElements, int _numberOfNewElementsToCreateIfNotEnough, int _maxNumberOfElements) {
        pool.reserve(_initialNumberOfElements);
        for(auto _i = 0; _i < _initialNumberOfElements; _i++) {
            auto _element = _allocator();
            pool.template emplace_back(_element);
        }

        currentNumberOfInstances = _initialNumberOfElements;
        initialNumberOfElements = _initialNumberOfElements;
        numberOfNewElementsToCreateIfNotEnough = _numberOfNewElementsToCreateIfNotEnough;
        maxNumberOfElements = _maxNumberOfElements;
        allocator = _allocator;
    }

    template<class T>
    T* Pool<T>::getElement() {
        if(pool.size() - 1 == 0) {
            auto _amountToCreate = numberOfNewElementsToCreateIfNotEnough;

            if(currentNumberOfInstances + _amountToCreate > maxNumberOfElements) {
                _amountToCreate = maxNumberOfElements - currentNumberOfInstances;
            }

            for(auto _i = 0; _i < _amountToCreate; _i++) {
                auto _particle = allocator();
                pool.push_back(_particle);
            }
        }

        #ifdef ENGINE_DEBUG
        if(pool.empty()) {
            #if IS_WINDOWS()
            throw std::runtime_error(Util::String::appendToString("There are no more elements in the pool, the maximum number of elements was reached and no element was returned yet to the pool!", __LINE__, " in file ", __FILE__));
            #else
                throw std::runtime_error(Util::String::appendToString("There are no more elements in the pool, the maximum number of elements was reached and no element was returned yet to the pool!", __LINE__, " in file ", __FILE_NAME__));
            #endif
        }
        #endif

        auto _toReturn = &pool.back();
        pool.pop_back();
        return _toReturn;
    }

    template<class T>
    void Pool<T>::returnElement(T* _element) {
        pool.push_back(*_element);
    }

    template<class T>
    void Pool<T>::clear() {
        pool.clear();
    }
}


#endif //RDE_POOL_H
