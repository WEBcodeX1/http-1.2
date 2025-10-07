C++ Classes Reference
=====================

This section provides detailed documentation for all C++ classes in the FalconAS HTTP/1.2 server.

Detailed Doxygen UML diagrams can be found here: https://docs.webcodex.de/developer/falconas/doxygen/index.html.

Core Server Classes
-------------------

Server
~~~~~~

The main server class that orchestrates all server operations. Inherits from ``ResultProcessor``, ``ASProcessHandler``, and ``ClientHandler``.

.. cpp:class:: Server

   **Public Methods:**

   .. cpp:function:: Server()
   
      Constructor - Initializes the server instance.

   .. cpp:function:: ~Server()
   
      Destructor - Cleans up server resources.

   .. cpp:function:: void init()
   
      Initializes all server components.

   .. cpp:function:: void setupSocket()
   
      Sets up the server listening socket.

   .. cpp:function:: void setupPoll()
   
      Sets up polling mechanism for the server socket.

   .. cpp:function:: void ServerLoop()
   
      Main server loop that handles incoming connections and processes requests.

   .. cpp:function:: void acceptClient()
   
      Accepts new client connections.

   .. cpp:function:: void setTerminationHandler()
   
      Sets up signal handler for graceful termination.

   .. cpp:function:: static void terminate(int)
   
      Static termination handler called on SIGTERM.

   .. cpp:function:: void setupSharedMemory()
   
      Sets up shared memory segments for IPC.

   .. cpp:function:: void setupFDPassingServer()
   
      Sets up Unix domain socket for file descriptor passing.

   .. cpp:function:: void handleFDPassingRequests()
   
      Handles file descriptor passing requests from child processes.

   .. cpp:function:: static void addChildPID(pid_t)
   
      Registers a child process PID for termination tracking.

   .. cpp:function:: static void terminateChildren()
   
      Sends termination signal to all child processes.

   **Private Members:**

   .. cpp:member:: string SocketListenAddress
   
      IPv4 address to bind the server socket.

   .. cpp:member:: uint SocketListenPort
   
      Port number to listen on.

   .. cpp:member:: struct sockaddr_in SocketAddr
   
      Server socket address structure.

   .. cpp:member:: struct sockaddr_in ClientSocketAddr
   
      Client socket address structure.

   .. cpp:member:: int ServerSocketFD
   
      Server socket file descriptor.

   .. cpp:member:: struct pollfd ServerConnFD[1]
   
      Poll file descriptor array for server socket.

   .. cpp:member:: void* _SHMStaticFS
   
      Pointer to static filesystem shared memory.

   .. cpp:member:: void* _SHMPythonASMeta
   
      Pointer to Python application server metadata shared memory.

   .. cpp:member:: void* _SHMPythonASRequests
   
      Pointer to Python AS request payload shared memory.

   .. cpp:member:: void* _SHMPythonASResults
   
      Pointer to Python AS result payload shared memory.

   .. cpp:member:: int _FDPassingServerFD
   
      File descriptor for Unix domain socket used in FD passing.

   .. cpp:member:: static std::vector<pid_t> ChildPIDs
   
      Vector storing all child process IDs for termination management.


Client
~~~~~~

Represents a client connection and manages client-specific state.

