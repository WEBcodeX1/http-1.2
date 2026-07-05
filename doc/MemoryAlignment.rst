Memory Alignment
================

Overview
--------

This document describes the memory alignment guarantees and implementation in the ``NLAP`` codebase.

Memory Allocation and Alignment
-------------------------------

Default Alignment with `malloc`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

Compile-Time Alignment
~~~~~~~~~~~~~~~~~~~~~~

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

Runtime Alignment Verification
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In debug builds, alignment is verified at allocation time:

.. code-block:: cpp

   #if defined(DEBUG_BUILD)
   // Verify alignment in debug builds
   verifyAlignment();
   #endif

This helps catch alignment issues during development without impacting production performance.

Alignment Checking Utilities
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``MemoryManager`` provides static methods for alignment checking:

.. code-block:: cpp

   // Check if a pointer is properly aligned for type T
   static bool isAligned(const void* ptr) {
       return reinterpret_cast<uintptr_t>(ptr) % Alignment == 0;
   }

When std::align is NOT Needed
-----------------------------

``std::align`` is primarily useful for:

* Creating multiple sub-objects with different alignment requirements in a buffer
* Manual memory management with over-aligned types
* Implementing custom allocators

In the ``NLAP`` codebase:

* `malloc` already provides sufficient alignment
* Each `MemoryManager` instance manages a single type `T`
* No over-aligned types are used
* Therefore, `std::align` is not necessary

Hugepage Support
----------------

The code uses ``madvise(ptr, size, MADV_HUGEPAGE)`` to request transparent hugepage support:

.. code-block:: cpp

   madvise(MemoryBaseAddress, MemSizeBytes, MADV_HUGEPAGE);

**Benefits**:

* Reduced TLB misses for large memory allocations
* Improved performance for memory-intensive operations
* Does not affect alignment (hugepages are more strictly aligned)

Testing
-------

Memory alignment is verified through unit tests in ``test/unit/memory-alignment/``:

1. **Default malloc alignment**: Verifies ``malloc`` provides ``max_align_t`` alignment
2. **MemoryManager alignment**: Tests alignment for various types (``char``, ``uint16_t``, ``atomic<uint16_t>``)
3. **Segment pointer alignment**: Ensures all segment pointers maintain proper alignment
4. **Atomic type alignment**: Verifies atomic types have correct alignment
