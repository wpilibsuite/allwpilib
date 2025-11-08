// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// This is a modified version of readerwriterqueue to remove atomics and barriers
// for single-thread operation.
//
// Copyright (c) 2013-2021, Cameron Desrochers
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, this list of
// conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, this list of
// conditions and the following disclaimer in the documentation and/or other materials
// provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdlib>		// For malloc/free/abort & size_t
#include <new>
#include <type_traits>
#include <utility>

// WPI_FQ_FORCEINLINE
#if defined(_MSC_VER)
#define WPI_FQ_FORCEINLINE __forceinline
#elif defined(__GNUC__)
//#define WPI_FQ_FORCEINLINE __attribute__((always_inline))
#define WPI_FQ_FORCEINLINE inline
#else
#define WPI_FQ_FORCEINLINE inline
#endif

// A queue for a single-consumer, single-producer architecture.
// The queue is also wait-free in the common path (except if more memory
// needs to be allocated, in which case malloc is called).
// Allocates memory sparingly, and only once if the original maximum size
// estimate is never exceeded.

namespace wpi {

template<typename T, size_t MAX_BLOCK_SIZE = 512>
class FastQueue
{
	// Design: Based on a queue-of-queues. The low-level queues are just
	// circular buffers with front and tail indices indicating where the
	// next element to dequeue is and where the next element can be enqueued,
	// respectively. Each low-level queue is called a "block". Each block
	// wastes exactly one element's worth of space to keep the design simple
	// (if front == tail then the queue is empty, and can't be full).
	// The high-level queue is a circular linked list of blocks; again there
	// is a front and tail, but this time they are pointers to the blocks.
	// The front block is where the next element to be dequeued is, provided
	// the block is not empty. The back block is where elements are to be
	// enqueued, provided the block is not full.
	// The producer owns all the tail indices/pointers. The consumer
	// owns all the front indices/pointers. Both read each
	// other's variables, but only the owning side updates them. E.g. After
	// the consumer reads the producer's tail, the tail may change before the
	// consumer is done dequeuing an object, but the consumer knows the tail
	// will never go backwards, only forwards.
	// If there is no room to enqueue an object, an additional block (of
	// equal size to the last block) is added. Blocks are never removed.

public:
	typedef T value_type;

	// Constructs a queue that can hold at least `size` elements without further
	// allocations. If more than MAX_BLOCK_SIZE elements are requested,
	// then several blocks of MAX_BLOCK_SIZE each are reserved (including
	// at least one extra buffer block).
	explicit FastQueue(size_t size = 15)
	{
		static_assert(MAX_BLOCK_SIZE == ceilToPow2(MAX_BLOCK_SIZE) && "MAX_BLOCK_SIZE must be a power of 2");
		static_assert(MAX_BLOCK_SIZE >= 2 && "MAX_BLOCK_SIZE must be at least 2");

		Block* firstBlock = nullptr;

		largestBlockSize = ceilToPow2(size + 1);		// We need a spare slot to fit size elements in the block
		if (largestBlockSize > MAX_BLOCK_SIZE * 2) {
			// We need a spare block in case the producer is writing to a different block the consumer is reading from, and
			// wants to enqueue the maximum number of elements. We also need a spare element in each block to avoid the ambiguity
			// between front == tail meaning "empty" and "full".
			// So the effective number of slots that are guaranteed to be usable at any time is the block size - 1 times the
			// number of blocks - 1. Solving for size and applying a ceiling to the division gives us (after simplifying):
			size_t initialBlockCount = (size + MAX_BLOCK_SIZE * 2 - 3) / (MAX_BLOCK_SIZE - 1);
			largestBlockSize = MAX_BLOCK_SIZE;
			Block* lastBlock = nullptr;
			for (size_t i = 0; i != initialBlockCount; ++i) {
				auto block = make_block(largestBlockSize);
				if (block == nullptr) {
					throw std::bad_alloc();
				}
				if (firstBlock == nullptr) {
					firstBlock = block;
				}
				else {
					lastBlock->next = block;
				}
				lastBlock = block;
				block->next = firstBlock;
			}
		}
		else {
			firstBlock = make_block(largestBlockSize);
			if (firstBlock == nullptr) {
				throw std::bad_alloc();
			}
			firstBlock->next = firstBlock;
		}
		frontBlock = firstBlock;
		tailBlock = firstBlock;
	}

