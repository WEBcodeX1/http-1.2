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
 * This implementation is designed to be 100% compatible with shared memory:
 * - Uses offset-based addressing instead of absolute pointers
 * - No iterators or types that rely on other memory regions
 * - The entire data structure can be placed in shared memory
 * 
 * Template parameter T can be any type.
 */
template<typename T>
class CustomVector {
private:
    size_t segment_size_bytes_;  // Size of each segment in bytes
    size_t capacity_;            // Total number of elements that can be stored
    size_t size_;                // Current number of elements
    size_t memory_offset_;       // Offset from this object to the data memory (0 means no allocation)

public:
    /**
     * Constructor: creates a new vector with specified segment size in bytes
     * @param segment_size_bytes Size of each memory segment in bytes
     */
    explicit CustomVector(size_t segment_size_bytes) 
        : segment_size_bytes_(segment_size_bytes),
          capacity_(0),
          size_(0),
          memory_offset_(0) {
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
        if (memory_offset_ != 0) {
            free(get_memory_base());
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
        if (memory_offset_ != 0 && size_ > 0) {
            char* old_memory = get_memory_base();
            for (size_t i = 0; i < size_; ++i) {
                T* old_element = reinterpret_cast<T*>(old_memory + i * segment_size_bytes_);
                T* new_element = reinterpret_cast<T*>(new_memory + i * segment_size_bytes_);
                
                // Move construct into new location
                new (new_element) T(std::move(*old_element));
                
                // Destroy old element
                old_element->~T();
            }
            
            // Free old memory
            free(old_memory);
        }

        // Calculate offset from this object to the new memory
        char* this_addr = reinterpret_cast<char*>(this);
        memory_offset_ = new_memory - this_addr;
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
     * Gets the base address of the data memory using offset-based addressing
     */
    char* get_memory_base() {
        if (memory_offset_ == 0) {
            return nullptr;
        }
        char* this_addr = reinterpret_cast<char*>(this);
        return this_addr + memory_offset_;
    }

    const char* get_memory_base() const {
        if (memory_offset_ == 0) {
            return nullptr;
        }
        const char* this_addr = reinterpret_cast<const char*>(this);
        return this_addr + memory_offset_;
    }

    /**
     * Gets pointer to element at specified index
     * Memory address calculation: base_address + (index * segment_size_bytes)
     */
    T* get_element_ptr(size_t index) {
        char* base = get_memory_base();
        return reinterpret_cast<T*>(base + index * segment_size_bytes_);
    }

    const T* get_element_ptr(size_t index) const {
        const char* base = get_memory_base();
        return reinterpret_cast<const T*>(base + index * segment_size_bytes_);
    }
};
