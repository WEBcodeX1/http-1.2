13. Memory Alignment
====================

13.1. Overview
--------------

This document describes the memory alignment guarantees and implementation in the HTTP/1.2 codebase.

13.2. Memory Allocation and Alignment
-------------------------------------

13.2.1. Default Alignment with `malloc`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``MemoryManager`` class uses ``malloc`` for memory allocation, which provides the following alignment guarantees:

1. **Standard Guarantee**: ``malloc`` returns memory aligned to ``alignof(max_align_t)``
   - On x86_64 systems: typically 16 bytes
   - On x86 systems: typically 8 bytes
   - This is sufficient for all standard types including atomic types

2. **Alignment Requirements for Common Types**:
   - ``char``: 1 byte
   - ``uint16_t``: 2 bytes
   - ``uint32_t``: 4 bytes
   - ``atomic_uint16_t``: typically 2 bytes (same as ``uint16_t``)
   - ``atomic_uint32_t``: typically 4 bytes (same as ``uint32_t``)

3. **Why malloc is Sufficient**:
   - ``malloc`` alignment (16 bytes on x86_64) exceeds all requirements
   - No custom alignment is needed for the types used in this project
   - Atomic operations work correctly with malloc-allocated memory

13.2.2. Compile-Time Alignment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``MemoryManager`` template class provides compile-time alignment information:

.. code-block:: cpp

   template<class T>
   class MemoryManager {
       // Compile-time alignment constant
       static constexpr size_t Alignment = alignof(T);
    
       // Get alignment requirement at compile-time
       static constexpr size_t getAlignment() {
           return Alignment;
       }
   };


**Benefits**:

* Zero runtime overhead for alignment queries
* Enables compile-time optimizations
* Type-safe alignment checking

13.2.3. Runtime Alignment Verification
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In debug builds, alignment is verified at allocation time:

.. code-block:: cpp

   #if defined(DEBUG_BUILD)
   // Verify alignment in debug builds
   verifyAlignment();
   #endif

This helps catch alignment issues during development without impacting production performance.

13.2.4. Alignment Checking Utilities
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``MemoryManager`` provides static methods for alignment checking:

.. code-block:: cpp

   // Check if a pointer is properly aligned for type T
   static bool isAligned(const void* ptr) {
       return reinterpret_cast<uintptr_t>(ptr) % Alignment == 0;
   }

13.3. When std::align is NOT Needed
-----------------------------------

``std::align`` is primarily useful for:

* Creating multiple sub-objects with different alignment requirements in a buffer
* Manual memory management with over-aligned types
* Implementing custom allocators

In the HTTP/1.2 codebase:

* `malloc` already provides sufficient alignment
* Each `MemoryManager` instance manages a single type `T`
* No over-aligned types are used
* Therefore, `std::align` is not necessary

13.4. Shared Memory Layout and Alignment
----------------------------------------

The shared memory segments use the following types:

13.4.1. StaticFS Request SHM #1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   0x00000000  atomic_uint16_t  StaticFSLock    (2 bytes, 2-byte aligned)
   0x00000002  uint16_t         RequestCount    (2 bytes, 2-byte aligned)

13.4.2. AS Request and Result Metadata SHM #2
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   0x00000000  atomic_uint16_t  CanRead         (2 bytes, 2-byte aligned)
   0x00000002  atomic_uint16_t  WriteReady      (2 bytes, 2-byte aligned)
   0x00000004  uint16_t         ClientFD        (2 bytes, 2-byte aligned)
   0x00000006  uint16_t         HTTPVersion     (2 bytes, 2-byte aligned)
   0x00000008  uint16_t         HTTPMethod      (2 bytes, 2-byte aligned)
   0x0000000a  uint16_t         ReqNr           (2 bytes, 2-byte aligned)
   0x0000000c  uint32_t         ReqPayloadLen   (4 bytes, 4-byte aligned)
   0x00000010  uint32_t         ResPayloadLen   (4 bytes, 4-byte aligned)

**Alignment Notes**:

* All fields are naturally aligned (offset is a multiple of the type's size)
* `mmap` with `MAP_ANONYMOUS` returns page-aligned memory (typically 4096 bytes)
* This provides more than sufficient alignment for all types

13.5. Hugepage Support
----------------------

The code uses ``madvise(ptr, size, MADV_HUGEPAGE)`` to request transparent hugepage support:

.. code-block:: cpp

   madvise(MemoryBaseAddress, MemSizeBytes, MADV_HUGEPAGE);

**Benefits**:

* Reduced TLB misses for large memory allocations
* Improved performance for memory-intensive operations
* Does not affect alignment (hugepages are more strictly aligned)

13.6. Testing
-------------

Memory alignment is verified through unit tests in ``test/unit/memory-alignment/``:

1. **Default malloc alignment**: Verifies ``malloc`` provides ``max_align_t`` alignment
2. **MemoryManager alignment**: Tests alignment for various types (``char``, ``uint16_t``, ``atomic<uint16_t>``)
3. **Segment pointer alignment**: Ensures all segment pointers maintain proper alignment
4. **Atomic type alignment**: Verifies atomic types have correct alignment