	FastQueue(FastQueue&& other)
		: frontBlock(other.frontBlock),
		tailBlock(other.tailBlock),
		largestBlockSize(other.largestBlockSize)
	{
		other.largestBlockSize = 32;
		Block* b = other.make_block(other.largestBlockSize);
		if (b == nullptr) {
			throw std::bad_alloc();
		}
		b->next = b;
		other.frontBlock = b;
		other.tailBlock = b;
	}

	FastQueue& operator=(FastQueue&& other)
	{
		Block* b = frontBlock;
		frontBlock = other.frontBlock;
		other.frontBlock = b;
		b = tailBlock;
		tailBlock = other.tailBlock;
		other.tailBlock = b;
		std::swap(largestBlockSize, other.largestBlockSize);
		return *this;
	}

	~FastQueue()
	{
		// Destroy any remaining objects in queue and free memory
		Block* frontBlock_ = frontBlock;
		Block* block = frontBlock_;
		do {
			Block* nextBlock = block->next;
			size_t blockFront = block->front;
			size_t blockTail = block->tail;

			for (size_t i = blockFront; i != blockTail; i = (i + 1) & block->sizeMask) {
				auto element = reinterpret_cast<T*>(block->data + i * sizeof(T));
				element->~T();
				(void)element;
			}

			block->~Block();
			std::free(block);
			block = nextBlock;
		} while (block != frontBlock_);
	}


	// Enqueues a copy of element if there is room in the queue.
	// Returns true if the element was enqueued, false otherwise.
	// Does not allocate memory.
	WPI_FQ_FORCEINLINE bool try_enqueue(T const& element)
	{
		return inner_enqueue<CannotAlloc>(element);
	}

	// Enqueues a moved copy of element if there is room in the queue.
	// Returns true if the element was enqueued, false otherwise.
	// Does not allocate memory.
	WPI_FQ_FORCEINLINE bool try_enqueue(T&& element)
	{
		return inner_enqueue<CannotAlloc>(std::forward<T>(element));
	}

	// Like try_enqueue() but with emplace semantics (i.e. construct-in-place).
	template<typename... Args>
	WPI_FQ_FORCEINLINE bool try_emplace(Args&&... args)
	{
		return inner_enqueue<CannotAlloc>(std::forward<Args>(args)...);
	}

	// Enqueues a copy of element on the queue.
	// Allocates an additional block of memory if needed.
	// Only fails (returns false) if memory allocation fails.
	WPI_FQ_FORCEINLINE bool enqueue(T const& element)
	{
		return inner_enqueue<CanAlloc>(element);
	}

	// Enqueues a moved copy of element on the queue.
	// Allocates an additional block of memory if needed.
	// Only fails (returns false) if memory allocation fails.
	WPI_FQ_FORCEINLINE bool enqueue(T&& element)
	{
		return inner_enqueue<CanAlloc>(std::forward<T>(element));
	}

	// Like enqueue() but with emplace semantics (i.e. construct-in-place).
	template<typename... Args>
	WPI_FQ_FORCEINLINE bool emplace(Args&&... args)
	{
		return inner_enqueue<CanAlloc>(std::forward<Args>(args)...);
	}

	// Attempts to dequeue an element; if the queue is empty,
	// returns false instead. If the queue has at least one element,
	// moves front to result using operator=, then returns true.
	template<typename U>
	bool try_dequeue(U& result)
	{
		// High-level pseudocode:
		// Remember where the tail block is
		// If the front block has an element in it, dequeue it
		// Else
		//     If front block was the tail block when we entered the function, return false
		//     Else advance to next block and dequeue the item there

		Block* frontBlock_ = frontBlock;
		size_t blockTail = frontBlock_->localTail;
		size_t blockFront = frontBlock_->front;

		if (blockFront != blockTail || blockFront != (frontBlock_->localTail = frontBlock_->tail)) {
			// Front block not empty, dequeue from here
			auto element = reinterpret_cast<T*>(frontBlock_->data + blockFront * sizeof(T));
			result = std::move(*element);
			element->~T();

			blockFront = (blockFront + 1) & frontBlock_->sizeMask;

			frontBlock_->front = blockFront;
		}
		else if (frontBlock_ != tailBlock) {
			frontBlock_ = frontBlock;
			blockTail = frontBlock_->localTail = frontBlock_->tail;
			blockFront = frontBlock_->front;

			// Front block is empty but there's another block ahead, advance to it
			Block* nextBlock = frontBlock_->next;
			size_t nextBlockFront = nextBlock->front;
			size_t nextBlockTail = nextBlock->localTail = nextBlock->tail;

			// Since the tailBlock is only ever advanced after being written to,
			// we know there's for sure an element to dequeue on it
			assert(nextBlockFront != nextBlockTail);
			(void) nextBlockTail;

			// We're done with this block, let the producer use it if it needs
			frontBlock = frontBlock_ = nextBlock;

			auto element = reinterpret_cast<T*>(frontBlock_->data + nextBlockFront * sizeof(T));

			result = std::move(*element);
			element->~T();

			nextBlockFront = (nextBlockFront + 1) & frontBlock_->sizeMask;

			frontBlock_->front = nextBlockFront;
		}
		else {
			// No elements in current block and no other block to advance to
			return false;
		}

		return true;
	}


