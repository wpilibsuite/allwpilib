// Copyright (c) Sleipnir contributors

#pragma once

#include <cstddef>
#include <memory>

#include "sleipnir/util/SmallVector.hpp"
#include "sleipnir/util/SymbolExports.hpp"

namespace sleipnir {

/**
 * This class implements a pool memory resource.
 *
 * The pool allocates chunks of memory and splits them into blocks managed by a
 * free list. Allocations return pointers from the free list, and deallocations
 * return pointers to the free list.
 */
class SLEIPNIR_DLLEXPORT PoolResource {
 public:
  /**
   * Constructs a default PoolResource.
   *
   * @param blocksPerChunk Number of blocks per chunk of memory.
   */
  explicit PoolResource(size_t blocksPerChunk)
      : blocksPerChunk{blocksPerChunk} {}

  PoolResource(const PoolResource&) = delete;
  PoolResource& operator=(const PoolResource&) = delete;
  PoolResource(PoolResource&&) = default;
  PoolResource& operator=(PoolResource&&) = default;

  /**
   * Returns a block of memory from the pool.
   *
   * @param bytes Number of bytes in the block.
   * @param alignment Alignment of the block (unused).
   */
  [[nodiscard]]
  void* allocate(size_t bytes, size_t alignment = alignof(std::max_align_t)) {
    if (m_freeList.empty()) {
      AddChunk(bytes);
    }

    auto ptr = m_freeList.back();
    m_freeList.pop_back();
    return ptr;
  }

  /**
   * Gives a block of memory back to the pool.
   *
   * @param p A pointer to the block of memory.
   * @param bytes Number of bytes in the block (unused).
   * @param alignment Alignment of the block (unused).
   */
  void deallocate(void* p, size_t bytes,
                  size_t alignment = alignof(std::max_align_t)) {
    m_freeList.emplace_back(p);
  }

  /**
   * Returns true if this pool resource has the same backing storage as another.
   */
  bool is_equal(const PoolResource& other) const noexcept {
    return this == &other;
  }

  /**
   * Returns the number of blocks from this pool resource that are in use.
   */
  size_t blocks_in_use() const noexcept {
    return m_buffer.size() * blocksPerChunk - m_freeList.size();
  }

 private:
  small_vector<std::unique_ptr<std::byte[]>> m_buffer;
  small_vector<void*> m_freeList;
  size_t blocksPerChunk;

  /**
   * Adds a memory chunk to the pool, partitions it into blocks with the given
   * number of bytes, and appends pointers to them to the free list.
   *
   * @param bytesPerBlock Number of bytes in the block.
   */
  void AddChunk(size_t bytesPerBlock) {
    m_buffer.emplace_back(new std::byte[bytesPerBlock * blocksPerChunk]);
    for (int i = blocksPerChunk - 1; i >= 0; --i) {
      m_freeList.emplace_back(m_buffer.back().get() + bytesPerBlock * i);
    }
  }
};

/**
 * This class is an allocator for the pool resource.
 *
 * @tparam T The type of object in the pool.
 */
template <typename T>
class PoolAllocator {
 public:
  /**
   * The type of object in the pool.
   */
  using value_type = T;

  /**
   * Constructs a pool allocator with the given pool memory resource.
   *
   * @param r The pool resource.
   */
  explicit constexpr PoolAllocator(PoolResource* r) : m_memoryResource{r} {}

  constexpr PoolAllocator(const PoolAllocator<T>& other) = default;
  constexpr PoolAllocator<T>& operator=(const PoolAllocator<T>&) = default;

  /**
   * Returns a block of memory from the pool.
   *
   * @param n Number of bytes in the block.
   */
  [[nodiscard]]
  constexpr T* allocate(size_t n) {
    return static_cast<T*>(m_memoryResource->allocate(n));
  }

  /**
   * Gives a block of memory back to the pool.
   *
   * @param p A pointer to the block of memory.
   * @param n Number of bytes in the block.
   */
  constexpr void deallocate(T* p, size_t n) {
    m_memoryResource->deallocate(p, n);
  }

 private:
  PoolResource* m_memoryResource;
};

/**
 * Returns a global pool memory resource.
 */
SLEIPNIR_DLLEXPORT PoolResource& GlobalPoolResource();

/**
 * Returns an allocator for a global pool memory resource.
 *
 * @tparam T The type of object in the pool.
 */
template <typename T>
PoolAllocator<T> GlobalPoolAllocator() {
  return PoolAllocator<T>{&GlobalPoolResource()};
}

}  // namespace sleipnir
