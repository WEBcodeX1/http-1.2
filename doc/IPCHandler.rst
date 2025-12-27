12. IPC / Shared Memory Handler
===============================

The IPCHandler (Inter Process Communication) Components are responsible for managing Shared Memory
Segmentation which is used for passing data between system processes.

The following two handler exist:

* IPCHandler (StaticFS GET Requests), IPCHandler.cpp
* IPCHandlerAS (POST Application Server Requests), IPCHandlerAS.cpp

12.1. IPCHandler
----------------

IPCHandler Memory (SHM Segment #1) Layout see :doc:`Overview` Section 3.1.

The following describes the functionality in detail.

12.1.1. Constructor
~~~~~~~~~~~~~~~~~~~

Set _OffsetSizes from ``static const SharedMemOffset_t ElementSizes`` (std::vector) defined in IPCHandler.hpp.

12.1.2. setBaseAddress()
~~~~~~~~~~~~~~~~~~~~~~~~

* Set _BaseAddress (void*) passed by BaseAddress param
* Call this.resetOffset()

12.1.3. resetOffset()
~~~~~~~~~~~~~~~~~~~~~

* Set _OffsetAddress = _BaseAddress

12.1.4. getCurrentOffsetAddress()
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Return _OffsetAddress

12.1.5. getNextAddress()
~~~~~~~~~~~~~~~~~~~~~~~~

* Increment _OffsetAddress by Type@_ElementOffset
* Call _stepElementOffset

12.1.6. getNextAddress(Bytes)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Increment _OffsetAddress by Bytes
* Call _stepElementOffset

12.1.7. _stepElementOffset()
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Increment _ElementOffset
* If _ElementOffset == count(ElementSizes): _ElementOffset = 0

12.2. IPCHandlerAS
------------------

IPCHandler Memory (SHM Segment #2, #3 and #4) Layout see :doc:`Overview` Section 3.2., 3.3 and 3.4.

12.2.1. setBaseAddresses()
~~~~~~~~~~~~~~~~~~~~~~~~~~

Set SHM Address Pointers for SHM Segment #2, #3 and #4.

12.2.2. getMetaAddress()
~~~~~~~~~~~~~~~~~~~~~~~~

Get Meta Address by Index (SHM Segment #2).

12.2.3. getRequestAddress()
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Get Request Payload Address (SHM Segment #3).

12.2.4. getResultAddress()
~~~~~~~~~~~~~~~~~~~~~~~~~~

Get Result Payload Address (SHM Segment #4).