	// Returns a pointer to the front element in the queue (the one that
	// would be removed next by a call to `try_dequeue` or `pop`). If the
	// queue appears empty at the time the method is called, nullptr is
	// returned instead.
	T* peek() const
	{
		// See try_dequeue() for reasoning

		Block* frontBlock_ = frontBlock;
		size_t blockTail = frontBlock_->localTail;
		size_t blockFront = frontBlock_->front;

		if (blockFront != blockTail || blockFront != (frontBlock_->localTail = frontBlock_->tail)) {
			return reinterpret_cast<T*>(frontBlock_->data + blockFront * sizeof(T));
		}
		else if (frontBlock_ != tailBlock) {
			frontBlock_ = frontBlock;
			blockTail = frontBlock_->localTail = frontBlock_->tail;
			blockFront = frontBlock_->front;

			Block* nextBlock = frontBlock_->next;

			size_t nextBlockFront = nextBlock->front;

			assert(nextBlockFront != nextBlock->tail);
			return reinterpret_cast<T*>(nextBlock->data + nextBlockFront * sizeof(T));
		}

		return nullptr;
	}

	// Removes the front element from the queue, if any, without returning it.
	// Returns true on success, or false if the queue appeared empty at the time
	// `pop` was called.
	bool pop()
	{
		// See try_dequeue() for reasoning

		Block* frontBlock_ = frontBlock;
		size_t blockTail = frontBlock_->localTail;
		size_t blockFront = frontBlock_->front;

		if (blockFront != blockTail || blockFront != (frontBlock_->localTail = frontBlock_->tail)) {
			auto element = reinterpret_cast<T*>(frontBlock_->data + blockFront * sizeof(T));
			element->~T();

			blockFront = (blockFront + 1) & frontBlock_->sizeMask;

			frontBlock_->front = blockFront;
		}
		else if (frontBlock_ != tailBlock) {
			frontBlock_ = frontBlock;
			blockTail = frontBlock_->localTail = frontBlock_->tail;
			blockFront = frontBlock_->front;

			// Front block is empty but there's another block ahead, advance to it
			Block* nextBlock = frontBlock_->next;

			size_t nextBlockFront = nextBlock->front;
			size_t nextBlockTail = nextBlock->localTail = nextBlock->tail;

			assert(nextBlockFront != nextBlockTail);
			(void) nextBlockTail;

			frontBlock = frontBlock_ = nextBlock;

			auto element = reinterpret_cast<T*>(frontBlock_->data + nextBlockFront * sizeof(T));
			element->~T();

			nextBlockFront = (nextBlockFront + 1) & frontBlock_->sizeMask;

			frontBlock_->front = nextBlockFront;
		}
		else {
			// No elements in current block and no other block to advance to
			return false;
		}

		return true;
	}

	// Returns if the queue is empty
	inline bool empty() const {
		return size() == 0;
	}

	// Returns the number of items currently in the queue.
	inline size_t size() const
	{
		size_t result = 0;
		Block* frontBlock_ = frontBlock;
		Block* block = frontBlock_;
		do {
			size_t blockFront = block->front;
			size_t blockTail = block->tail;
			result += (blockTail - blockFront) & block->sizeMask;
			block = block->next;
		} while (block != frontBlock_);
		return result;
	}

	// Returns the total number of items that could be enqueued without incurring
	// an allocation when this queue is empty.
	inline size_t max_capacity() const {
		size_t result = 0;
		Block* frontBlock_ = frontBlock;
		Block* block = frontBlock_;
		do {
			result += block->sizeMask;
			block = block->next;
		} while (block != frontBlock_);
		return result;
	}


private:
	enum AllocationMode { CanAlloc, CannotAlloc };

