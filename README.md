# Custom HTTP/HTTPS Proxy Server

A modular, multi-threaded HTTP/HTTPS proxy server implemented in C++ with support for request forwarding, domain blocking, logging, and HTTPS tunneling.

## Features

### Core Functionality
- TCP socket-based client-server communication
- HTTP request parsing (GET, POST, HEAD, CONNECT)
- Forwarding of HTTP requests and responses
- Streaming response relay (no full buffering)

### Concurrency
- Thread-per-connection model using std::thread
- Supports multiple simultaneous clients

### Filtering
- Domain-based blocking using configuration file
- Canonicalization (lowercase normalization)
- Blocks subdomains implicitly (e.g., example.com blocks www.example.com)

### Logging
- Comprehensive request logging with:
  - Timestamp
  - Client IP:port
  - Requested host:port
  - Request line
  - Action (ALLOWED / BLOCKED)
  - HTTP status
  - Bytes transferred
- Automatic log directory creation
- Bounded log file size (keeps latest entries)

### HTTPS Support
- CONNECT method tunneling
- Full bidirectional byte relay
- No TLS interception (transparent tunnel)

## Project Structure

```
Custom-Proxy-Server/
├── src/
│   ├── proxy.cpp        # Main server loop
│   ├── parser.cpp       # HTTP request parsing
│   ├── forwarder.cpp    # Request forwarding + CONNECT tunnel
│   ├── filter.cpp       # Domain filtering logic
│   ├── logger.cpp       # Logging and metrics
│   └── config.cpp       # Configuration loader
├── include/
│   ├── parser.h
│   ├── forwarder.h
│   ├── filter.h
│   ├── logger.h
│   └── config.h
├── config/
│   ├── proxy.conf       # Server configuration
│   └── blocked_domains.txt  # Blocked domains list
├── test/
│   ├── test_logger.cpp
│   └── test_filter.cpp
├── logs/
│   └── proxy.log        # Log file (auto-created)
├── Makefile             # Build configuration
└── README.md            # This file
```

## Building

### Prerequisites
- C++17 compatible compiler (g++, clang++, or MSVC)
- Make (optional, or compile manually)
- On Windows: MinGW-w64 or Visual Studio

### Windows (MSYS2 MinGW64)

```bash
g++ -std=c++17 -Iinclude \
  src/proxy.cpp src/parser.cpp src/filter.cpp \
  src/logger.cpp src/forwarder.cpp src/config.cpp \
  -o proxy.exe -lws2_32
```

### Linux/Unix

```bash
g++ -std=c++17 -Iinclude \
  src/proxy.cpp src/parser.cpp src/filter.cpp \
  src/logger.cpp src/forwarder.cpp src/config.cpp \
  -o proxy -pthread
```

### Using Makefile

```bash
make
```

## Running

```bash
./proxy.exe    # Windows
./proxy        # Linux/Unix
```

Expected output:
```
Proxy listening on 0.0.0.0:8888
```

## Configuration

### Blocked Domains

Edit `config/blocked_domains.txt`:

```
# Blocked domains
example.com
badsite.org
```

All requests to these domains (and subdomains) will return 403 Forbidden.

### Proxy Settings

Configured via `config/proxy.conf` or in `config.h`:

- Bind address (default: 0.0.0.0)
- Port (default: 8888)

## Usage Examples

### HTTP Request

```bash
curl -x http://localhost:8888 http://example.com
```

### HEAD Request

```bash
curl -I -x http://localhost:8888 http://example.com
```

### HTTPS (CONNECT Tunnel)

```bash
curl -v -x http://localhost:8888 https://example.com
```

### Blocking Test

1. Add `example.com` to `config/blocked_domains.txt`
2. Test with:
   ```bash
   curl -v -x http://localhost:8888 http://example.com
   ```
3. Expected: `HTTP/1.1 403 Forbidden`

### Concurrent Test

**Windows PowerShell:**
```powershell
for ($i=0; $i -lt 20; $i++) {
  Start-Process curl.exe "-x http://localhost:8888 http://example.com"
}
```

**Linux/Unix:**
```bash
for i in {1..20}; do
  curl -x http://localhost:8888 http://example.com &
done
wait
```

## Logging

Logs are written to `logs/proxy.log`. When the file exceeds the maximum size, older entries are discarded and only the latest logs are retained.

### Example Log Entry

```
2026-01-04 11:45:23 | 127.0.0.1:53422 | example.com:80 | GET / HTTP/1.1 | ALLOWED | 200 | 512 bytes
```

### View Logs

```bash
# View entire log
cat logs/proxy.log

# View in real-time
tail -f logs/proxy.log    # Linux/Unix
Get-Content logs/proxy.log -Wait    # Windows PowerShell
```

## Design Decisions

- **Thread-per-connection**: Chosen for simplicity and clarity
- **Streaming forwarding**: Avoids large memory usage
- **Transparent HTTPS tunneling**: Ensures security
- **Modular design**: Easy to extend

## Limitations

- No caching
- No authentication
- No chunked-encoding parsing (forwarded transparently)
- No rate limiting
- No graceful shutdown signal handling

## Testing

Run test files:

```bash
# Compile and run tests
g++ -std=c++17 -Iinclude test/test_logger.cpp src/logger.cpp -o test_logger
./test_logger

g++ -std=c++17 -Iinclude test/test_forwarder.cpp src/forwarder.cpp -o test_forwarder
./test_forwarder

g++ -std=c++17 -Iinclude test/test_filter.cpp src/filter.cpp -o test_filter
./test_filter
```

## Future Enhancements

- Thread pool implementation
- Event-driven I/O (select / poll)
- HTTP response caching
- Access control / authentication
- Live metrics endpoint
- Graceful shutdown handling

## Troubleshooting

### Port Already in Use

Change the port in `config.h` or `config/proxy.conf`:
```cpp
const int PROXY_PORT = 8889;
```

### Permission Denied (Linux)

On Linux, ports below 1024 require root privileges. Use a port >= 1024 or run with sudo.

### Connection Refused

Ensure the proxy server is running and listening on the correct address/port.