.. cpp:class:: Client

   **Public Methods:**

   .. cpp:function:: Client(ClientFD_t)
   
      Constructor - Creates a client instance with the given file descriptor.

   .. cpp:function:: ~Client()
   
      Destructor - Cleans up client resources.

   .. cpp:function:: ClientRequestNr_t getNextReqNr()
   
      Returns the next request number for this client.

   **Type Definitions:**

   .. cpp:type:: typedef uint16_t ClientFD_t
   
      Client file descriptor type.

   .. cpp:type:: typedef uint16_t ClientRequestNr_t
   
      Client request number type.

   **Protected Members:**

   .. cpp:member:: ClientFD_t _ClientFD
   
      Client socket file descriptor.

   **Private Members:**

   .. cpp:member:: ClientRequestNr_t _RequestNr
   
      Current request number for this client.

   .. cpp:member:: bool _Error
   
      Error flag indicating if an error occurred.

   .. cpp:member:: uint16_t _ErrorID
   
      Error identifier code.

   .. cpp:member:: time_t _RequestStartTime
   
      Timestamp when the request started.

   .. cpp:member:: time_t _RequestEndTime
   
      Timestamp when the request ended.

   .. cpp:member:: time_t _ResponseStartTime
   
      Timestamp when the response started.

   .. cpp:member:: time_t _ResponseEndTime
   
      Timestamp when the response ended.

   .. cpp:member:: bool _TimeoutReached
   
      Flag indicating if timeout was reached.


ClientHandler
~~~~~~~~~~~~~

Handles client connections, epoll setup, and data buffering.

.. cpp:class:: ClientHandler

   **Type Definitions:**

   .. cpp:type:: typedef std::shared_ptr<HTTPParser> ClientRef_t
   
      Shared pointer to HTTP parser for client.

   .. cpp:type:: typedef pair<uint16_t, const ClientRef_t> ClientMapPair_t
   
      Pair of client FD and client reference.

   .. cpp:type:: typedef unordered_map<uint16_t, const ClientRef_t> ClientMap_t
   
      Map of client file descriptors to client references.

   .. cpp:type:: typedef struct ClientHandlerSHMPointer_t
   
      Structure holding pointers to shared memory segments:
      
      - ``void* StaticFSPtr`` - Static filesystem SHM pointer
      - ``void* PostASMetaPtr`` - AS metadata SHM pointer  
      - ``void* PostASRequestsPtr`` - AS requests SHM pointer
      - ``void* PostASResultsPtr`` - AS results SHM pointer

   **Public Methods:**

   .. cpp:function:: ClientHandler()
   
      Constructor - Initializes client handler and sets up epoll.

   .. cpp:function:: ~ClientHandler()
   
      Destructor - Cleans up client handler resources.

   .. cpp:function:: void addClient(const uint16_t)
   
      Adds a new client to the handler with the given file descriptor.

   .. cpp:function:: void processClients()
   
      Processes all clients with waiting data using epoll.

   .. cpp:function:: void readClientData(const uint16_t)
   
      Reads data from the specified client file descriptor.

   .. cpp:function:: void setSharedMemPointer(ClientHandlerSHMPointer_t)
   
      Sets the shared memory pointers for client handler.

   .. cpp:function:: void setClientHandlerConfig()
   
      Configures the client handler settings.

   .. cpp:function:: ASRequestHandler& getClientHandlerASRequestHandlerRef()
   
      Returns reference to the AS request handler.

   **Public Members:**

   .. cpp:member:: uint16_t ProcessedClients
   
      Counter for number of processed clients.

   .. cpp:member:: MemoryManager<char> BufferMemory
   
      Memory manager for client data buffers.

   **Private Members:**

   .. cpp:member:: ClientMap_t Clients
   
      Map of active client connections.

   .. cpp:member:: struct epoll_event EpollEvent
   
      Epoll event structure.

   .. cpp:member:: struct epoll_event EpollEvents[EPOLL_FD_COUNT_MAX]
   
      Array of epoll events.

   .. cpp:member:: int EpollFD
   
      Epoll file descriptor.

   .. cpp:member:: uint8_t LastProcessingIDStaticFS
   
      Last processing ID for static filesystem.

   .. cpp:member:: uint8_t LastProcessingIDAppServer
   
      Last processing ID for application server.

   .. cpp:member:: void* _SHMStaticFS
   
      Static filesystem shared memory pointer.

   .. cpp:member:: void* _SHMPythonASMeta
   
      Python AS metadata shared memory pointer.

   .. cpp:member:: void* _SHMPythonASRequests
   
      Python AS requests shared memory pointer.

   .. cpp:member:: void* _SHMPythonASResults
   
      Python AS results shared memory pointer.

   .. cpp:member:: ASRequestHandlerRef_t _ASRequestHandlerRef
   
      Reference to AS request handler.


