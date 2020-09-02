/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*

Sigslot, a signal-slot library

https://github.com/palacaze/sigslot

MIT License

Copyright (c) 2017 Pierre-Antoine Lacaze

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include "wpi/mutex.h"

namespace wpi {

namespace sig {

namespace trait {

/// represent a list of types
template <typename...> struct typelist {};

/**
 * Pointers that can be converted to a weak pointer concept for tracking
 * purpose must implement the to_weak() function in order to make use of
 * ADL to convert that type and make it usable
 */

template<typename T>
std::weak_ptr<T> to_weak(std::weak_ptr<T> w) {
    return w;
}

template<typename T>
std::weak_ptr<T> to_weak(std::shared_ptr<T> s) {
    return s;
}

// tools
namespace detail {

template <class...>
struct voider { using type = void; };

// void_t from c++17
template <class...T>
using void_t = typename detail::voider<T...>::type;


template <typename, typename, typename = void, typename = void>
struct is_callable : std::false_type {};

template <typename F, typename P, typename... T>
struct is_callable<F, P, typelist<T...>,
        void_t<decltype(((*std::declval<P>()).*std::declval<F>())(std::declval<T>()...))>>
    : std::true_type {};

template <typename F, typename... T>
struct is_callable<F, typelist<T...>,
        void_t<decltype(std::declval<F>()(std::declval<T>()...))>>
    : std::true_type {};


template <typename T, typename = void>
struct is_weak_ptr : std::false_type {};

template <typename T>
struct is_weak_ptr<T, void_t<decltype(std::declval<T>().expired()),
                             decltype(std::declval<T>().lock()),
                             decltype(std::declval<T>().reset())>>
    : std::true_type {};

template <typename T, typename = void>
struct is_weak_ptr_compatible : std::false_type {};

template <typename T>
struct is_weak_ptr_compatible<T, void_t<decltype(to_weak(std::declval<T>()))>>
    : is_weak_ptr<decltype(to_weak(std::declval<T>()))> {};

}  // namespace detail

/// determine if a pointer is convertible into a "weak" pointer
template <typename P>
constexpr bool is_weak_ptr_compatible_v = detail::is_weak_ptr_compatible<std::decay_t<P>>::value;

/// determine if a type T (Callable or Pmf) is callable with supplied arguments in L
template <typename L, typename... T>
constexpr bool is_callable_v = detail::is_callable<T..., L>::value;

}  // namespace trait


namespace detail {

/* SlotState holds slot type independent state, to be used to interact with
 * slots indirectly through connection and ScopedConnection objects.
 */
class SlotState {
public:
    constexpr SlotState() noexcept
        : m_connected(true),
          m_blocked(false) {}

    virtual ~SlotState() = default;

    bool connected() const noexcept { return m_connected; }
    bool disconnect() noexcept { return m_connected.exchange(false); }

    bool blocked() const noexcept { return m_blocked.load(); }
    void block()   noexcept { m_blocked.store(true); }
    void unblock() noexcept { m_blocked.store(false); }

private:
    std::atomic<bool> m_connected;
    std::atomic<bool> m_blocked;
};

}  // namespace detail

/**
 * ConnectionBlocker is a RAII object that blocks a connection until destruction
 */
class ConnectionBlocker {
public:
    ConnectionBlocker() = default;
    ~ConnectionBlocker() noexcept { release(); }

    ConnectionBlocker(const ConnectionBlocker &) = delete;
    ConnectionBlocker & operator=(const ConnectionBlocker &) = delete;

    ConnectionBlocker(ConnectionBlocker && o) noexcept
        : m_state{std::move(o.m_state)}
    {}

    ConnectionBlocker & operator=(ConnectionBlocker && o) noexcept {
        release();
        m_state.swap(o.m_state);
        return *this;
    }

private:
    friend class Connection;
    ConnectionBlocker(std::weak_ptr<detail::SlotState> s) noexcept
        : m_state{std::move(s)}
    {
        auto d = m_state.lock();
        if (d) d->block();
    }

