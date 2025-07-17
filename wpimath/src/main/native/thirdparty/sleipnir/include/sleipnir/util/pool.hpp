// Copyright (c) Sleipnir contributors

#pragma once

#include <cstddef>
#include <memory>

#include <gch/small_vector.hpp>

#include "sleipnir/util/symbol_exports.hpp"

namespace slp {

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
   * @param blocks_per_chunk Number of blocks per chunk of memory.
   */
  explicit PoolResource(size_t blocks_per_chunk)
      : blocks_per_chunk{blocks_per_chunk} {}

  /**
   * Copy constructor.
   */
  PoolResource(const PoolResource&) = delete;

  /**
   * Copy assignment operator.
   *
   * @return This pool resource.
   */
  PoolResource& operator=(const PoolResource&) = delete;

  /**
   * Move constructor.
   */
  PoolResource(PoolResource&&) = default;

  /**
   * Move assignment operator.
   *
   * @return This pool resource.
   */
  PoolResource& operator=(PoolResource&&) = default;

  /**
   * Returns a block of memory from the pool.
   *
   * @param bytes Number of bytes in the block.
   * @param alignment Alignment of the block (unused).
   * @return A block of memory from the pool.
   */
  [[nodiscard]]
  void* allocate(size_t bytes, [[maybe_unused]] size_t alignment =
                                   alignof(std::max_align_t)) {
    if (m_free_list.empty()) {
      add_chunk(bytes);
    }

    auto ptr = m_free_list.back();
    m_free_list.pop_back();
    return ptr;
  }

  /**
   * Gives a block of memory back to the pool.
   *
   * @param p A pointer to the block of memory.
   * @param bytes Number of bytes in the block (unused).
   * @param alignment Alignment of the block (unused).
   */
  void deallocate(
      void* p, [[maybe_unused]] size_t bytes,
      [[maybe_unused]] size_t alignment = alignof(std::max_align_t)) {
    m_free_list.emplace_back(p);
  }

  /**
   * Returns true if this pool resource has the same backing storage as another.
   *
   * @param other The other pool resource.
   * @return True if this pool resource has the same backing storage as another.
   */
  bool is_equal(const PoolResource& other) const noexcept {
    return this == &other;
  }

  /**
   * Returns the number of blocks from this pool resource that are in use.
   *
   * @return The number of blocks from this pool resource that are in use.
   */
  size_t blocks_in_use() const noexcept {
    return m_buffer.size() * blocks_per_chunk - m_free_list.size();
  }

 private:
  gch::small_vector<std::unique_ptr<std::byte[]>> m_buffer;
  gch::small_vector<void*> m_free_list;
  size_t blocks_per_chunk;

  /**
   * Adds a memory chunk to the pool, partitions it into blocks with the given
   * number of bytes, and appends pointers to them to the free list.
   *
   * @param bytes_per_block Number of bytes in the block.
   */
  void add_chunk(size_t bytes_per_block) {
    m_buffer.emplace_back(new std::byte[bytes_per_block * blocks_per_chunk]);
    for (int i = blocks_per_chunk - 1; i >= 0; --i) {
      m_free_list.emplace_back(m_buffer.back().get() + bytes_per_block * i);
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
  explicit constexpr PoolAllocator(PoolResource* r) : m_memory_resource{r} {}

  /**
   * Copy constructor.
   */
  constexpr PoolAllocator(const PoolAllocator<T>&) = default;

  /**
   * Copy assignment operator.
   *
   * @return This pool allocator.
   */
  constexpr PoolAllocator<T>& operator=(const PoolAllocator<T>&) = default;

  /**
   * Returns a block of memory from the pool.
   *
   * @param n Number of bytes in the block.
   * @return A block of memory from the pool.
   */
  [[nodiscard]]
  constexpr T* allocate(size_t n) {
    return static_cast<T*>(m_memory_resource->allocate(n));
  }

  /**
   * Gives a block of memory back to the pool.
   *
   * @param p A pointer to the block of memory.
   * @param n Number of bytes in the block.
   */
  constexpr void deallocate(T* p, size_t n) {
    m_memory_resource->deallocate(p, n);
  }

 private:
  PoolResource* m_memory_resource;
};

/**
 * Returns a global pool memory resource.
 */
SLEIPNIR_DLLEXPORT PoolResource& global_pool_resource();

/**
 * Returns an allocator for a global pool memory resource.
 *
 * @tparam T The type of object in the pool.
 */
template <typename T>
PoolAllocator<T> global_pool_allocator() {
  return PoolAllocator<T>{&global_pool_resource()};
}

}  // namespace slp
