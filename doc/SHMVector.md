# SHMVector

A minimalistic C++ Vector with Shared Memory Support.

## Overview

`SHMVector<T>` is a minimalistic vector implementation designed specifically for shared memory usage. It provides custom memory management where memory is allocated in configurable segments, and accepts externally allocated shared memory (e.g., from `mmap`).

## Key Features

1. **Configurable segment size**: Each element occupies a configurable number of bytes
2. **Shared memory compatible**: Uses externally provided memory (via `mmap`)
3. **Contiguous addressing**: Next segment address = previous address + segment_size_bytes
4. **No iterators**: Designed to be 100% compatible with shared memory (no types that rely on other memory regions)
5. **Simple interface**: Provides essential vector operations only

## Required Functions

1. ✅ **new vector** (providing segment size and shared memory)
2. ✅ **reserve vector** (elements count)
3. ✅ **push_back** element
4. ✅ **get element** at element index
5. ✅ **get elements count**

## Usage Example

```cpp
#include "SHMVector.hpp"
#include <sys/mman.h>

// Allocate shared memory using mmap
void* shmpointer = mmap(NULL, 640000, PROT_READ | PROT_WRITE, 
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);

// Create vector with:
// - segment_size: sizeof(int) bytes per element
// - shared_memory_ptr: pointer from mmap
// - shared_memory_size: total size of shared memory
SHMVector<int> vec(sizeof(int), shmpointer, 640000);

// Reserve capacity for 100 elements
vec.reserve(100);

// Add elements
vec.push_back(10);
vec.push_back(20);
vec.push_back(30);

// Get element at index
int value = vec.at(0);  // Returns 10

// Get element count
size_t count = vec.size();  // Returns 3

// When done, clean up the shared memory
munmap(shmpointer, 640000);
```

## Working with Struct Types

SHMVector works perfectly with struct types, making it ideal for storing structured data in shared memory:

```cpp
#include <cstring>

// Define your struct type
struct Payload_t {
    char Payload[4096];
    uint16_t PayloadLength;
};

// Allocate shared memory
void* shmem = mmap(NULL, 640000, PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);

// Create vector for struct type
SHMVector<Payload_t> vec(sizeof(Payload_t), shmem, 640000);

// Create and add struct instances
Payload_t payload1;
strcpy(payload1.Payload, "Payload char array");
payload1.PayloadLength = 18;
vec.push_back(payload1);

// Access struct members
std::cout << "Payload: " << vec.at(0).Payload << std::endl;
std::cout << "Length: " << vec.at(0).PayloadLength << std::endl;

// Clean up
munmap(shmem, 640000);
```

## Placing SHMVector Object in Shared Memory

When using placement new to place the SHMVector object itself inside shared memory, you must ensure the data storage region does not overlap with the SHMVector object's memory:

```cpp
#include <cstring>

struct Payload_t {
    char Payload[1024];
    uint16_t PayloadLength;
};

// Allocate shared memory
void* shmpointer = mmap(NULL, 640000, PROT_READ | PROT_WRITE, 
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);

// Calculate where data storage should start (after the SHMVector object)
size_t vector_obj_size = sizeof(SHMVector<Payload_t>);
size_t alignment = alignof(Payload_t);
size_t data_offset = ((vector_obj_size + alignment - 1) / alignment) * alignment;

// Data region starts after the SHMVector object
char* data_region = static_cast<char*>(shmpointer) + data_offset;
size_t data_region_size = 640000 - data_offset;

// Use placement new to construct SHMVector at start of shared memory
// But point it to use the data region for storage (not the same address)
SHMVector<Payload_t>* shmvector = new(shmpointer) SHMVector<Payload_t>(
    sizeof(Payload_t), data_region, data_region_size);

// Now use it normally
Payload_t payload1;
strcpy(payload1.Payload, "Payload char array");
payload1.PayloadLength = 18;
shmvector->push_back(payload1);

Payload_t payload2;
strcpy(payload2.Payload, "Second payload test");
payload2.PayloadLength = 19;
shmvector->push_back(payload2);

// Access elements
std::cout << "Payload: " << shmvector->at(0).Payload << std::endl;

// Manually call destructor since we used placement new
shmvector->~SHMVector();

// Clean up shared memory
munmap(shmpointer, 640000);
```

**Important**: When using placement new, the SHMVector object occupies the first bytes of the shared memory region. You must pass a different pointer (offset past the object) as the `shared_memory_ptr` parameter to avoid memory overlap.

## Custom Segment Sizes

You can use segment sizes larger than the type size for alignment or spacing:

```cpp
void* shmem = mmap(NULL, 640000, PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);

// Use 16 bytes per segment even though int is only 4 bytes
SHMVector<int> vec(16, shmem, 640000);

vec.push_back(100);
vec.push_back(200);

// Elements are stored 16 bytes apart:
// address[0] = base + 0*16
// address[1] = base + 1*16
// address[2] = base + 2*16
```

## Memory Layout

The memory layout is strictly contiguous based on segment size:

```
Base Address: memory_base_
Element 0:    memory_base_ + (0 * segment_size_bytes)
Element 1:    memory_base_ + (1 * segment_size_bytes)
Element 2:    memory_base_ + (2 * segment_size_bytes)
...
Element N:    memory_base_ + (N * segment_size_bytes)
```

## Important Notes

1. **Memory Ownership**: SHMVector does NOT own the memory. The caller must:
   - Allocate shared memory before creating the vector
   - Ensure the vector is destroyed before freeing the shared memory
   - Call `munmap()` to free the shared memory after the vector is destroyed

2. **Shared Memory Compatible**: The design uses:
   - No iterators
   - No STL containers within the class
   - Only POD-style data members (size_t and char*)
   - Simple pointer arithmetic for element access

3. **Type Compatibility**: 
   - **Best for cross-process shared memory**: Trivially copyable types (int, double, structs with POD members)
   - **Not recommended for cross-process**: Types with dynamic allocation (std::string, std::vector, etc.) as their internal data is heap-allocated outside the shared memory region
   - Works with any type for single-process usage, but for true cross-process shared memory, use only trivially copyable types

## API Reference

### Constructor
```cpp
SHMVector(size_t segment_size_bytes, void* shared_memory_ptr, size_t shared_memory_size)
```
Creates a vector with specified segment size using external shared memory.

### reserve()
```cpp
void reserve(size_t element_count)
```
Reserves capacity for the specified number of elements. Throws `std::bad_alloc` if requested capacity exceeds available shared memory.

### push_back()
```cpp
void push_back(const T& element)
```
Adds an element to the end of the vector. Automatically expands capacity if needed.

### at()
```cpp
T& at(size_t index)
const T& at(size_t index) const
```
Returns reference to element at specified index. Throws `std::out_of_range` if index is out of bounds.

### size()
```cpp
size_t size() const
```
Returns the current number of elements in the vector.

### capacity()
```cpp
size_t capacity() const
```
Returns the current capacity (number of elements that can be stored without reallocation).

### segment_size()
```cpp
size_t segment_size() const
```
Returns the segment size in bytes.

## Testing

Run the test suite:
```bash
cd /home/runner/work/http-1.2/http-1.2
cmake .
make test-SHMVector
./test/integration/custom-vector/test-SHMVector
```
