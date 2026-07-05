C++ Classes Reference
=====================

This section summarizes the classes and templates that are present in the current source tree.

Core Runtime
------------

Server
~~~~~~

``Server`` inherits from ``ASProcessHandler`` and ``ClientHandler`` and coordinates startup,
socket setup, signal handling, and the main poll loop.

.. cpp:class:: Server

   .. cpp:function:: void init()
   .. cpp:function:: void setupSocket()
   .. cpp:function:: void setupPoll()
   .. cpp:function:: void ServerLoop()
   .. cpp:function:: void acceptClient()
   .. cpp:function:: void setupSharedMemory()
   .. cpp:function:: static void addChildPID(pid_t)
   .. cpp:function:: static void terminateChildren()

``setupSharedMemory()`` currently exists as an empty hook in ``src/Server.cpp``.

Client
~~~~~~

``Client`` inherits from ``HTTPParser`` and stores the socket file descriptor plus a reusable
receive buffer pointer.

.. cpp:class:: Client

   .. cpp:function:: Client(Filedescriptor_t, char*)
   .. cpp:function:: bool receiveData()

``receiveData()`` repeatedly reads from the client socket until the kernel buffer is drained, EOF
is reached, or a hard receive error occurs.

ClientHandler
~~~~~~~~~~~~~

``ClientHandler`` owns the active client map and epoll state.

.. cpp:class:: ClientHandler

   .. cpp:type:: typedef std::shared_ptr<Client> ClientRef_t
   .. cpp:type:: typedef unordered_map<Filedescriptor_t, ClientRef_t> ClientMap_t
   .. cpp:function:: void addClient(const Filedescriptor_t)
   .. cpp:function:: void processClients()
   .. cpp:function:: void readClientData(const uint16_t)

Configuration and Filesystem
----------------------------

Configuration
~~~~~~~~~~~~~

``Configuration`` loads ``config/config.json`` and maps namespace definitions to runtime objects.

.. cpp:class:: Configuration

   .. cpp:type:: typedef struct NamespaceProps_t
   .. cpp:type:: typedef unordered_map<string, NamespaceProps_t> Namespaces_t
   .. cpp:function:: void mapStaticFSData()

Filesystem
~~~~~~~~~~

``Filesystem`` indexes static files and their metadata.

.. cpp:class:: Filesystem

   .. cpp:type:: typedef struct FileProperties_t
   .. cpp:type:: typedef vector<string> FilelistPlain_t
   .. cpp:type:: typedef unordered_map<string, const FileProperties_t> FileListExtended_t
   .. cpp:function:: void initFiles()
   .. cpp:function:: void processFileProperties()
   .. cpp:function:: FileProperties_t getFilePropertiesByFile(const string &File)
   .. cpp:function:: bool checkFileExists(const string &File)
   .. cpp:function:: string getFileEtag(const string &File)

Backend Support
---------------

ASProcessHandler
~~~~~~~~~~~~~~~~

``ASProcessHandler`` currently provides the lifecycle interface for backend child processes and the
configured interpreter count.

.. cpp:class:: ASProcessHandler

   .. cpp:type:: typedef struct ASProcessHandlerSHMPointer_t
   .. cpp:function:: void forkProcessASHandler(ASProcessHandlerSHMPointer_t)
   .. cpp:function:: uint getASInterpreterCount()
   .. cpp:function:: void setTerminationHandler()
   .. cpp:function:: static void terminate(int)
   .. cpp:function:: static void registerChildPID(pid_t)

HTTP Library
------------

HTTPParser
~~~~~~~~~~

``HTTPParser`` is the reusable request parser from ``lib/http/httpparser.*``.

.. cpp:class:: HTTPParser

   .. cpp:type:: typedef unordered_map<string, string> RequestHeader_t
   .. cpp:type:: typedef unordered_map<string, string> URLParamMap_t
   .. cpp:type:: typedef vector<RequestProperties_t> RequestsMap_t
   .. cpp:function:: void appendBuffer(const char*, const uint16_t)
   .. cpp:function:: RequestsMap_t getRequests()
   .. cpp:function:: RequestsMapPtr_t getRequestsPtr()

HTTPMessageGenerator
~~~~~~~~~~~~~~~~~~~~

The message-generator component is implemented by ``HTTPGenerator``.

.. cpp:class:: HTTPGenerator

   .. cpp:type:: typedef unordered_map<HeaderID_t, HeaderValue_t> ResponseHeader_t
   .. cpp:function:: void MsgReset()
   .. cpp:function:: void MsgSetStatus(const uint16_t, const string&)
   .. cpp:function:: void MsgAddHeader(const HeaderID_t, const HeaderValue_t)
   .. cpp:function:: void MsgAddDateHeader()
   .. cpp:function:: void MsgSetBodyRef(const unsigned char*, const unsigned int)
   .. cpp:function:: void MsgGenerate()
   .. cpp:function:: SendMetadata_t MsgGetSendMetadata()
   .. cpp:function:: bool MsgUpdateSendMetadata(ssize_t)

Utilities
---------

MemoryManager
~~~~~~~~~~~~~

``MemoryManager<T>`` provides aligned segmented storage used by the runtime for reusable buffers.

.. cpp:class:: template<class T> MemoryManager

   .. cpp:function:: MemoryManager(uint16_t SegmentCount, uint16_t SegmentSize)
   .. cpp:function:: T* getNextMemPointer()
   .. cpp:function:: T* getMemBaseAddress()

SHMVector
~~~~~~~~~

``SHMVector<T>`` in ``src/SHMVector.hpp`` is the shared-memory-safe vector implementation.

.. cpp:class:: template<typename T> SHMVector

   .. cpp:function:: SHMVector(size_t segment_size_bytes, void* shared_memory_ptr, size_t shared_memory_size)
   .. cpp:function:: void reserve(size_t element_count)
   .. cpp:function:: void push_back(const T& element)
   .. cpp:function:: T& at(size_t index)
   .. cpp:function:: size_t size() const
   .. cpp:function:: size_t capacity() const
   .. cpp:function:: void eraseAt(size_t index)
   .. cpp:function:: T getNextElement()

CPU
~~~

``CPU`` contains CPU affinity helper methods used by process-oriented runtime code.

.. cpp:class:: CPU

   .. cpp:function:: void bindToCPU(int core)
   .. cpp:function:: void bindToCPUs(std::vector<int> cores)

Vector
~~~~~~

``Vector`` contains custom helper functionality for vector-style containers.

.. cpp:class:: Vector

   .. cpp:function:: void multiErase(std::vector<size_t> indices)
