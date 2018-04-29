/**
 * Can be spawned from mother owning_ptr.
 * If the mother owning_ptr gets released, invalidates.
 * Can be verified with .valid() call.
 * Can be dereferenced if the object still exists.
 * Otherwise raises the assert.
 */

#pragma once

#include <memory>
#include <cassert>

namespace iocpputil
{
    template <typename T>
    class observer_ptr
    {
    public:
        observer_ptr()
        {}


        observer_ptr(std::shared_ptr<T> ptr)
        : m_weak(ptr)
        {}

        observer_ptr(std::nullptr_t null)
        {}

        observer_ptr(T* object)
        : m_weak(std::dynamic_pointer_cast<T>(object->shared_from_this()))
        {}

        T& operator*()
        {
            assert(!m_weak.expired());
            return *m_weak.lock().get();
        }

        const T& operator*() const
        {
            assert(!m_weak.expired());
            return *m_weak.lock().get();
        }

        T* operator->()
        {
            assert(!m_weak.expired());
            return m_weak.lock().get();
        }

        const T* operator->() const
        {
            assert(!m_weak.expired());
            return m_weak.lock().get();
        }

        T* get()
        {
            assert(!m_weak.expired());
            return m_weak.lock().get();
        }

        const T* get() const
        {
            assert(!m_weak.expired());
            return m_weak.lock().get();
        }

        inline bool valid() const
        {
            return !m_weak.expired();
        }

        operator bool() const
        {
            return valid();
        }

        template<typename U>
        inline bool operator==(const observer_ptr<U>& other) const
        {
            return (!valid() && !other.valid()) ||
            (valid() && other.valid() && get() == other.get());
        }

        inline bool operator==(const observer_ptr& other) const
        {
            return (!valid() && !other.valid()) ||
            (m_weak.lock() == other.m_weak.lock());
        }

        inline bool operator==(const T* other) const
        {
            return (!valid() && other == nullptr) ||
            (valid() && m_weak.lock().get() == other);
        }

        inline bool operator!=(const T* other) const
        {
            return !operator==(other);
        }

        template<typename U>
        inline bool operator!=(const observer_ptr<U>& other) const
        {
            return !operator==<U>(other);
        }

        inline bool operator!=(const observer_ptr& other) const
        {
            return !operator==(other);
        }

        inline bool operator==(std::nullptr_t other) const
        {
            return !valid();
        }

        inline bool operator!=(std::nullptr_t other) const
        {
            return !operator==(other);
        }

        inline operator observer_ptr<const T>() const
        {
            assert(!m_weak.expired());
            return observer_ptr<const T>(std::dynamic_pointer_cast<const T>(m_weak.lock()));
        }

        inline void reset()
        {
            m_weak.reset();
        }

        template<typename NEW>
        observer_ptr<NEW> cast() const
        {
            assert(!m_weak.expired());
            return observer_ptr<NEW>(std::dynamic_pointer_cast<NEW>(m_weak.lock()));
        }

    private:
        std::weak_ptr<T> m_weak;
    };
};
