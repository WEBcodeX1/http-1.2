#pragma once

#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <new>
#include <pthread.h>
#include <cstring>
#include <string>

/**
 * Minimalistic vector implementation with custom memory management.
 * Memory is allocated in segments where each segment is a configurable number of bytes.
 * The next segment's memory address is always: previous_segment_address + segment_size_bytes
 * 
 * Design for shared memory compatibility:
 * - Uses externally provided shared memory (e.g., from mmap)
 * - No iterators or complex types that rely on separate memory regions
 * - Uses simple POD-style data members only
 * - The data memory uses contiguous segment-based addressing
 * - Does not own the memory (no malloc/free)
 * - Template parameter T should be trivially copyable for optimal shared memory usage
 * - Thread-safe getNextElement() operation using pthread mutex
 * 
 * Usage:
 *   void* shmem = mmap(NULL, 640000, PROT_READ | PROT_WRITE, 
 *                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
 *   CustomVector<int> vec(sizeof(int), shmem, 640000);
 * 
 * Template parameter T can be any type.
 */
template<typename T>
class CustomVector {
private:
    size_t segment_size_bytes_;  // Size of each segment in bytes
    size_t capacity_;            // Total number of elements that can be stored
    size_t size_;                // Current number of elements
    char* memory_base_;          // Base address of shared memory
    size_t memory_total_bytes_;  // Total size of shared memory region
    pthread_mutex_t mutex_;      // Mutex for thread-safe operations

public:
    /**
     * Constructor: creates a new vector with specified segment size in bytes
     * @param segment_size_bytes Size of each memory segment in bytes
     * @param shared_memory_ptr Pointer to externally allocated shared memory (e.g., from mmap)
     * @param shared_memory_size Total size of the shared memory region in bytes
     */
    CustomVector(size_t segment_size_bytes, void* shared_memory_ptr, size_t shared_memory_size) 
        : segment_size_bytes_(segment_size_bytes),
          capacity_(0),
          size_(0),
          memory_base_(static_cast<char*>(shared_memory_ptr)),
          memory_total_bytes_(shared_memory_size) {
        if (segment_size_bytes == 0) {
            throw std::invalid_argument("Segment size must be greater than 0");
        }
        if (shared_memory_ptr == nullptr) {
            throw std::invalid_argument("Shared memory pointer cannot be null");
        }
        if (shared_memory_size == 0) {
            throw std::invalid_argument("Shared memory size must be greater than 0");
        }
        
        // Initialize mutex with process-shared attribute for cross-process synchronization
        pthread_mutexattr_t attr;
        int ret = pthread_mutexattr_init(&attr);
        if (ret != 0) {
            throw std::runtime_error("Failed to initialize mutex attributes: error code " + std::to_string(ret));
        }
        
        ret = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        if (ret != 0) {
            pthread_mutexattr_destroy(&attr);
            throw std::runtime_error("Failed to set mutex process-shared attribute: error code " + std::to_string(ret));
        }
        
        ret = pthread_mutex_init(&mutex_, &attr);
        if (ret != 0) {
            pthread_mutexattr_destroy(&attr);
            throw std::runtime_error("Failed to initialize mutex: error code " + std::to_string(ret));
        }
        
        pthread_mutexattr_destroy(&attr);
    }

    /**
     * Destructor: destroys constructed elements but does NOT free memory
     * (memory is owned externally and must be freed by the caller)
     */
    ~CustomVector() {
        // Destroy all constructed elements
        for (size_t i = 0; i < size_; ++i) {
            get_element_ptr(i)->~T();
        }
        // Destroy the mutex
        pthread_mutex_destroy(&mutex_);
        // Note: We do NOT free memory_base_ as it's externally managed (e.g., via munmap)
    }

    // Disable copy constructor and copy assignment
    CustomVector(const CustomVector&) = delete;
    CustomVector& operator=(const CustomVector&) = delete;

    /**
     * Reserves memory for the specified number of elements
     * @param element_count Number of elements to reserve space for
     * @throws std::bad_alloc if requested capacity exceeds available shared memory
     */
    void reserve(size_t element_count) {
        if (element_count <= capacity_) {
            return; // Already have enough capacity
        }

        // Calculate total memory needed
        size_t total_bytes = element_count * segment_size_bytes_;
        
        // Check if we have enough shared memory
        if (total_bytes > memory_total_bytes_) {
            throw std::bad_alloc(); // Not enough shared memory
        }

        // Since we're using pre-allocated shared memory, we just update capacity
        // No need to move elements as memory_base_ stays the same
        capacity_ = element_count;
    }

