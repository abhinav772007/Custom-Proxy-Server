# Test Artifacts – Custom Network Proxy Server

This files describes the test commands, scripts, and sample outputs used to validate the functionality of the custom HTTP/HTTPS proxy server.

---

## 1. Environment Setup

* **OS:** Windows 10 / 11
* **Compiler:** MinGW-w64 (g++ with C++17)
* **Proxy Port:** 8888
* **Proxy Address:** 0.0.0.0 / localhost

### Compilation

Using MinGW-w64's terminal:

```bash
g++ -std=c++17 -Iinclude src/proxy.cpp src/parser.cpp src/filter.cpp src/logger.cpp src/forwarder.cpp src/config.cpp -o proxy.exe -lws2_32
```

Start the proxy server:


```bash
./proxy.exe
```

Expected output:

```
Proxy listening on 0.0.0.0 :8888
```

---

## 2. Successful HTTP Forwarding Test

### Command

```bash
curl.exe -x http://localhost:8888 http://example.com
```

### Expected Result

* HTTP response returned successfully
* Status: **200 OK**
* HTML content of example.com displayed

---

## 3. Header-Only (HEAD) Request Test

### Command

```bash
curl.exe -I -x http://localhost:8888 http://example.com
```

### Expected Result

* Headers returned without body
* Status: **200 OK**


---

## 4. Blocking Test (Blacklisted Domain)

### Configuration

Edit `config/blocked_domains.txt`:

```
example.com
```

Restart proxy server after editing.

### Command

```bash
curl.exe -v -x http://localhost:8888 http://example.com
```
Now lets test sub-domains 
```bash
curl.exe -v -x http://localhost:8888 http://sub.example.com
```
### Expected Result

```
HTTP/1.1 403 Forbidden
Content-Length: 0
Connection: close
```

---

## 5. HTTPS CONNECT Tunneling Test

### Command

```bash
curl.exe -v -x http://localhost:8888 https://example.com
```

### Expected Result

* CONNECT request sent to proxy
* `HTTP/1.1 200 Connection Established`
* TLS handshake succeeds
* HTTPS page loads correctly

---

## 6. Concurrent Client Test

### PowerShell Script

```powershell
for ($i=0; $i -lt 5; $i++) {
    Start-Process powershell -ArgumentList "curl.exe -x http://localhost:8888 http://example.com"
}
```

### Expected Result

* Multiple requests handled simultaneously
* No crashes or deadlocks

---

## 7. Malformed Request Test

### Command

```bash
echo "BADREQUEST" | nc localhost 8888
```

### Expected Result

* Connection closed gracefully
* No proxy crash


---

## 8. Log Verification

### Command

```bash
notepad logs\\proxy.log
```

### Sample Log Entries

```
2026-01-04 11:23:01 | 127.0.0.1:51342 | example.com:80 | GET / HTTP/1.1 | ALLOWED | 200 | 513
2026-01-04 11:24:10 | 127.0.0.1:51401 | example.com:80 | GET / HTTP/1.1 | BLOCKED | 403 | 0
```

### Purpose

* Confirms logging format
* Confirms blocked and allowed actions recorded
* Confirms bounded log behavior under load

---

## 9. Summary

The above tests collectively demonstrate:

* Correct HTTP and HTTPS proxying
* Domain-based filtering
* Concurrent client handling
* Robust error handling
* Accurate logging and metrics support

These artifacts were used during the demo video and validation of the proxy server.