    void release() noexcept {
        auto d = m_state.lock();
        if (d) d->unblock();
    }

private:
    std::weak_ptr<detail::SlotState> m_state;
};


/**
 * A Connection object allows interaction with an ongoing slot connection
 *
 * It allows common actions such as connection blocking and disconnection.
 * Note that Connection is not a RAII object, one does not need to hold one
 * such object to keep the signal-slot connection alive.
 */
class Connection {
public:
    Connection() = default;
    virtual ~Connection() = default;

    Connection(const Connection &) noexcept = default;
    Connection & operator=(const Connection &) noexcept = default;
    Connection(Connection &&) noexcept = default;
    Connection & operator=(Connection &&) noexcept = default;

    bool valid() const noexcept {
        return !m_state.expired();
    }

    bool connected() const noexcept {
        const auto d = m_state.lock();
        return d && d->connected();
    }

    bool disconnect() noexcept {
        auto d = m_state.lock();
        return d && d->disconnect();
    }

    bool blocked() const noexcept {
        const auto d = m_state.lock();
        return d && d->blocked();
    }

    void block() noexcept {
        auto d = m_state.lock();
        if(d)
            d->block();
    }

    void unblock() noexcept {
        auto d = m_state.lock();
        if(d)
            d->unblock();
    }

    ConnectionBlocker blocker() const noexcept {
        return ConnectionBlocker{m_state};
    }

protected:
    template <typename, typename...> friend class SignalBase;
    Connection(std::weak_ptr<detail::SlotState> s) noexcept
        : m_state{std::move(s)}
    {}

protected:
    std::weak_ptr<detail::SlotState> m_state;
};

/**
 * ScopedConnection is a RAII version of Connection
 * It disconnects the slot from the signal upon destruction.
 */
class ScopedConnection : public Connection {
public:
    ScopedConnection() = default;
    ~ScopedConnection() {
        disconnect();
    }

    ScopedConnection(const Connection &c) noexcept : Connection(c) {}
    ScopedConnection(Connection &&c) noexcept : Connection(std::move(c)) {}

    ScopedConnection(const ScopedConnection &) noexcept = delete;
    ScopedConnection & operator=(const ScopedConnection &) noexcept = delete;

    ScopedConnection(ScopedConnection && o) noexcept
        : Connection{std::move(o.m_state)}
    {}

    ScopedConnection & operator=(ScopedConnection && o) noexcept {
        disconnect();
        m_state.swap(o.m_state);
        return *this;
    }

private:
    template <typename, typename...> friend class SignalBase;
    ScopedConnection(std::weak_ptr<detail::SlotState> s) noexcept
        : Connection{std::move(s)}
    {}
};

namespace detail {

template <typename...>
class SlotBase;

template <typename... T>
using SlotPtr = std::shared_ptr<SlotBase<T...>>;

/* A base class for slot objects. This base type only depends on slot argument
 * types, it will be used as an element in an intrusive singly-linked list of
 * slots, hence the public next member.
 */
template <typename... Args>
class SlotBase : public SlotState {
public:
    using base_types = trait::typelist<Args...>;

    virtual ~SlotBase() noexcept = default;

    // method effectively responsible for calling the "slot" function with
    // supplied arguments whenever emission happens.
    virtual void call_slot(Args...) = 0;

    template <typename... U>
    void operator()(U && ...u) {
        if (SlotState::connected() && !SlotState::blocked())
            call_slot(std::forward<U>(u)...);
    }

    SlotPtr<Args...> next;
};

template <typename, typename...> class Slot {};

/*
 * A slot object holds state information, and a callable to to be called
 * whenever the function call operator of its SlotBase base class is called.
 */
template <typename Func, typename... Args>
class Slot<Func, trait::typelist<Args...>> : public SlotBase<Args...> {
public:
    template <typename F>
    constexpr Slot(F && f) : func{std::forward<F>(f)} {}

