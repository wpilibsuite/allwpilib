//===- StringMap.h - String Hash table map interface ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the StringMap class.
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_STRINGMAP_H
#define WPIUTIL_WPI_STRINGMAP_H

#include "wpi/SmallVector.h"
#include "wpi/StringRef.h"
#include "wpi/iterator.h"
#include "wpi/iterator_range.h"
#include "wpi/MemAlloc.h"
#include "wpi/PointerLikeTypeTraits.h"
#include "wpi/ErrorHandling.h"
#include "wpi/deprecated.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <utility>

namespace wpi {

template<typename ValueTy> class StringMapConstIterator;
template<typename ValueTy> class StringMapIterator;
template<typename ValueTy> class StringMapKeyIterator;
template<typename ValueTy> class StringMapEntry;

/// StringMapEntryBase - Shared base class of StringMapEntry instances.
class StringMapEntryBase {
  size_t StrLen;

public:
  explicit StringMapEntryBase(size_t Len) : StrLen(Len) {}

  size_t getKeyLength() const { return StrLen; }
};

/// StringMapImpl - This is the base class of StringMap that is shared among
/// all of its instantiations.
class StringMapImpl {
protected:
  // Array of NumBuckets pointers to entries, null pointers are holes.
  // TheTable[NumBuckets] contains a sentinel value for easy iteration. Followed
  // by an array of the actual hash values as unsigned integers.
  StringMapEntryBase **TheTable = nullptr;
  unsigned NumBuckets = 0;
  unsigned NumItems = 0;
  unsigned NumTombstones = 0;
  unsigned ItemSize;

protected:
  explicit StringMapImpl(unsigned itemSize)
      : ItemSize(itemSize) {}
  StringMapImpl(StringMapImpl &&RHS) noexcept
      : TheTable(RHS.TheTable), NumBuckets(RHS.NumBuckets),
        NumItems(RHS.NumItems), NumTombstones(RHS.NumTombstones),
        ItemSize(RHS.ItemSize) {
    RHS.TheTable = nullptr;
    RHS.NumBuckets = 0;
    RHS.NumItems = 0;
    RHS.NumTombstones = 0;
  }

  StringMapImpl(unsigned InitSize, unsigned ItemSize);
  unsigned RehashTable(unsigned BucketNo = 0);

  /// LookupBucketFor - Look up the bucket that the specified string should end
  /// up in.  If it already exists as a key in the map, the Item pointer for the
  /// specified bucket will be non-null.  Otherwise, it will be null.  In either
  /// case, the FullHashValue field of the bucket will be set to the hash value
  /// of the string.
  unsigned LookupBucketFor(StringRef Key);

  /// FindKey - Look up the bucket that contains the specified key. If it exists
  /// in the map, return the bucket number of the key.  Otherwise return -1.
  /// This does not modify the map.
  int FindKey(StringRef Key) const;

  /// RemoveKey - Remove the specified StringMapEntry from the table, but do not
  /// delete it.  This aborts if the value isn't in the table.
  void RemoveKey(StringMapEntryBase *V);

  /// RemoveKey - Remove the StringMapEntry for the specified key from the
  /// table, returning it.  If the key is not in the table, this returns null.
  StringMapEntryBase *RemoveKey(StringRef Key);

  /// Allocate the table with the specified number of buckets and otherwise
  /// setup the map as empty.
  void init(unsigned Size);

public:
  static StringMapEntryBase *getTombstoneVal() {
    uintptr_t Val = static_cast<uintptr_t>(-1);
    Val <<= PointerLikeTypeTraits<StringMapEntryBase *>::NumLowBitsAvailable;
    return reinterpret_cast<StringMapEntryBase *>(Val);
  }

  unsigned getNumBuckets() const { return NumBuckets; }
  unsigned getNumItems() const { return NumItems; }

  bool empty() const { return NumItems == 0; }
  unsigned size() const { return NumItems; }

  void swap(StringMapImpl &Other) {
    std::swap(TheTable, Other.TheTable);
    std::swap(NumBuckets, Other.NumBuckets);
    std::swap(NumItems, Other.NumItems);
    std::swap(NumTombstones, Other.NumTombstones);
  }
};

/// StringMapEntry - This is used to represent one value that is inserted into
/// a StringMap.  It contains the Value itself and the key: the string length
/// and data.
template<typename ValueTy>
class StringMapEntry : public StringMapEntryBase {
public:
  ValueTy second;

