#pragma once

#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <new>

/**
 * Minimalistic vector implementation with custom memory management.
 * Memory is allocated in segments where each segment is a configurable number of bytes.
 * The next segment's memory address is always: previous_segment_address + segment_size_bytes
 * 
 * Design for shared memory compatibility:
 * - No iterators or complex types that rely on separate memory regions
 * - Uses simple POD-style data members only
 * - The data memory uses contiguous segment-based addressing
 * - Template parameter T should be trivially copyable for optimal shared memory usage
 * 
 * Note: This implementation uses absolute pointers. For cross-process shared memory,
 * the entire CustomVector structure would need to be allocated within the shared memory
 * region and adjusted for process address spaces.
 * 
 * Template parameter T can be any type.
 */
template<typename T>
class CustomVector {
private:
    size_t segment_size_bytes_;  // Size of each segment in bytes
    size_t capacity_;            // Total number of elements that can be stored
    size_t size_;                // Current number of elements
    char* memory_base_;          // Base address of allocated memory

public:
    /**
     * Constructor: creates a new vector with specified segment size in bytes
     * @param segment_size_bytes Size of each memory segment in bytes
     */
    explicit CustomVector(size_t segment_size_bytes) 
        : segment_size_bytes_(segment_size_bytes),
          capacity_(0),
          size_(0),
          memory_base_(nullptr) {
        if (segment_size_bytes == 0) {
            throw std::invalid_argument("Segment size must be greater than 0");
        }
    }

    /**
     * Destructor: frees allocated memory
     */
    ~CustomVector() {
        // Destroy all constructed elements
        for (size_t i = 0; i < size_; ++i) {
            get_element_ptr(i)->~T();
        }
        // Free raw memory
        if (memory_base_ != nullptr) {
            free(memory_base_);
        }
    }

    // Disable copy constructor and copy assignment
    CustomVector(const CustomVector&) = delete;
    CustomVector& operator=(const CustomVector&) = delete;

    /**
     * Reserves memory for the specified number of elements
     * @param element_count Number of elements to reserve space for
     */
    void reserve(size_t element_count) {
        if (element_count <= capacity_) {
            return; // Already have enough capacity
        }

        // Calculate total memory needed
        size_t total_bytes = element_count * segment_size_bytes_;
        
        // Allocate new memory
        char* new_memory = static_cast<char*>(malloc(total_bytes));
        if (new_memory == nullptr) {
            throw std::bad_alloc();
        }

        // Move existing elements to new memory if we have any
        if (memory_base_ != nullptr && size_ > 0) {
            for (size_t i = 0; i < size_; ++i) {
                T* old_element = reinterpret_cast<T*>(memory_base_ + i * segment_size_bytes_);
                T* new_element = reinterpret_cast<T*>(new_memory + i * segment_size_bytes_);
                
                // Move construct into new location
                new (new_element) T(std::move(*old_element));
                
                // Destroy old element
                old_element->~T();
            }
            
            // Free old memory
            free(memory_base_);
        }

        memory_base_ = new_memory;
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

private:
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