    virtual void call_slot(Args ...args) override {
        func(args...);
    }

private:
    std::decay_t<Func> func;
};

/*
 * Variation of slot that prepends a Connection object to the callable
 */
template <typename Func, typename... Args>
class Slot<Func, trait::typelist<Connection&, Args...>> : public SlotBase<Args...> {
public:
    template <typename F>
    constexpr Slot(F && f) : func{std::forward<F>(f)} {}

    virtual void call_slot(Args ...args) override {
        func(conn, args...);
    }

    Connection conn;

private:
    std::decay_t<Func> func;
};

/*
 * A slot object holds state information, an object and a pointer over member
 * function to be called whenever the function call operator of its SlotBase
 * base class is called.
 */
template <typename Pmf, typename Ptr, typename... Args>
class Slot<Pmf, Ptr, trait::typelist<Args...>> : public SlotBase<Args...> {
public:
    template <typename F, typename P>
    constexpr Slot(F && f, P && p)
        : pmf{std::forward<F>(f)},
          ptr{std::forward<P>(p)} {}

    virtual void call_slot(Args ...args) override {
        ((*ptr).*pmf)(args...);
    }

private:
    std::decay_t<Pmf> pmf;
    std::decay_t<Ptr> ptr;
};

/*
 * Variation of slot that prepends a Connection object to the callable
 */
template <typename Pmf, typename Ptr, typename... Args>
class Slot<Pmf, Ptr, trait::typelist<Connection&, Args...>> : public SlotBase<Args...> {
public:
    template <typename F, typename P>
    constexpr Slot(F && f, P && p)
        : pmf{std::forward<F>(f)},
          ptr{std::forward<P>(p)} {}

    virtual void call_slot(Args ...args) override {
        ((*ptr).*pmf)(conn, args...);
    }

    Connection conn;

private:
    std::decay_t<Pmf> pmf;
    std::decay_t<Ptr> ptr;
};

template <typename, typename, typename...> class SlotTracked {};

/*
 * An implementation of a slot that tracks the life of a supplied object
 * through a weak pointer in order to automatically disconnect the slot
 * on said object destruction.
 */
template <typename Func, typename WeakPtr, typename... Args>
class SlotTracked<Func, WeakPtr, trait::typelist<Args...>> : public SlotBase<Args...> {
public:
    template <typename F, typename P>
    constexpr SlotTracked(F && f, P && p)
        : func{std::forward<F>(f)},
          ptr{std::forward<P>(p)}
    {}

    virtual void call_slot(Args ...args) override {
        if (! SlotState::connected())
            return;
        if (ptr.expired())
            SlotState::disconnect();
        else
            func(args...);
    }

private:
    std::decay_t<Func> func;
    std::decay_t<WeakPtr> ptr;
};

template <typename, typename, typename...> class SlotPmfTracked {};

/*
 * An implementation of a slot as a pointer over member function, that tracks
 * the life of a supplied object through a weak pointer in order to automatically
 * disconnect the slot on said object destruction.
 */
template <typename Pmf, typename WeakPtr, typename... Args>
class SlotPmfTracked<Pmf, WeakPtr, trait::typelist<Args...>> : public SlotBase<Args...> {
public:
    template <typename F, typename P>
    constexpr SlotPmfTracked(F && f, P && p)
        : pmf{std::forward<F>(f)},
          ptr{std::forward<P>(p)}
    {}

    virtual void call_slot(Args ...args) override {
        if (! SlotState::connected())
            return;
        auto sp = ptr.lock();
        if (!sp)
            SlotState::disconnect();
        else
            ((*sp).*pmf)(args...);
    }

private:
    std::decay_t<Pmf> pmf;
    std::decay_t<WeakPtr> ptr;
};


// noop mutex for thread-unsafe use
struct NullMutex {
    NullMutex() = default;
    NullMutex(const NullMutex &) = delete;
    NullMutex operator=(const NullMutex &) = delete;
    NullMutex(NullMutex &&) = delete;
    NullMutex operator=(NullMutex &&) = delete;