Configuration Classes
---------------------

Configuration
~~~~~~~~~~~~~

Manages server configuration loaded from JSON configuration file.

.. cpp:class:: Configuration

   **Type Definitions:**

   .. cpp:type:: typedef struct NamespaceProps_t
   
      Namespace properties structure containing:
      
      - ``nlohmann::json JSONConfig`` - JSON configuration for namespace
      - ``std::shared_ptr<Filesystem> FilesystemRef`` - Reference to filesystem handler

   .. cpp:type:: typedef unordered_map<string, NamespaceProps_t> Namespaces_t
   
      Map of namespace IDs to namespace properties.

   .. cpp:type:: typedef pair<string, NamespaceProps_t> NamespacePair_t
   
      Pair of namespace ID and properties.

   .. cpp:type:: typedef vector<string> Mimetypes_t
   
      Vector of supported MIME types.

   .. cpp:type:: typedef Namespaces_t& NamespacesRef_t
   
      Reference to namespaces map.

   **Public Methods:**

   .. cpp:function:: Configuration()
   
      Constructor - Loads and parses configuration from config.json.

   .. cpp:function:: ~Configuration()
   
      Destructor - Cleans up configuration resources.

   .. cpp:function:: void mapStaticFSData()
   
      Maps static filesystem data for all namespaces.

   **Public Members:**

   .. cpp:member:: string RunAsUnixUser
   
      Unix username to run the server as.

   .. cpp:member:: string RunAsUnixGroup
   
      Unix group name to run the server as.

   .. cpp:member:: uint16_t RunAsUnixUserID
   
      Unix user ID to drop privileges to.

   .. cpp:member:: uint16_t RunAsUnixGroupID
   
      Unix group ID to drop privileges to.

   .. cpp:member:: string BasePath
   
      Base path for web content.

   .. cpp:member:: string ServerAddress
   
      Server IPv4 bind address.

   .. cpp:member:: uint16_t ServerPort
   
      Server listen port.

   .. cpp:member:: Mimetypes_t Mimetypes
   
      List of supported MIME types.

   .. cpp:member:: Namespaces_t Namespaces
   
      Map of all configured namespaces.


Filesystem
~~~~~~~~~~

Handles filesystem operations and virtual host file mappings.

.. cpp:class:: Filesystem

   **Type Definitions:**

   .. cpp:type:: typedef struct FileProperties_t
   
      File properties structure containing:
      
      - ``Filedescriptor_t Filedescriptor`` - File descriptor
      - ``unsigned int FileSize`` - Size of the file in bytes
      - ``string FileName`` - Name of the file
      - ``string FileExtension`` - File extension
      - ``string MimeType`` - MIME type of the file
      - ``string ETag`` - ETag for caching
      - ``string LastModifiedString`` - Last modified date string
      - ``string LastModifiedSeconds`` - Last modified in seconds

   .. cpp:type:: typedef unordered_map<string, const string> MimetypeRelations_t
   
      Map of file extensions to MIME types.

   .. cpp:type:: typedef unsigned int Filedescriptor_t
   
      File descriptor type.

   .. cpp:type:: typedef vector<string> FilelistPlain_t
   
      Vector of file paths.

   .. cpp:type:: typedef pair<string, FileProperties_t> FileListExtendedPair_t
   
      Pair of file path and properties.

   .. cpp:type:: typedef unordered_map<string, const FileProperties_t> FileListExtended_t
   
      Map of file paths to properties.

   **Public Methods:**

   .. cpp:function:: Filesystem()
   
      Constructor - Initializes filesystem handler.

   .. cpp:function:: ~Filesystem()
   
      Destructor - Closes file descriptors and cleans up.

   .. cpp:function:: void initFiles()
   
      Initializes and indexes all files in the configured path.

   .. cpp:function:: void processFileProperties()
   
      Processes and extracts properties for all files.

   .. cpp:function:: FileProperties_t getFilePropertiesByFile(const string &File)
   
      Returns file properties for the specified file path.

   .. cpp:function:: bool checkFileExists(const string &File)
   
      Checks if a file exists in the filesystem.

   .. cpp:function:: string getFileEtag(const string &File)
   
      Returns the ETag for the specified file.

   **Public Members:**

   .. cpp:member:: string Hostname
   
      Virtual hostname for this filesystem.

   .. cpp:member:: string BasePath
   
      Base path for web content.

   .. cpp:member:: string Path
   
      Relative path from base path.

   .. cpp:member:: vector<string> Mimetypes
   
      List of allowed MIME types.

   **Private Members:**

   .. cpp:member:: FilelistPlain_t _Files
   
      List of all file paths.

   .. cpp:member:: FileListExtended_t _FilesExtended
   
      Map of file paths to extended properties.

   .. cpp:member:: string _CompletePath
   
      Complete path (BasePath + Path).


