//===-- llvm/Support/ManagedStatic.h - Static Global wrapper ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the ManagedStatic class and the wpi_shutdown() function.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_MANAGEDSTATIC_H
#define WPIUTIL_WPI_MANAGEDSTATIC_H

#include <atomic>
#include <cstddef>

namespace wpi {

/// object_creator - Helper method for ManagedStatic.
template <class C> struct object_creator {
  static void *call() { return new C(); }
};

/// object_deleter - Helper method for ManagedStatic.
///
template <typename T> struct object_deleter {
  static void call(void *Ptr) { delete (T *)Ptr; }
};
template <typename T, size_t N> struct object_deleter<T[N]> {
  static void call(void *Ptr) { delete[](T *)Ptr; }
};

/// ManagedStaticBase - Common base class for ManagedStatic instances.
class ManagedStaticBase {
protected:
  // This should only be used as a static variable, which guarantees that this
  // will be zero initialized.
  mutable std::atomic<void *> Ptr;
  mutable void (*DeleterFn)(void*);
  mutable const ManagedStaticBase *Next;

  void RegisterManagedStatic(void *(*creator)(), void (*deleter)(void*)) const;
  void RegisterManagedStatic(void *created, void (*deleter)(void*)) const;

public:
  /// isConstructed - Return true if this object has not been created yet.
  bool isConstructed() const { return Ptr != nullptr; }

  void destroy() const;
};

/// ManagedStatic - This transparently changes the behavior of global statics to
/// be lazily constructed on demand (good for reducing startup times of dynamic
/// libraries that link in LLVM components) and for making destruction be
/// explicit through the wpi_shutdown() function call.
///
template <class C, class Creator = object_creator<C>,
          class Deleter = object_deleter<C>>
class ManagedStatic : public ManagedStaticBase {
public:
  ManagedStatic() = default;

  ManagedStatic(C* created, void(*deleter)(void*)) {
    RegisterManagedStatic(created, deleter);
  }

  // Accessors.
  C &operator*() {
    void *Tmp = Ptr.load(std::memory_order_acquire);
    if (!Tmp)
      RegisterManagedStatic(Creator::call, Deleter::call);

    return *static_cast<C *>(Ptr.load(std::memory_order_relaxed));
  }

  C *operator->() { return &**this; }

  const C &operator*() const {
    void *Tmp = Ptr.load(std::memory_order_acquire);
    if (!Tmp)
      RegisterManagedStatic(Creator::call, Deleter::call);

    return *static_cast<C *>(Ptr.load(std::memory_order_relaxed));
  }

  const C *operator->() const { return &**this; }
};

/// wpi_shutdown - Deallocate and destroy all ManagedStatic variables.
void wpi_shutdown();

/// wpi_shutdown_obj - This is a simple helper class that calls
/// wpi_shutdown() when it is destroyed.
struct wpi_shutdown_obj {
  wpi_shutdown_obj() = default;
  ~wpi_shutdown_obj() { wpi_shutdown(); }
};

} // end namespace wpi

#endif // WPIUTIL_WPI_MANAGEDSTATIC_H
