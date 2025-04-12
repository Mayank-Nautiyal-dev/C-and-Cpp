## Advanced Computer Fundamentals – Technical Overview

#  What is a Computer?
A computer is a programmable electronic system that takes raw input (data), processes it using a defined set of instructions (program), and produces meaningful output. It’s composed of three main domains:

Computation (Processing Units)

Memory & Storage

Networking

## A. Computation
### A.1 CPU – Central Processing Unit
The CPU executes instructions and coordinates activities between all other components. It performs the fetch-decode-execute cycle for processing machine instructions.

 Example Instruction Flow (Pseudocode):

## assembly 
```
Copy
Edit
MOV AX, 01h     ; Load value into register
ADD AX, 02h     ; Add value to register
```
## A.2 DPU – Data Processing Unit
DPUs handle data-centric operations like encryption, compression, and packet parsing. Used in cloud computing, data centers, and zero-trust networking.

 Example Use Case: Offloading SSL/TLS encryption from CPU to DPU for performance optimization.

## A.3 GPU – Graphics Processing Unit
GPUs are massively parallel processors for vectorized math operations. Extensively used in:

> Graphics rendering

> Deep Learning

> Video encoding/decoding

 Example: Python with TensorFlow (GPU Training)

python
```
import tensorflow as tf

with tf.device('/GPU:0'):
    a = tf.constant([1.0, 2.0])
    b = tf.constant([3.0, 4.0])
    result = tf.add(a, b)
```
## A.4 APU – Accelerated Processing Unit
Combines CPU + GPU on a single chip. Ideal for ultrabooks and integrated graphics scenarios (e.g., AMD Ryzen APUs).

 Example Use Case: Lightweight gaming or video editing without a discrete GPU.

## A.5 NPU – Neural Processing Unit
Dedicated AI chips for neural inference and edge AI (e.g., smartphone camera recognition, voice assistants).

 Used in: Huawei Kirin chips, Apple Neural Engine.

## A.6 TPU – Tensor Processing Unit
Designed by Google, optimized for matrix operations (used in deep learning models).

### Use Case: Training large-scale models like BERT, GPT, etc.

## A.7 FPGA – Field-Programmable Gate Array
Reconfigurable ICs tailored for specific logic flows.

 Use Case: Real-time analytics in high-frequency trading.

 Example Logic in Verilog:

## verilog
```
always @(posedge clk)
  if (reset)
    counter <= 0;
  else
    counter <= counter + 1;
```
A.8 ASIC – Application-Specific Integrated Circuit
Hardwired chips designed for a single task (e.g., Bitcoin mining via SHA-256).

 Use Case: ASICs used in Bitmain AntMiner for cryptocurrency.

A.9 CISC vs. A.10 RISC
CISC: Complex, fewer lines of code (e.g., Intel x86)

RISC: Simple instructions, faster execution (e.g., ARM)

 Example:

assembly
```
; CISC
MULT AX, BX

; RISC
LOAD R1, A
LOAD R2, B
MUL R3, R1, R2
```
A.11 RISC-V
An open-source instruction set enabling customizable processors.

 Example: Used in low-power IoT devices or academic CPUs.

A.12 SoC – System on Chip
Single-chip solutions including CPU, GPU, memory controllers, and I/O. Examples:

Snapdragon (Qualcomm)

Apple M1/M2

 B. Memory and Storage
B.1 Primary Memory
RAM: Temporary, volatile.

Cache: CPU-integrated fast memory (L1, L2, L3).

 Example:

c
```
int main() {
    int arr[5] = {1, 2, 3, 4, 5}; // Stored in RAM during runtime
    return 0;
}
```
B.2 Secondary Storage
Non-volatile storage:

HDD, SSD, NVMe

Use case: Permanent storage for files, OS, and media.

 Example with Python File Write:

python
```
with open("data.txt", "w") as f:
    f.write("Stored on secondary storage")
```
 C. Networking