  explicit StringMapEntry(size_t strLen)
    : StringMapEntryBase(strLen), second() {}
  template <typename... InitTy>
  StringMapEntry(size_t strLen, InitTy &&... InitVals)
      : StringMapEntryBase(strLen), second(std::forward<InitTy>(InitVals)...) {}
  StringMapEntry(StringMapEntry &E) = delete;

  StringRef getKey() const {
    return StringRef(getKeyData(), getKeyLength());
  }

  const ValueTy &getValue() const { return second; }
  ValueTy &getValue() { return second; }

  void setValue(const ValueTy &V) { second = V; }

  /// getKeyData - Return the start of the string data that is the key for this
  /// value.  The string data is always stored immediately after the
  /// StringMapEntry object.
  const char *getKeyData() const {return reinterpret_cast<const char*>(this+1);}

  StringRef first() const { return StringRef(getKeyData(), getKeyLength()); }

  /// Create a StringMapEntry for the specified key construct the value using
  /// \p InitiVals.
  template <typename... InitTy>
  static StringMapEntry *Create(StringRef Key, InitTy &&... InitVals) {
    size_t KeyLength = Key.size();

    // Allocate a new item with space for the string at the end and a null
    // terminator.
    size_t AllocSize = sizeof(StringMapEntry) + KeyLength + 1;

    StringMapEntry *NewItem =
      static_cast<StringMapEntry*>(safe_malloc(AllocSize));

    // Construct the value.
    new (NewItem) StringMapEntry(KeyLength, std::forward<InitTy>(InitVals)...);

    // Copy the string information.
    char *StrBuffer = const_cast<char*>(NewItem->getKeyData());
    if (KeyLength > 0)
      memcpy(StrBuffer, Key.data(), KeyLength);
    StrBuffer[KeyLength] = 0;  // Null terminate for convenience of clients.
    return NewItem;
  }

  static StringMapEntry *Create(StringRef Key) {
    return Create(Key, ValueTy());
  }

  /// GetStringMapEntryFromKeyData - Given key data that is known to be embedded
  /// into a StringMapEntry, return the StringMapEntry itself.
  static StringMapEntry &GetStringMapEntryFromKeyData(const char *KeyData) {
    char *Ptr = const_cast<char*>(KeyData) - sizeof(StringMapEntry<ValueTy>);
    return *reinterpret_cast<StringMapEntry*>(Ptr);
  }

  /// Destroy - Destroy this StringMapEntry, releasing memory back to the
  /// specified allocator.
  void Destroy() {
    // Free memory referenced by the item.
    this->~StringMapEntry();
    std::free(static_cast<void *>(this));
  }
};


/// StringMap - This is an unconventional map that is specialized for handling
/// keys that are "strings", which are basically ranges of bytes. This does some
/// funky memory allocation and hashing things to make it extremely efficient,
/// storing the string data *after* the value in the map.
template<typename ValueTy>
class StringMap : public StringMapImpl {
public:
  using MapEntryTy = StringMapEntry<ValueTy>;

  StringMap() : StringMapImpl(static_cast<unsigned>(sizeof(MapEntryTy))) {}

  explicit StringMap(unsigned InitialSize)
    : StringMapImpl(InitialSize, static_cast<unsigned>(sizeof(MapEntryTy))) {}

  StringMap(std::initializer_list<std::pair<StringRef, ValueTy>> List)
      : StringMapImpl(List.size(), static_cast<unsigned>(sizeof(MapEntryTy))) {
    for (const auto &P : List) {
      insert(P);
    }
  }

  StringMap(StringMap &&RHS)
      : StringMapImpl(std::move(RHS)) {}

  StringMap(const StringMap &RHS) :
    StringMapImpl(static_cast<unsigned>(sizeof(MapEntryTy))) {
    if (RHS.empty())
      return;

    // Allocate TheTable of the same size as RHS's TheTable, and set the
    // sentinel appropriately (and NumBuckets).
    init(RHS.NumBuckets);
    unsigned *HashTable = (unsigned *)(TheTable + NumBuckets + 1),
             *RHSHashTable = (unsigned *)(RHS.TheTable + NumBuckets + 1);

    NumItems = RHS.NumItems;
    NumTombstones = RHS.NumTombstones;
    for (unsigned I = 0, E = NumBuckets; I != E; ++I) {
      StringMapEntryBase *Bucket = RHS.TheTable[I];
      if (!Bucket || Bucket == getTombstoneVal()) {
        TheTable[I] = Bucket;
        continue;
      }

      TheTable[I] = MapEntryTy::Create(
          static_cast<MapEntryTy *>(Bucket)->getKey(),
          static_cast<MapEntryTy *>(Bucket)->getValue());
      HashTable[I] = RHSHashTable[I];
    }

    // Note that here we've copied everything from the RHS into this object,
    // tombstones included. We could, instead, have re-probed for each key to
    // instantiate this new object without any tombstone buckets. The
    // assumption here is that items are rarely deleted from most StringMaps,
    // and so tombstones are rare, so the cost of re-probing for all inputs is
    // not worthwhile.
  }