    /**
     * Adds an element to the end of the vector
     * @param element The element to add
     */
    void push_back(const T& element) {
        // Expand capacity if needed
        if (size_ >= capacity_) {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            reserve(new_capacity);
        }

        // Construct element at the next available position
        T* position = get_element_ptr(size_);
        new (position) T(element);
        ++size_;
    }

    /**
     * Gets element at the specified index
     * @param index Index of the element
     * @return Reference to the element
     * @throws std::out_of_range if index is out of bounds
     */
    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of bounds");
        }
        return *get_element_ptr(index);
    }

    /**
     * Gets element at the specified index (const version)
     * @param index Index of the element
     * @return Const reference to the element
     * @throws std::out_of_range if index is out of bounds
     */
    const T& at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of bounds");
        }
        return *get_element_ptr(index);
    }

    /**
     * Returns the number of elements in the vector
     * @return Number of elements
     */
    size_t size() const {
        return size_;
    }

    /**
     * Returns the current capacity of the vector
     * @return Capacity (number of elements that can be stored without reallocation)
     */
    size_t capacity() const {
        return capacity_;
    }

    /**
     * Returns the segment size in bytes
     * @return Segment size in bytes
     */
    size_t segment_size() const {
        return segment_size_bytes_;
    }

    /**
     * Removes element at specified index (thread-safe)
     * @param index Index of the element to remove
     * @throws std::out_of_range if index is out of bounds
     * 
     * This operation shifts all elements after the removed element forward by one position.
     * Time complexity: O(n) where n is the number of elements after the removed element.
     * This function is thread-safe and can be called from multiple threads or processes.
     */
    void eraseAt(size_t index) {
        pthread_mutex_lock(&mutex_);
        
        try {
            eraseAt_unlocked(index);
            pthread_mutex_unlock(&mutex_);
        } catch (...) {
            pthread_mutex_unlock(&mutex_);
            throw;
        }
    }

    /**
     * Thread-safe operation: returns the first element and removes it from the vector
     * @return Copy of the first element
     * @throws std::out_of_range if vector is empty
     * 
     * This function is thread-safe and can be called from multiple threads or processes.
     * It uses a mutex to ensure atomic get-and-remove operation.
     */
    T getNextElement() {
        pthread_mutex_lock(&mutex_);
        
        try {
            if (size_ == 0) {
                pthread_mutex_unlock(&mutex_);
                throw std::out_of_range("Cannot get next element: vector is empty");
            }

            // Get a copy of the first element
            T result = *get_element_ptr(0);

            // Remove the first element (unlocked version since we already hold the lock)
            eraseAt_unlocked(0);

            pthread_mutex_unlock(&mutex_);
            return result;
        } catch (...) {
            pthread_mutex_unlock(&mutex_);
            throw;
        }
    }

private:
    /**
     * Removes element at specified index (internal unlocked version)
     * @param index Index of the element to remove
     * @throws std::out_of_range if index is out of bounds
     * 
     * This is an internal helper that does not acquire locks.
     * Use the public eraseAt() for thread-safe access.
     */
    void eraseAt_unlocked(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of bounds");
        }

        // Destroy the element at the index
        get_element_ptr(index)->~T();

        // Shift all elements after the erased element forward
        for (size_t i = index; i < size_ - 1; ++i) {
            T* current = get_element_ptr(i);
            T* next = get_element_ptr(i + 1);
            
            // Use placement new to copy-construct in place
            new (current) T(*next);
            
            // Destroy the old element
            next->~T();
        }

        --size_;
    }
    /**
     * Gets pointer to element at specified index
     * Memory address calculation: base_address + (index * segment_size_bytes)
     */
    T* get_element_ptr(size_t index) {
        return reinterpret_cast<T*>(memory_base_ + index * segment_size_bytes_);
    }

    const T* get_element_ptr(size_t index) const {
        return reinterpret_cast<const T*>(memory_base_ + index * segment_size_bytes_);
    }
};