    bool try_lock() { return true; }
    void lock() {}
    void unlock() {}
};

}  // namespace detail


/**
 * SignalBase is an implementation of the observer pattern, through the use
 * of an emitting object and slots that are connected to the signal and called
 * with supplied arguments when a signal is emitted.
 *
 * wpi::SignalBase is the general implementation, whose locking policy must be
 * set in order to decide thread safety guarantees. wpi::Signal and wpi::Signal_st
 * are partial specializations for multi-threaded and single-threaded use.
 *
 * It does not allow slots to return a value.
 *
 * @tparam Lockable a lock type to decide the lock policy
 * @tparam T... the argument types of the emitting and slots functions.
 */
template <typename Lockable, typename... T>
class SignalBase {
    using lock_type = std::unique_lock<Lockable>;
    using SlotPtr = detail::SlotPtr<T...>;

    struct CallSlots {
        SlotPtr m_slots;
        SignalBase& m_base;

        CallSlots(SignalBase& base) : m_base(base) {}

        template <typename... A>
        void operator()(A && ... a) {
            SlotPtr *prev = nullptr;
            SlotPtr *curr = m_slots ? &m_slots : nullptr;

            while (curr) {
                // call non blocked, non connected slots
                if ((*curr)->connected()) {
                    if (!m_base.m_block && !(*curr)->blocked())
                        (*curr)->operator()(a...);
                    prev = curr;
                    curr = (*curr)->next ? &((*curr)->next) : nullptr;
                }
                // remove slots marked as disconnected
                else {
                    if (prev) {
                        (*prev)->next = (*curr)->next;
                        curr = (*prev)->next ? &((*prev)->next) : nullptr;
                    }
                    else
                        curr = (*curr)->next ? &((*curr)->next) : nullptr;
                }
            }
        }
    };

public:
    using arg_list = trait::typelist<T...>;
    using ext_arg_list = trait::typelist<Connection&, T...>;

    SignalBase() noexcept : m_block(false) {}
    ~SignalBase() {
        disconnect_all();
    }

    SignalBase(const SignalBase&) = delete;
    SignalBase & operator=(const SignalBase&) = delete;

    SignalBase(SignalBase && o)
        : m_block{o.m_block.load()}
    {
        lock_type lock(o.m_mutex);
        std::swap(m_func, o.m_func);
    }

    SignalBase & operator=(SignalBase && o) {
        std::scoped_lock lock(m_mutex, o.m_mutex);

        std::swap(m_func, o.m_func);
        m_block.store(o.m_block.exchange(m_block.load()));
        return *this;
    }

    /**
     * Emit a signal
     *
     * Effect: All non blocked and connected slot functions will be called
     *         with supplied arguments.
     * Safety: With proper locking (see wpi::Signal), emission can happen from
     *         multiple threads simultaneously. The guarantees only apply to the
     *         signal object, it does not cover thread safety of potentially
     *         shared state used in slot functions.
     *
     * @param a... arguments to emit
     */
    template <typename... A>
    void operator()(A && ... a) const {
        lock_type lock(m_mutex);
        if (!m_block && m_func) m_func(std::forward<A>(a)...);
    }

    /**
     * Connect a callable of compatible arguments
     *
     * Effect: Creates and stores a new slot responsible for executing the
     *         supplied callable for every subsequent signal emission.
     * Safety: Thread-safety depends on locking policy.
     *
     * @param c a callable
     * @return a Connection object that can be used to interact with the slot
     */
    template <typename Callable>
    void connect(Callable && c) {
        if (!m_func) {
          m_func = std::forward<Callable>(c);
        } else {
          using slot_t = detail::Slot<Callable, arg_list>;
          auto s = std::make_shared<slot_t>(std::forward<Callable>(c));
          add_slot(s);
        }
    }

