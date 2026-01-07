# Design Document – Custom HTTP/HTTPS Proxy Server

## 1. Overview
This document describes the design and architecture of a custom HTTP/HTTPS proxy server implemented in **C++ using Winsock (Windows)**.The proxy supports HTTP request forwarding,domain-based filtering,logging with rotation,basic metrics collection, concurrent client handling,and HTTPS tunneling via the CONNECT method.


---

## 2. High-Level Architecture

### 2.1Component Diagram

```
Client (curl / browser)
        |
        v
+-------------------+
|   Proxy Server    |
|-------------------|
|  Listener Socket  |
|  (proxy.cpp)      |
+-------------------+
        |
        v
+-------------------+
| Request Handler   |  (thread-per-connection)
+-------------------+
   |        |        |
   v        v        v
Parser   Filter   Logger
(parser) (filter) (logger)
   |
   v
Forwarder / CONNECT Tunnel
(forwarder.cpp)
   |
   v
Destination Server
```

---

## 3.Component Descriptions

### 3.1`proxy.cpp`–Core Server

* Creates and binds a listening TCP socket
* Accepts incoming client connections
* Spawns a **new thread per connection**
* Coordinates parsing, filtering, forwarding, and logging
* Applies socket timeouts for stability

Responsibilities:

* Connection lifecycle management
* Concurrency handling
* High-level request orchestration

---

### 3.2`parser.cpp / parser.h` – HTTP Request Parsing

Parses raw HTTP requests to extract:

* HTTP method (GET, POST, CONNECT)
* Request target (absolute URI or relative path)
* HTTP version
* Host and port
* Headers (Content-Type, Content-Length)
* Request body (if present)

Supports:

* Absolute-form proxy requests (e.g., `GET http://example.com/path HTTP/1.1`)
* Relative-form requests with `Host` header

---

### 3.3 `filter.cpp / filter.h` – Domain Filtering

* Loads blocked domains/IPs from `config/blocked_domains.txt`
* Canonicalizes hostnames (lowercase normalization)
* Performs match checks before forwarding

If a request is blocked:

* Returns `HTTP/1.1 403 Forbidden`
* Logs the blocked event

---

### 3.4 `forwarder.cpp / forwarder.h` – Request Forwarding

Handles two forwarding modes:

#### HTTP Forwarding

* Opens a TCP connection to destination server
* Forwards full HTTP request (headers + body)
* Streams server response back to client
* Does not buffer entire responses in memory

#### HTTPS Tunneling (CONNECT)

* Establishes TCP connection to target host:port
* Responds with `200 Connection Established`
* Relays encrypted bytes bidirectionally using `select()`

---

### 3.5 `logger.cpp / logger.h` – Logging and Metrics

Logging:

* Timestamp
* Client IP:port
* Requested host:port
* Request line
* Action (ALLOWED / BLOCKED)
* Response status code
* Bytes transferred

Log Management:

* Bounded log file size
* Old log data automatically discarded when size limit is exceeded

Metrics (optional demonstration):

* Total request count
* Per-host request frequency

---

### 3.6 `config.h / config.cpp` – Configuration

* Provides configurable bind address and port
* Centralizes runtime configuration

---

## 4. Concurrency Model

### Model Used: **Thread-per-Connection**

Each incoming client connection is handled in a **separate detached `std::thread`**.

#### Rationale:

* Simple and easy to implement
* Suitable for prototyping and moderate workloads
* Clear mapping between client and execution context

#### Trade-offs:

* Higher memory usage for large numbers of clients
* Not as scalable as event-driven models (e.g., epoll)

---

## 5. Data Flow Description

### 5.1 HTTP Request Flow

1. Client connects to proxy
2. Proxy accepts connection
3. Raw request received via `recv()`
4. Request parsed by `parser`
5. Host checked by `filter`
6. Request logged by `logger`
7. If allowed:

   * Forwarded to destination server
   * Response streamed back to client
8. Connection closed

---

### 5.2 HTTPS CONNECT Flow

1. Client sends `CONNECT host:port HTTP/1.1`
2. Proxy checks filter rules
3. TCP connection opened to host:port
4. Proxy responds `200 Connection Established`
5. Encrypted data tunneled bidirectionally
6. Tunnel closes on socket termination
---
## 6. Error Handling Strategy
* Handles partial reads/writes using loops
* Socket timeouts prevent hanging connections
* All sockets closed on error paths
* DNS resolution failures handled gracefully
* Invalid or blocked requests return proper HTTP errors
---
## 8. Conclusion

The proxy server successfully demonstrates reliable TCP communication, HTTP parsing and forwarding, domain filtering, logging, concurrency handling, and HTTPS tunneling. The design emphasizes clarity, modularity, and correctness, making it suitable for academic evaluation and further extension.