  StringMap &operator=(StringMap RHS) {
    StringMapImpl::swap(RHS);
    return *this;
  }

  ~StringMap() {
    // Delete all the elements in the map, but don't reset the elements
    // to default values.  This is a copy of clear(), but avoids unnecessary
    // work not required in the destructor.
    if (!empty()) {
      for (unsigned I = 0, E = NumBuckets; I != E; ++I) {
        StringMapEntryBase *Bucket = TheTable[I];
        if (Bucket && Bucket != getTombstoneVal()) {
          static_cast<MapEntryTy*>(Bucket)->Destroy();
        }
      }
    }
    free(TheTable);
  }

  using key_type = const char*;
  using mapped_type = ValueTy;
  using value_type = StringMapEntry<ValueTy>;
  using size_type = size_t;

  using const_iterator = StringMapConstIterator<ValueTy>;
  using iterator = StringMapIterator<ValueTy>;

  iterator begin() {
    return iterator(TheTable, NumBuckets == 0);
  }
  iterator end() {
    return iterator(TheTable+NumBuckets, true);
  }
  const_iterator begin() const {
    return const_iterator(TheTable, NumBuckets == 0);
  }
  const_iterator end() const {
    return const_iterator(TheTable+NumBuckets, true);
  }

  iterator_range<StringMapKeyIterator<ValueTy>> keys() const {
    return make_range(StringMapKeyIterator<ValueTy>(begin()),
                      StringMapKeyIterator<ValueTy>(end()));
  }

  iterator find(StringRef Key) {
    int Bucket = FindKey(Key);
    if (Bucket == -1) return end();
    return iterator(TheTable+Bucket, true);
  }

  const_iterator find(StringRef Key) const {
    int Bucket = FindKey(Key);
    if (Bucket == -1) return end();
    return const_iterator(TheTable+Bucket, true);
  }

  /// lookup - Return the entry for the specified key, or a default
  /// constructed value if no such entry exists.
  ValueTy lookup(StringRef Key) const {
    const_iterator it = find(Key);
    if (it != end())
      return it->second;
    return ValueTy();
  }

  /// Lookup the ValueTy for the \p Key, or create a default constructed value
  /// if the key is not in the map.
  ValueTy &operator[](StringRef Key) { return try_emplace(Key).first->second; }

  /// count - Return 1 if the element is in the map, 0 otherwise.
  size_type count(StringRef Key) const {
    return find(Key) == end() ? 0 : 1;
  }

  /// insert - Insert the specified key/value pair into the map.  If the key
  /// already exists in the map, return false and ignore the request, otherwise
  /// insert it and return true.
  bool insert(MapEntryTy *KeyValue) {
    unsigned BucketNo = LookupBucketFor(KeyValue->getKey());
    StringMapEntryBase *&Bucket = TheTable[BucketNo];
    if (Bucket && Bucket != getTombstoneVal())
      return false;  // Already exists in map.

    if (Bucket == getTombstoneVal())
      --NumTombstones;
    Bucket = KeyValue;
    ++NumItems;
    assert(NumItems + NumTombstones <= NumBuckets);

    RehashTable();
    return true;
  }

  /// insert - Inserts the specified key/value pair into the map if the key
  /// isn't already in the map. The bool component of the returned pair is true
  /// if and only if the insertion takes place, and the iterator component of
  /// the pair points to the element with key equivalent to the key of the pair.
  std::pair<iterator, bool> insert(std::pair<StringRef, ValueTy> KV) {
    return try_emplace(KV.first, std::move(KV.second));
  }

