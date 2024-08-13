# 1. IPC / Shared Memory Handler

The IPCHandler (Inter Process Communication) Components are responsible for managing Shared Memory
Segmentation which is used for passing data between system processes.

The following two handler exist:

- IPCHandler (StaticFS GET Requests), IPCHandler.cpp
- IPCHandlerAS (POST Application Server Requests), IPCHandlerAS.cpp

## 1.1. IPCHandler

IPCHandler Memory (SHM Segment #1) Layout see [BaseLayout.md](./BaseLayout.md) Section 3.1.

The following describes the functionality in detail.

### 1.1.1. Construtor

Set _OffsetSizes from `static const SharedMemOffset_t ElementSizes` (std::vector) defined in IPCHandler.hpp.

### 1.1.2. setBaseAddress()

- Set _BaseAddress (void*) passed by BaseAddress param
- Call this.resetOffset()

### 1.1.3. resetOffset()

- Set _OffsetAddress = _BaseAddress

### 1.1.4. getCurrentOffsetAddress()

- Return _OffsetAddress

### 1.1.5. getNextAddress()

- Increment _OffsetAddress by Type@_ElementOffset
- Call _stepElementOffset

### 1.1.6. getNextAddress(Bytes)

- Increment _OffsetAddress by Bytes
- Call _stepElementOffset

### 1.1.7. _stepElementOffset()

- Increment _ElementOffset
- If _ElementOffset == count(ElementSizes): _ElementOffset = 0

## 1.2. IPCHandlerAS

IPCHandler Memory (SHM Segment #2 and Segment #3) Layout see [BaseLayout.md](./BaseLayout.md) Section 3.2. and 3.3.
