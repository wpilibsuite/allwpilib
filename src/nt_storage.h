/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_STORAGE_H_
#define NT_STORAGE_H_

#include "ntcore.h"

#include "llvm/StringMap.h"

namespace NtImpl {

class StorageEntry
{
public:
    StorageEntry()
    {
        NT_InitValue(&value);
        flags = 0;
    }
    ~StorageEntry()
    {
        NT_DisposeValue(&value);
    }
    NT_Value value;
    unsigned int flags;

private:
    StorageEntry(const StorageEntry&);
    StorageEntry& operator= (const StorageEntry&);
};

class Storage
{
public:
    static Storage &GetInstance()
    {
        if (!m_instance)
            m_instance = new Storage;
        return *m_instance;
    }

    llvm::StringMap<StorageEntry> entries;

private:
    Storage();
    ~Storage();
    Storage(const Storage&);
    Storage& operator= (const Storage&);

    static Storage *m_instance;
};

} // namespace NtImpl

#endif /* NT_STORAGE_H_ */