  /// Emplace a new element for the specified key into the map if the key isn't
  /// already in the map. The bool component of the returned pair is true
  /// if and only if the insertion takes place, and the iterator component of
  /// the pair points to the element with key equivalent to the key of the pair.
  template <typename... ArgsTy>
  std::pair<iterator, bool> try_emplace(StringRef Key, ArgsTy &&... Args) {
    unsigned BucketNo = LookupBucketFor(Key);
    StringMapEntryBase *&Bucket = TheTable[BucketNo];
    if (Bucket && Bucket != getTombstoneVal())
      return std::make_pair(iterator(TheTable + BucketNo, false),
                            false); // Already exists in map.

    if (Bucket == getTombstoneVal())
      --NumTombstones;
    Bucket = MapEntryTy::Create(Key, std::forward<ArgsTy>(Args)...);
    ++NumItems;
    assert(NumItems + NumTombstones <= NumBuckets);

    BucketNo = RehashTable(BucketNo);
    return std::make_pair(iterator(TheTable + BucketNo, false), true);
  }

  // clear - Empties out the StringMap
  void clear() {
    if (empty()) return;

    // Zap all values, resetting the keys back to non-present (not tombstone),
    // which is safe because we're removing all elements.
    for (unsigned I = 0, E = NumBuckets; I != E; ++I) {
      StringMapEntryBase *&Bucket = TheTable[I];
      if (Bucket && Bucket != getTombstoneVal()) {
        static_cast<MapEntryTy*>(Bucket)->Destroy();
      }
      Bucket = nullptr;
    }

    NumItems = 0;
    NumTombstones = 0;
  }

  /// remove - Remove the specified key/value pair from the map, but do not
  /// erase it.  This aborts if the key is not in the map.
  void remove(MapEntryTy *KeyValue) {
    RemoveKey(KeyValue);
  }

  void erase(iterator I) {
    MapEntryTy &V = *I;
    remove(&V);
    V.Destroy();
  }