    /**
     * Connect a callable of compatible arguments, returning a Connection
     *
     * Effect: Creates and stores a new slot responsible for executing the
     *         supplied callable for every subsequent signal emission.
     * Safety: Thread-safety depends on locking policy.
     *
     * @param c a callable
     * @return a Connection object that can be used to interact with the slot
     */
    template <typename Callable>
    std::enable_if_t<trait::is_callable_v<arg_list, Callable>, Connection>
    connect_connection(Callable && c) {
        using slot_t = detail::Slot<Callable, arg_list>;
        auto s = std::make_shared<slot_t>(std::forward<Callable>(c));
        add_slot(s);
        return Connection(s);
    }

    /**
     * Connect a callable with an additional Connection argument
     *
     * The callable's first argument must be of type Connection. This overload
     * the callable to manage it's own connection through this argument.
     *
     * @param c a callable
     * @return a Connection object that can be used to interact with the slot
     */
    template <typename Callable>
    std::enable_if_t<trait::is_callable_v<ext_arg_list, Callable>, Connection>
    connect_extended(Callable && c) {
        using slot_t = detail::Slot<Callable, ext_arg_list>;
        auto s = std::make_shared<slot_t>(std::forward<Callable>(c));
        s->conn = Connection(s);
        add_slot(s);
        return Connection(s);
    }

    /**
     * Overload of connect for pointers over member functions
     *
     * @param pmf a pointer over member function
     * @param ptr an object pointer
     * @return a Connection object that can be used to interact with the slot
     */
    template <typename Pmf, typename Ptr>
    std::enable_if_t<trait::is_callable_v<arg_list, Pmf, Ptr> &&
                     !trait::is_weak_ptr_compatible_v<Ptr>, Connection>
    connect(Pmf && pmf, Ptr && ptr) {
        using slot_t = detail::Slot<Pmf, Ptr, arg_list>;
        auto s = std::make_shared<slot_t>(std::forward<Pmf>(pmf), std::forward<Ptr>(ptr));
        add_slot(s);
        return Connection(s);
    }

    /**
     * Overload  of connect for pointer over member functions and
     *
     * @param pmf a pointer over member function
     * @param ptr an object pointer
     * @return a Connection object that can be used to interact with the slot
     */
    template <typename Pmf, typename Ptr>
    std::enable_if_t<trait::is_callable_v<ext_arg_list, Pmf, Ptr> &&
                     !trait::is_weak_ptr_compatible_v<Ptr>, Connection>
    connect_extended(Pmf && pmf, Ptr && ptr) {
        using slot_t = detail::Slot<Pmf, Ptr, ext_arg_list>;
        auto s = std::make_shared<slot_t>(std::forward<Pmf>(pmf), std::forward<Ptr>(ptr));
        s->conn = Connection(s);
        add_slot(s);
        return Connection(s);
    }

    /**
     * Overload of connect for lifetime object tracking and automatic disconnection
     *
     * Ptr must be convertible to an object following a loose form of weak pointer
     * concept, by implementing the ADL-detected conversion function to_weak().
     *
     * This overload covers the case of a pointer over member function and a
     * trackable pointer of that class.
     *
     * Note: only weak references are stored, a slot does not extend the lifetime
     * of a suppied object.
     *
     * @param pmf a pointer over member function
     * @param ptr a trackable object pointer
     * @return a Connection object that can be used to interact with the slot
     */
    template <typename Pmf, typename Ptr>
    std::enable_if_t<!trait::is_callable_v<arg_list, Pmf> &&
                     trait::is_weak_ptr_compatible_v<Ptr>, Connection>
    connect(Pmf && pmf, Ptr && ptr) {
        using trait::to_weak;
        auto w = to_weak(std::forward<Ptr>(ptr));
        using slot_t = detail::SlotPmfTracked<Pmf, decltype(w), arg_list>;
        auto s = std::make_shared<slot_t>(std::forward<Pmf>(pmf), w);
        add_slot(s);
        return Connection(s);
    }

