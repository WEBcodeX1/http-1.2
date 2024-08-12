# Static Filesystem Handler

The StaticFSHandler Component manages Filesystem Data / Mime-Types for Static Data Requests.

All Virtual Host dependend file properties will be loaded at startup into static C++ class
member Objects / Structs.

The File Data will be mmapped() into Kernel-Space for sendfile() usage when requested.
If a files size is bigger than 2 Megabytes a huge-page for mmap() will be tried to allocate.

![Info]
> A huge amount of files / subdirs currently is a performance killer. Segmentation on subdirs
must be implemented.

## Program Logic

### Initialization

Loop recursive over files found in Virtual Host dir (from configuration). Add properties to
internal C++ Objects / Structs.

All found files will be mmapped() (read into memory) for sendfile() processing.

### Get Property

When a HTTP request for a Static File arrives, the files properties (e.g. SendfileFD) must
be determined / returned. This happens inside multiple parallel ResultProcessors Threads.

Due to immutable member Objects / Structs a pointer can be used for parallel read access.