Request Processing Classes
--------------------------

ASProcessHandler
~~~~~~~~~~~~~~~~

Manages Python application server processes. Inherits from ``SHMPythonAS`` and ``CPU``.

.. cpp:class:: ASProcessHandler

   **Type Definitions:**

   .. cpp:type:: typedef struct ASProcessHandlerSHMPointer_t
   
      Structure holding shared memory pointers:
      
      - ``void* PostASMetaPtr`` - AS metadata SHM pointer
      - ``void* PostASRequestsPtr`` - AS requests payload SHM pointer
      - ``void* PostASResultsPtr`` - AS results payload SHM pointer

   **Public Methods:**

   .. cpp:function:: ASProcessHandler()
   
      Constructor - Initializes AS process handler.

   .. cpp:function:: ~ASProcessHandler()
   
      Destructor - Cleans up AS process handler resources.

   .. cpp:function:: void forkProcessASHandler(ASProcessHandlerSHMPointer_t)
   
      Forks Python interpreter processes for application server.

   .. cpp:function:: void setTerminationHandler()
   
      Sets up signal handler for graceful termination.

   .. cpp:function:: void setASProcessHandlerOffsets(VHostOffsetsPrecalc_t)
   
      Sets virtual host memory offsets for AS processes.

   .. cpp:function:: uint getASInterpreterCount()
   
      Returns the number of Python interpreters spawned.

   .. cpp:function:: static void terminate(int)
   
      Static termination handler for AS processes.

   .. cpp:function:: static void registerChildPID(pid_t)
   
      Registers a child AS process PID.

   **Public Members:**

   .. cpp:member:: string ReqPayloadString
   
      Request payload string buffer.

   .. cpp:member:: boost::python::object PyClass
   
      Python class object (when using Python backend).

   **Private Members:**

   .. cpp:member:: VHostOffsetsPrecalc_t _VHostOffsetsPrecalc
   
      Pre-calculated virtual host offsets.


ResultProcessor
~~~~~~~~~~~~~~~

Processes and sends results to clients. Inherits from ``SHMStaticFS``, ``CPU``, ``ResultOrder``, and ``SHMPythonAS``.

