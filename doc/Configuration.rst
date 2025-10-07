Server Configuration File
=========================

The server configuration is stored in ``/config/config.json`` and uses JSON format.

Configuration Structure
-----------------------

The configuration file has the following top-level sections:

* ``global`` - Global server settings
* ``server`` - Server-specific settings
* ``cpu-binding`` - CPU core binding configuration
* ``namespaces`` - Application namespace definitions

Global Section
--------------

global.path
~~~~~~~~~~~

Base path settings for the server.

**global.path.base**
  Base directory for web content.
  
  * Type: ``string``
  * Example: ``"/var/www"``

global.tcp
~~~~~~~~~~

TCP connection settings.

**global.tcp.cork**
  Enable/disable TCP cork option.
  
  * Type: ``boolean``
  * Default: ``false``

**global.tcp.nodelay**
  Enable/disable TCP_NODELAY option (Nagle's algorithm).
  
  * Type: ``boolean``
  * Default: ``true``

Server Section
--------------

server.runas
~~~~~~~~~~~~

User and group settings for dropping privileges after startup.

**server.runas.user**
  Unix user name to run the server as.
  
  * Type: ``string``
  * Example: ``"x0-http"``

**server.runas.group**
  Unix group name to run the server as.
  
  * Type: ``string``
  * Example: ``"x0-http"``

server.connection
~~~~~~~~~~~~~~~~~

Connection and network settings.

**server.connection.timeout**
  Connection timeout in seconds.
  
  * Type: ``string`` (numeric value)
  * Example: ``"600"``

**server.connection.ipv4.address**
  IPv4 address to bind the server to.
  
  * Type: ``string``
  * Example: ``"127.0.0.1"``
  * Default: ``"127.0.0.1"``

**server.connection.ipv4.port**
  Port number to listen on.
  
  * Type: ``integer``
  * Example: ``80``
  * Default: ``80``

server.mimetypes
~~~~~~~~~~~~~~~~

List of supported MIME types for static file serving.

**server.mimetypes**
  Array of file extensions that are allowed to be served.
  
  * Type: ``array`` of ``string``
  * Example: ``["html", "js", "json", "css", "png", "jpg", "svg", "woff2", "ico"]``

CPU Binding Section
-------------------

CPU core binding configuration for different server components.

**cpu-binding.main-server**
  CPU cores assigned to the main server process.
  
  * Type: ``array`` of ``integer``
  * Example: ``[0]``

**cpu-binding.staticfs**
  CPU cores assigned to static file serving processes.
  
  * Type: ``array`` of ``integer``
  * Example: ``[1]``

**cpu-binding.app-server**
  CPU cores assigned to application server processes.
  
  * Type: ``array`` of ``integer``
  * Example: ``[2, 3]``

Namespaces Section
------------------

Application namespaces define virtual hosts and application server configurations.

Each namespace is an object in the ``namespaces`` array with the following properties:

**id**
  Unique identifier for the namespace.
  
  * Type: ``string``
  * Example: ``"app1"``

**hostname**
  Virtual hostname for this namespace.
  
  * Type: ``string``
  * Example: ``"testapp1.local"``

**path**
  File system path to the application.
  
  * Type: ``string``
  * Example: ``"/app1"``

**interpreters**
  Number of Python interpreter processes to spawn for this namespace.
  
  * Type: ``integer``
  * Example: ``5``

**cache-control**
  Cache control headers for responses.
  
  * Type: ``object``

**cache-control.type**
  Cache type (public or private).
  
  * Type: ``string``
  * Values: ``"public"``, ``"private"``
  * Example: ``"private"``

**cache-control.max-age**
  Maximum age in seconds for cached content.
  
  * Type: ``integer``
  * Example: ``3600``

**access**
  Access control and routing configuration.
  
  * Type: ``object``

**access.as-post**
  POST request routing configuration.
  
  * Type: ``object``
  * Maps URL paths to Python methods or ``false`` to deny access
  * Example: ``{"/": "default", "/endpoint": false}``

**access.as-get**
  GET request routing configuration.
  
  * Type: ``object``
  * Can map to simple method name or object with params
  * Example: ``{"/endpoint": {"params": ["param1", "param2"], "method": "method_name"}}``

Example Configuration
---------------------

.. code-block:: json

   {
       "global": {
           "path": {
               "base": "/var/www"
           },
           "tcp": {
               "cork": false,
               "nodelay": true
           }
       },
       "server": {
           "runas": {
               "user": "x0-http",
               "group": "x0-http"
           },
           "connection": {
               "timeout": "600",
               "ipv4": {
                   "address": "127.0.0.1",
                   "port": 80
               }
           },
           "mimetypes": [
               "html", "js", "json", "css", "png", "jpg", "svg", "woff2", "ico"
           ]
       },
       "cpu-binding": {
           "main-server": [0],
           "staticfs": [1],
           "app-server": [2, 3]
       },
       "namespaces": [
           {
               "id": "app1",
               "hostname": "testapp1.local",
               "path": "/app1",
               "interpreters": 5,
               "cache-control": {
                   "type": "private",
                   "max-age": 3600
               },
               "access": {
                   "as-post": {
                       "/": "default"
                   }
               }
           }
       ]
   }
