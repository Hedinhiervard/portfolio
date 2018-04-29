/**
 * Holds the main (owning) pointer to the object
 * the pointer can spawn as many observer_ptr's as you like.
 * Can't be copied, but can be moved.
 * When the owning_ptr is destroyed, the object is released
 * and all observer_ptr's are invalidated.
 * There is no way to extend the object's lifetime beyond the
 * lifetime of the owning_ptr.
 */

#pragma once

#include <memory>

#include "iocpputil/observer_ptr.h"

namespace iocpputil
{
    template<typename T>
    class owning_ptr
    {
    public:
        template<typename> friend class owning_ptr;

        owning_ptr()
        {}

        owning_ptr(T* raw)
        :m_ptr(raw)
        {}

        owning_ptr(const owning_ptr& other) = delete;
        owning_ptr& operator= (const owning_ptr& other) = delete;

        owning_ptr& operator= (std::nullptr_t)
        {
            m_ptr.reset();
            return *this;
        }

        template<typename OtherType>
        owning_ptr(const owning_ptr<OtherType>& other) = delete;

        inline bool operator==(T* other) const
        {
            return m_ptr.get() == other;
        }

        template<typename U = T>
        inline
        typename std::enable_if<!std::is_const<U>::value, bool>::type
        operator==(const T* other) const
        {
            return m_ptr.get() == other;
        }

        inline bool operator!=(T* other) const
        {
            return !operator==(other);
        }

        template<typename U = T>
        inline
        typename std::enable_if<!std::is_const<U>::value, bool>::type
        operator!=(const T* other) const
        {
            return !operator==(other);
        }

        template<typename OtherType>
        owning_ptr<T>& operator= (const owning_ptr<OtherType>& other) = delete;

        template<typename OtherType>
        owning_ptr(owning_ptr<OtherType>&& other)
        : m_ptr(std::dynamic_pointer_cast<T>(std::move(other.m_ptr)))
        {}

        template<typename OtherType>
        owning_ptr<T>& operator= (owning_ptr<OtherType>&& other)
        {
            m_ptr = std::dynamic_pointer_cast<T>(std::move(other.m_ptr));
            return *this;
        }

        template<typename Target>
        observer_ptr<Target> create_observer() const
        {
            return observer_ptr<Target>(std::dynamic_pointer_cast<Target>(m_ptr));
        }

        observer_ptr<T> create_observer() const
        {
            return observer_ptr<T>(m_ptr);
        }

        template<typename Target = T>
        observer_ptr<const Target> create_const_observer() const
        {
            return observer_ptr<const Target>(std::dynamic_pointer_cast<const Target>(m_ptr));
        }

        operator observer_ptr<T>() const
        {
            return create_observer();
        }

        T& operator*()
        {
            assert(m_ptr != nullptr);
            return *(m_ptr.get());
        }

        T* operator->()
        {
            assert(m_ptr != nullptr);
            return m_ptr.get();
        }

        const T* operator->() const
        {
            assert(m_ptr != nullptr);
            return m_ptr.get();
        }

        inline bool valid() const
        {
            return m_ptr != nullptr;
        }
        operator bool() const
        {
            return valid();
        }

        inline void reset()
        {
            m_ptr.reset();
        }

        inline T* get()
        {
            return m_ptr.get();
        }

        inline const T* get() const
        {
            return m_ptr.get();
        }

    private:
        std::shared_ptr<T> m_ptr;
    };

    template <typename T, typename... Args>
    owning_ptr<T> make_owning(Args&&... args)
    {
        return owning_ptr<T>(new T(std::forward<Args>(args)...));
    }
};