.. cpp:class:: ResultProcessor

   **Type Definitions:**

   .. cpp:type:: typedef struct ResultProcessorSHMPointer_t
   
      Structure holding shared memory pointers:
      
      - ``void* StaticFSPtr`` - Static FS SHM pointer
      - ``void* PostASMetaPtr`` - AS metadata SHM pointer
      - ``void* PostASRequestsPtr`` - AS requests SHM pointer
      - ``void* PostASResultsPtr`` - AS results SHM pointer

   **Public Methods:**

   .. cpp:function:: ResultProcessor()
   
      Constructor - Initializes result processor.

   .. cpp:function:: ~ResultProcessor()
   
      Destructor - Cleans up result processor resources.

   .. cpp:function:: pid_t forkProcessResultProcessor(ResultProcessorSHMPointer_t)
   
      Forks the result processor process and returns its PID.

   .. cpp:function:: void setTerminationHandler()
   
      Sets up signal handler for graceful termination.

   .. cpp:function:: void setVHostOffsets(VHostOffsetsPrecalc_t)
   
      Sets virtual host memory offsets.

   .. cpp:function:: static void terminate(int)
   
      Static termination handler.

   **Private Methods:**

   .. cpp:function:: void _processStaticFSRequests(uint16_t)
   
      Processes static filesystem requests from shared memory.

   .. cpp:function:: inline void _parseHTTPBaseProps(string&)
   
      Parses basic HTTP properties from request string.

   .. cpp:function:: uint16_t _processPythonASResults()
   
      Processes Python application server results.

   .. cpp:function:: int _getFDFromParent(uint16_t fd)
   
      Receives file descriptor from parent process via Unix socket.

   **Private Members:**

   .. cpp:member:: pid_t _ForkResult
   
      Process ID of forked result processor.

   .. cpp:member:: int _FDPassingSocketFD
   
      File descriptor for Unix domain socket.

   .. cpp:member:: VHostOffsetsPrecalc_t _VHostOffsetsPrecalc
   
      Pre-calculated virtual host offsets.


Memory Management Classes
-------------------------

MemoryManager
~~~~~~~~~~~~~

Template class for aligned memory management with huge page support.

.. cpp:class:: template<class T> MemoryManager

   **Public Methods:**

   .. cpp:function:: MemoryManager(uint16_t SegmentCount, uint16_t SegmentSize)
   
      Constructor - Allocates aligned memory with huge page support.

   .. cpp:function:: ~MemoryManager()
   
      Destructor - Frees allocated memory.

   .. cpp:function:: T* getNextMemPointer()
   
      Returns pointer to the next memory segment.

   .. cpp:function:: T* getMemBaseAddress()
   
      Returns the base address of allocated memory.

   .. cpp:function:: static constexpr size_t getAlignment()
   
      Returns the alignment requirement for type T.

   .. cpp:function:: static bool isAligned(const void* ptr)
   
      Checks if a pointer is properly aligned for type T.

   **Public Members:**

   .. cpp:member:: static constexpr size_t Alignment
   
      Compile-time alignment requirement for type T.

   **Private Methods:**

   .. cpp:function:: void allocateMemory()
   
      Allocates memory and advises kernel to use huge pages.

   .. cpp:function:: void verifyAlignment()
   
      Verifies memory alignment in debug builds.

   .. cpp:function:: T* getMemPointer(uint16_t SegmentOffset)
   
      Returns pointer to memory at the specified segment offset.

   **Private Members:**

   .. cpp:member:: uint16_t SegmentCount
   
      Number of memory segments.

   .. cpp:member:: uint16_t SegmentSize
   
      Size of each segment.

   .. cpp:member:: uint16_t SegmentOffset
   
      Current segment offset.

   .. cpp:member:: T* MemoryBaseAddress
   
      Base address of allocated memory.


IPC and Shared Memory Classes
-----------------------------

IPCHandler
~~~~~~~~~~

Base class for inter-process communication and shared memory management.