	template<AllocationMode canAlloc, typename... Args>
	bool inner_enqueue(Args&&... args)
	{
		// High-level pseudocode (assuming we're allowed to alloc a new block):
		// If room in tail block, add to tail
		// Else check next block
		//     If next block is not the head block, enqueue on next block
		//     Else create a new block and enqueue there
		//     Advance tail to the block we just enqueued to

		Block* tailBlock_ = tailBlock;
		size_t blockFront = tailBlock_->localFront;
		size_t blockTail = tailBlock_->tail;

		size_t nextBlockTail = (blockTail + 1) & tailBlock_->sizeMask;
		if (nextBlockTail != blockFront || nextBlockTail != (tailBlock_->localFront = tailBlock_->front)) {
			// This block has room for at least one more element
			char* location = tailBlock_->data + blockTail * sizeof(T);
			new (location) T(std::forward<Args>(args)...);

			tailBlock_->tail = nextBlockTail;
		}
		else {
			if (tailBlock_->next != frontBlock) {
				// Note that the reason we can't advance to the frontBlock and start adding new entries there
				// is because if we did, then dequeue would stay in that block, eventually reading the new values,
				// instead of advancing to the next full block (whose values were enqueued first and so should be
				// consumed first).

				// tailBlock is full, but there's a free block ahead, use it
				Block* tailBlockNext = tailBlock_->next;
				size_t nextBlockFront = tailBlockNext->localFront = tailBlockNext->front;
				nextBlockTail = tailBlockNext->tail;

				// This block must be empty since it's not the head block and we
				// go through the blocks in a circle
				assert(nextBlockFront == nextBlockTail);
				tailBlockNext->localFront = nextBlockFront;

				char* location = tailBlockNext->data + nextBlockTail * sizeof(T);
				new (location) T(std::forward<Args>(args)...);

				tailBlockNext->tail = (nextBlockTail + 1) & tailBlockNext->sizeMask;

				tailBlock = tailBlockNext;
			}
			else if constexpr (canAlloc == CanAlloc) {
				// tailBlock is full and there's no free block ahead; create a new block
				auto newBlockSize = largestBlockSize >= MAX_BLOCK_SIZE ? largestBlockSize : largestBlockSize * 2;
				auto newBlock = make_block(newBlockSize);
				if (newBlock == nullptr) {
					// Could not allocate a block!
					return false;
				}
				largestBlockSize = newBlockSize;

				new (newBlock->data) T(std::forward<Args>(args)...);
				assert(newBlock->front == 0);
				newBlock->tail = newBlock->localTail = 1;

				newBlock->next = tailBlock_->next;
				tailBlock_->next = newBlock;

				tailBlock = newBlock;
			}
			else if constexpr (canAlloc == CannotAlloc) {
				// Would have had to allocate a new block to enqueue, but not allowed
				return false;
			}
			else {
				assert(false && "Should be unreachable code");
				return false;
			}
		}

		return true;
	}

	// Disable copying
	FastQueue(FastQueue const&) = delete;

	// Disable assignment
	FastQueue& operator=(FastQueue const&) = delete;

	static constexpr size_t ceilToPow2(size_t x)
	{
		// From http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		for (size_t i = 1; i < sizeof(size_t); i <<= 1) {
			x |= x >> (i << 3);
		}
		++x;
		return x;
	}

	template<typename U>
	static WPI_FQ_FORCEINLINE char* align_for(char* ptr)
	{
		const std::size_t alignment = std::alignment_of<U>::value;
		return ptr + (alignment - (reinterpret_cast<std::uintptr_t>(ptr) % alignment)) % alignment;
	}
private:

	struct Block
	{
		// Avoid false-sharing by putting highly contended variables on their own cache lines
		size_t front;	// Elements are read from here
		size_t localTail;			// An uncontended shadow copy of tail, owned by the consumer

		size_t tail;	// Elements are enqueued here
		size_t localFront;

		Block* next;

		char* data;		// Contents (on heap) are aligned to T's alignment

		const size_t sizeMask;

		// size must be a power of two (and greater than 0)
		Block(size_t _size, char* _data)
			: front(0UL), localTail(0), tail(0UL), localFront(0), next(nullptr), data(_data), sizeMask(_size - 1)
		{
		}
	};


	static Block* make_block(size_t capacity)
	{
		// Allocate enough memory for the block itself, as well as all the elements it will contain
		auto size = sizeof(Block);
		size += sizeof(T) * capacity + std::alignment_of<T>::value - 1;
		auto newBlock = static_cast<char*>(std::malloc(size));
		if (newBlock == nullptr) {
			return nullptr;
		}

		auto newBlockData = align_for<T>(newBlock + sizeof(Block));
		return new (newBlock) Block(capacity, newBlockData);
	}

private:
	Block* frontBlock;		// Elements are dequeued from this block
	Block* tailBlock;		// Elements are enqueued to this block
	size_t largestBlockSize;
};

}    // end namespace wpi