  bool erase(StringRef Key) {
    iterator I = find(Key);
    if (I == end()) return false;
    erase(I);
    return true;
  }
};

template <typename DerivedTy, typename ValueTy>
class StringMapIterBase
    : public iterator_facade_base<DerivedTy, std::forward_iterator_tag,
                                  ValueTy> {
protected:
  StringMapEntryBase **Ptr = nullptr;

public:
  StringMapIterBase() = default;

  explicit StringMapIterBase(StringMapEntryBase **Bucket,
                             bool NoAdvance = false)
      : Ptr(Bucket) {
    if (!NoAdvance) AdvancePastEmptyBuckets();
  }

  DerivedTy &operator=(const DerivedTy &Other) {
    Ptr = Other.Ptr;
    return static_cast<DerivedTy &>(*this);
  }

  bool operator==(const DerivedTy &RHS) const { return Ptr == RHS.Ptr; }

  DerivedTy &operator++() { // Preincrement
    ++Ptr;
    AdvancePastEmptyBuckets();
    return static_cast<DerivedTy &>(*this);
  }

  DerivedTy operator++(int) { // Post-increment
    DerivedTy Tmp(Ptr);
    ++*this;
    return Tmp;
  }

  DerivedTy &operator--() { // Predecrement
    --Ptr;
    ReversePastEmptyBuckets();
    return static_cast<DerivedTy &>(*this);
  }

  DerivedTy operator--(int) { // Post-decrement
    DerivedTy Tmp(Ptr);
    --*this;
    return Tmp;
  }

private:
  void AdvancePastEmptyBuckets() {
    while (*Ptr == nullptr || *Ptr == StringMapImpl::getTombstoneVal())
      ++Ptr;
  }
  void ReversePastEmptyBuckets() {
    while (*Ptr == nullptr || *Ptr == StringMapImpl::getTombstoneVal())
      --Ptr;
  }
};

template <typename ValueTy>
class StringMapConstIterator
    : public StringMapIterBase<StringMapConstIterator<ValueTy>,
                               const StringMapEntry<ValueTy>> {
  using base = StringMapIterBase<StringMapConstIterator<ValueTy>,
                                 const StringMapEntry<ValueTy>>;

public:
  using value_type = const StringMapEntry<ValueTy>;

  StringMapConstIterator() = default;
  explicit StringMapConstIterator(StringMapEntryBase **Bucket,
                                  bool NoAdvance = false)
      : base(Bucket, NoAdvance) {}

  value_type &operator*() const {
    return *static_cast<value_type *>(*this->Ptr);
  }
};

template <typename ValueTy>
class StringMapIterator : public StringMapIterBase<StringMapIterator<ValueTy>,
                                                   StringMapEntry<ValueTy>> {
  using base =
      StringMapIterBase<StringMapIterator<ValueTy>, StringMapEntry<ValueTy>>;

public:
  using value_type = StringMapEntry<ValueTy>;

  StringMapIterator() = default;
  explicit StringMapIterator(StringMapEntryBase **Bucket,
                             bool NoAdvance = false)
      : base(Bucket, NoAdvance) {}

  value_type &operator*() const {
    return *static_cast<value_type *>(*this->Ptr);
  }

  operator StringMapConstIterator<ValueTy>() const {
    return StringMapConstIterator<ValueTy>(this->Ptr, true);
  }
};

template <typename ValueTy>
class StringMapKeyIterator
    : public iterator_adaptor_base<StringMapKeyIterator<ValueTy>,
                                   StringMapConstIterator<ValueTy>,
                                   std::forward_iterator_tag, StringRef> {
  using base = iterator_adaptor_base<StringMapKeyIterator<ValueTy>,
                                     StringMapConstIterator<ValueTy>,
                                     std::forward_iterator_tag, StringRef>;

public:
  StringMapKeyIterator() = default;
  explicit StringMapKeyIterator(StringMapConstIterator<ValueTy> Iter)
      : base(std::move(Iter)) {}

  StringRef &operator*() {
    Key = this->wrapped()->getKey();
    return Key;
  }

private:
  StringRef Key;
};

template <typename ValueTy>
bool operator==(const StringMap<ValueTy>& lhs, const StringMap<ValueTy>& rhs) {
  // same instance?
  if (&lhs == &rhs) return true;

  // first check that sizes are identical
  if (lhs.size() != rhs.size()) return false;

  // copy into vectors and sort by key
  SmallVector<StringMapConstIterator<ValueTy>, 16> lhs_items;
  lhs_items.reserve(lhs.size());
  for (auto i = lhs.begin(), end = lhs.end(); i != end; ++i)
    lhs_items.push_back(i);
  std::sort(lhs_items.begin(), lhs_items.end(),
            [](const StringMapConstIterator<ValueTy>& a,
               const StringMapConstIterator<ValueTy>& b) {
              return a->getKey() < b->getKey();
            });

  SmallVector<StringMapConstIterator<ValueTy>, 16> rhs_items;
  rhs_items.reserve(rhs.size());
  for (auto i = rhs.begin(), end = rhs.end(); i != end; ++i)
    rhs_items.push_back(i);
  std::sort(rhs_items.begin(), rhs_items.end(),
            [](const StringMapConstIterator<ValueTy>& a,
               const StringMapConstIterator<ValueTy>& b) {
              return a->getKey() < b->getKey();
            });

  // compare vector keys and values
  for (auto a = lhs_items.begin(), b = rhs_items.begin(),
            aend = lhs_items.end(), bend = rhs_items.end();
       a != aend && b != bend; ++a, ++b) {
    if ((*a)->first() != (*b)->first() || (*a)->second != (*b)->second)
      return false;
  }
  return true;
}

template <typename ValueTy>
inline bool operator!=(const StringMap<ValueTy>& lhs,
                       const StringMap<ValueTy>& rhs) {
  return !(lhs == rhs);
}

template <typename ValueTy>
bool operator<(const StringMap<ValueTy>& lhs, const StringMap<ValueTy>& rhs) {
  // same instance?
  if (&lhs == &rhs) return false;

  // copy into vectors and sort by key
  SmallVector<StringRef, 16> lhs_keys;
  lhs_keys.reserve(lhs.size());
  for (auto i = lhs.begin(), end = lhs.end(); i != end; ++i)
    lhs_keys.push_back(i->getKey());
  std::sort(lhs_keys.begin(), lhs_keys.end());

  SmallVector<StringRef, 16> rhs_keys;
  rhs_keys.reserve(rhs.size());
  for (auto i = rhs.begin(), end = rhs.end(); i != end; ++i)
    rhs_keys.push_back(i->getKey());
  std::sort(rhs_keys.begin(), rhs_keys.end());

  // use std::vector comparison
  return lhs_keys < rhs_keys;
}

template <typename ValueTy>
inline bool operator<=(const StringMap<ValueTy>& lhs,
                       const StringMap<ValueTy>& rhs) {
  return !(rhs < lhs);
}

template <typename ValueTy>
inline bool operator>(const StringMap<ValueTy>& lhs,
                      const StringMap<ValueTy>& rhs) {
  return !(lhs <= rhs);
}

template <typename ValueTy>
inline bool operator>=(const StringMap<ValueTy>& lhs,
                       const StringMap<ValueTy>& rhs) {
  return !(lhs < rhs);
}

} // end namespace wpi

#endif // LLVM_ADT_STRINGMAP_H