.. cpp:class:: IPCHandler

   Provides shared memory segment management for static filesystem requests.

   **Type Definitions:**

   .. cpp:type:: typedef struct SHMData_t
   
      Shared memory data structure for IPC.

   **Public Methods:**

   .. cpp:function:: IPCHandler()
   
      Constructor - Initializes IPC handler.

   .. cpp:function:: ~IPCHandler()
   
      Destructor - Cleans up IPC resources.

   **Shared Memory Layout:**

   Static FS SHM Segment #1:
   
   - Address 0x00: ``atomic_uint16_t StaticFSLock`` - Lock for static FS access
   - Address 0x02: ``uint16_t RequestCount`` - Number of requests
   - For each request:
     
     - ``uint16_t ClientFD`` - Client file descriptor
     - ``uint16_t HTTPVersion`` - HTTP version
     - ``uint16_t RequestNr`` - Request number
     - ``uint16_t PayloadLength`` - Payload length
     - ``char[] Payload`` - Request payload data


SHMPythonAS
~~~~~~~~~~~

Shared memory handler for Python application server communication.

.. cpp:class:: SHMPythonAS

   Provides shared memory segment management for Python AS requests and results.

   **Shared Memory Layout:**

   AS Metadata SHM Segment #2 (per interpreter):
   
   - ``atomic_uint16_t CanRead`` - Flag indicating request is ready
   - ``atomic_uint16_t WriteReady`` - Flag indicating ready for result
   - ``uint16_t ClientFD`` - Client file descriptor
   - ``uint16_t HTTPVersion`` - HTTP version
   - ``uint16_t HTTPMethod`` - HTTP method
   - ``uint16_t ReqNr`` - Request number
   - ``uint32_t ReqPayloadLen`` - Request payload length
   - ``uint32_t ResPayloadLen`` - Result payload length

   AS Requests Payload SHM Segment #3:
   
   - ``char[] Payload`` - Request payload data (per interpreter segment)

   AS Results Payload SHM Segment #4:
   
   - ``char[] Payload`` - Result payload data (per interpreter segment)


IPCHandlerAS
~~~~~~~~~~~~

IPC handler specific to application server processes.

.. cpp:class:: IPCHandlerAS

   Extends IPC functionality for application server communication.

   **Public Methods:**

   .. cpp:function:: IPCHandlerAS()
   
      Constructor - Initializes AS-specific IPC handler.

   .. cpp:function:: ~IPCHandlerAS()
   
      Destructor - Cleans up AS IPC resources.


Utility Classes
---------------

CPU
~~~

CPU affinity and binding utilities.

.. cpp:class:: CPU

   **Public Methods:**

   .. cpp:function:: void bindToCPU(int core)
   
      Binds the current thread to the specified CPU core.

   .. cpp:function:: void bindToCPUs(std::vector<int> cores)
   
      Binds the current thread to multiple CPU cores.


Vector
~~~~~~

Custom vector implementation with specialized operations.

.. cpp:class:: Vector

   **Public Methods:**

   .. cpp:function:: void multiErase(std::vector<size_t> indices)
   
      Efficiently erases multiple elements by indices.


Helper Classes
~~~~~~~~~~~~~~

Socket
^^^^^^

Socket utility functions.

.. cpp:class:: Socket

   **Public Static Methods:**

   .. cpp:function:: static void makeNonblocking(int fd)
   
      Makes a socket non-blocking by setting O_NONBLOCK flag.


FilesystemHelper
^^^^^^^^^^^^^^^^

Filesystem helper utilities.

.. cpp:class:: FilesystemHelper

   **Public Static Methods:**

   .. cpp:function:: static void GetDirListingByFiletype(vector<string>& FileListRef, const string Path, const string FileType)
   
      Recursively gets all files of a specific type from a directory.


String
^^^^^^

String manipulation utilities.

.. cpp:class:: String

   **Public Static Methods:**

   .. cpp:function:: static void split(string& StringRef, const string Delimiter, vector<string>& ResultRef)
   
      Splits a string by delimiter and stores results in vector.

   .. cpp:function:: static void rsplit(string& String, size_t StartPos, const string Delimiter, vector<string>& ResultRef)
   
      Reverse splits a string from a starting position.

   .. cpp:function:: static void hexout(string& String)
   
      Outputs string content in hexadecimal format for debugging.