# C.1 Internet
Global network using TCP/IP protocols. 
## ip-provides every device on internet a pair of ip, one public and one private
> private ip for internal communication inside a network and public ip for conntecting to isp or a server or internet
![image](https://imgs.search.brave.com/SzbsKyPBnQvdBZKq_FKgBoZ9tStT4vELCYTgh4zQ4Ew/rs:fit:860:0:0:0/g:ce/aHR0cHM6Ly9jaXZv/LWNvbS1hc3NldHMu/YW1zMy5kaWdpdGFs/b2NlYW5zcGFjZXMu/Y29tL2NvbnRlbnRf/aW1hZ2VzLzIxNzYu/YmxvZy5QTkc_MTY3/NDU3NTM2Ng)
## tcp is a more secure way of communication which ensures that first the connection is built properly and both nodes are ready to recive data ,before actual tranmisson through a three way handshake 
![image](https://imgs.search.brave.com/sE72KWZjFFDIJ62ccPAmxZQHoKIzfoPQwlHVBu5FsXM/rs:fit:860:0:0:0/g:ce/aHR0cHM6Ly9hZnRl/cmFjYWRlbXkuY29t/L2ltYWdlcy93aGF0/LWlzLWEtdGNwLTMt/d2F5LWhhbmRzaGFr/ZS1wcm9jZXNzLXRo/cmVlLXdheS1oYW5k/c2hha2luZy1lc3Rh/Ymxpc2hpbmctY29u/bmVjdGlvbi02YTcy/NGU3N2JhOTZlMjQx/LmpwZw)

## the tcp packet looks like this ![image](https://imgs.search.brave.com/z_YymkG1-UphJjXHMPrdUsiPpDP87Yzbg18vF9Y-Fsg/rs:fit:860:0:0:0/g:ce/aHR0cHM6Ly9pbWFn/ZXMudHBvaW50dGVj/aC5jb20vdHV0b3Jp/YWwvY29tcHV0ZXIt/bmV0d29yay9pbWFn/ZXMvdGNwMy5wbmc)
C.2 DNS
Maps domain names to IP addresses.

 Example:

bash
```
nslookup openai.com
```
C.3 IP Addressing
IPv4: 32-bit, e.g., 192.168.1.1

IPv6: 128-bit, e.g., 2001:0db8::1

 Private Ranges:

192.168.x.x

10.x.x.x

172.16.x.x – 172.31.x.x

C.4 MAC Address
Unique NIC identifier (e.g., 00:1A:2B:3C:4D:5E)

C.5 Ports
Used to route traffic to applications.

Example:

HTTP → Port 80

SSH → Port 22

C.6 OSI Model
markdown
Copy
Edit
1. Physical      - Cables, connectors
2. Data Link     - Ethernet, MAC
3. Network       - IP, ICMP
4. Transport     - TCP, UDP
5. Session       - API sessions
6. Presentation  - Encryption, compression
7. Application   - HTTP, FTP
 Blockchain
Distributed ledger system with transparency and immutability.

 Example:

json
```
{
  "block": 1,
  "timestamp": "2025-04-12",
  "data": "Transaction Data",
  "hash": "0000ab34..."
}
```
 Operating System (OS)
Manages hardware, provides environment for software.

 Examples:

Windows

macOS

Linux

Kernel
Handles system calls, device drivers, and process management.

Bare Metal
Running workloads directly on hardware without virtualization. Often used in performance-intensive tasks.

 Cloud Computing
  Types:

> ## Private: OpenStack

> ## Public: AWS, Azure, GCP

> ## Hybrid: Mix of both

VM – Virtual Machine
Software emulation of physical hardware.

 Tools: VirtualBox, VMware, KVM

 Algorithm
Finite steps to solve a problem.

 Example: Python Bubble Sort

python
```t
def bubble_sort(arr):
    for i in range(len(arr)):
        for j in range(0, len(arr)-i-1):
            if arr[j] > arr[j+1]:
                arr[j], arr[j+1] = arr[j+1], arr[j]
```
 IPv6
128-bit address format to handle IP exhaustion.

 Format: 2001:0db8:85a3:0000:0000:8a2e:0370:7334

 Kubernetes
Container orchestration platform.

 Commands:

bash
```
kubectl get pods
kubectl apply -f deployment.yaml
```
 API (Application Programming Interface)
Bridge between frontend and backend.

 Example: REST API in Flask (Python)

python
```
from flask import Flask
app = Flask(__name__)

@app.route("/api/v1/status")
def status():
    return {"status": "running"}

app.run()
```
 Frontend
Technologies: HTML, CSS, JavaScript

 Example:

html
```
<button onclick="alert('Hello!')">Click Me</button>
```
 Backend
Frameworks: Django, Flask, Node.js

 Example (Django View):

python
```
def home(request):
    return HttpResponse("Welcome to the backend!")
```
 Binary and Number Systems
Binary: 0s and 1s → Base-2

2^10 = 1 KB, 2^20 = 1 MB, 2^30 = 1 GB

IPv4 Address: 32-bit → 4 octets

 DevOps
Practice of integrating development + operations.

 Tools:

CI/CD: Jenkins, GitHub Actions

IaC: Terraform, Ansible

Containers: Docker, Kubernetes

 Example CI/CD YAML (GitHub Actions):

yaml
```
name: Build and Deploy

on: [push]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - run: echo "Deploying..."
```