    /**
     * Overload of connect for lifetime object tracking and automatic disconnection
     *
     * Trackable must be convertible to an object following a loose form of weak
     * pointer concept, by implementing the ADL-detected conversion function to_weak().
     *
     * This overload covers the case of a standalone callable and unrelated trackable
     * object.
     *
     * Note: only weak references are stored, a slot does not extend the lifetime
     * of a suppied object.
     *
     * @param c a callable
     * @param ptr a trackable object pointer
     * @return a Connection object that can be used to interact with the slot
     */
    template <typename Callable, typename Trackable>
    std::enable_if_t<trait::is_callable_v<arg_list, Callable> &&
                     trait::is_weak_ptr_compatible_v<Trackable>, Connection>
    connect(Callable && c, Trackable && ptr) {
        using trait::to_weak;
        auto w = to_weak(std::forward<Trackable>(ptr));
        using slot_t = detail::SlotTracked<Callable, decltype(w), arg_list>;
        auto s = std::make_shared<slot_t>(std::forward<Callable>(c), w);
        add_slot(s);
        return Connection(s);
    }

    /**
     * Creates a connection whose duration is tied to the return object
     * Use the same semantics as connect
     */
    template <typename... CallArgs>
    ScopedConnection connect_scoped(CallArgs && ...args) {
        return connect_connection(std::forward<CallArgs>(args)...);
    }

    /**
     * Disconnects all the slots
     * Safety: Thread safety depends on locking policy
     */
    void disconnect_all() {
        lock_type lock(m_mutex);
        clear();
    }

    /**
     * Blocks signal emission
     * Safety: thread safe
     */
    void block() noexcept {
        m_block.store(true);
    }

    /**
     * Unblocks signal emission
     * Safety: thread safe
     */
    void unblock() noexcept {
        m_block.store(false);
    }

    /**
     * Tests blocking state of signal emission
     */
    bool blocked() const noexcept {
        return m_block.load();
    }

private:
    template <typename S>
    void add_slot(S &s) {
        lock_type lock(m_mutex);
        if (!m_func) {
          // nothing stored
          m_func = CallSlots(*this);
          auto slots = m_func.template target<CallSlots>();
          s->next = slots->m_slots;
          slots->m_slots = s;
        } else if (auto call_slots = m_func.template target<CallSlots>()) {
          // already CallSlots
          s->next = call_slots->m_slots;
          call_slots->m_slots = s;
        } else {
          // was normal std::function, need to move it into a call slot
          using slot_t = detail::Slot<std::function<void(T...)>, arg_list>;
          auto s2 = std::make_shared<slot_t>(
              std::forward<std::function<void(T...)>>(m_func));
          m_func = CallSlots(*this);
          auto slots = m_func.template target<CallSlots>();
          s2->next = slots->m_slots;
          s->next = s2;
          slots->m_slots = s;
        }
    }

    void clear() {
        m_func = nullptr;
    }

private:
    std::function<void(T...)> m_func;
    mutable Lockable m_mutex;
    std::atomic<bool> m_block;
};

/**
 * Specialization of SignalBase to be used in single threaded contexts.
 * Slot connection, disconnection and signal emission are not thread-safe.
 * This is significantly smaller than the thread-safe variant.
 */
template <typename... T>
using Signal = SignalBase<detail::NullMutex, T...>;

/**
 * Specialization of SignalBase to be used in multi-threaded contexts.
 * Slot connection, disconnection and signal emission are thread-safe.
 *
 * Beware of accidentally using recursive signal emission or cycles between
 * two or more signals in your code. Locking std::mutex more than once is
 * undefined behavior, even if it "seems to work somehow". Use signal_r
 * instead for that use case.
 */
template <typename... T>
using Signal_mt = SignalBase<mutex, T...>;

/**
 * Specialization of SignalBase to be used in multi-threaded contexts, allowing
 * for recursive signal emission and emission cycles.
 * Slot connection, disconnection and signal emission are thread-safe.
 */
template <typename... T>
using Signal_r = SignalBase<recursive_mutex, T...>;

}  // namespace sig
}  // namespace wpi
