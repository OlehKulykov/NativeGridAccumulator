# NativeGrid

**NativeGrid** is a high-performance, deterministic trading engine engineered in C++. It is purpose-built for bare-metal execution on Linux (Gentoo) environments, specifically optimized for AMD Ryzen (Zen) architectures.

The system automates market-making and liquidity accumulation strategies through a high-frequency recursive order reversal algorithm on the Kraken exchange.

[![Platform](https://img.shields.io/badge/Platform-Gentoo%20Linux-purple.svg)](https://www.gentoo.org/)
[![Architecture](https://img.shields.io/badge/Arch-AMD%20Ryzen%20%2F%20Zen-orange.svg)](https://www.amd.com/)

---

## 🚀 Core Concept

NativeGrid operates on a **Deterministic Inventory Management** model. Unlike speculative bots, NativeGrid monetizes market volatility rather than direction:

1.  **Source of Truth Synchronization:** Upon initialization, the engine synchronizes state directly with the Kraken API. Local SQLite storage is utilized exclusively for high-speed auditing and historical analysis.
2.  **Recursive Reversal Loop:** When an order is filled (e.g., *Buy*), the engine instantly calculates and executes an inverse order (*Sell*) based on:
    * Real-time Order Book depth (Best Bid/Ask).
    * Exchange-specific Fee structures.
    * Target Yield parameters (e.g., +0.5% asset accumulation and +1% currency profit).
3.  **Inventory Protection:** If the market moves beyond defined thresholds, the system enters an "Inventory Hold" state, preserving capital and waiting for mean reversion or manual intervention.

## 🛠 Technology Stack

Engineered for ultra-low latency and maximum reliability:

* **Core Engine:** C++ (utilizing `std::jthread`, asynchronous event loops, and stack-allocated static buffers).
* **Networking Layer:** `Boost.Beast` + `Boost.Asio`. Handles both the Kraken WebSocket/REST client and the integrated HTTP Management Server.
* **Numerical Precision:** `Boost.Multiprecision` (utilizing `cpp_dec_float_50` to eliminate floating-point rounding errors inherent in standard `double` types).
* **Persistence:** `SQLite3`. High-speed local indexing of every order state, trade execution, and audit log.
* **Environment:** Optimized for **Gentoo Linux** (Open-RC). Compiled with `-march=native -O3` flags to leverage specific Zen-core CPU instructions.

## 💎 Key Features

* **Binary Autonomy:** A "Fat Binary" approach. The Web UI and management tools are embedded directly into the executable as compressed static resources.
* **Zero Garbage Architecture:** Minimized dynamic memory allocations in critical execution paths to prevent latency spikes.
* **Security & Privacy:** Fully autonomous logic. API keys and sensitive trade data never leave your dedicated hardware.
* **Hardware-Locked Logic:** Support for CPUID-based binding to prevent unauthorized binary redistribution.

## 🏗 Project Structure

```text
NativeGrid/
├── core/                # [PRIVATE] Proprietary reversal logic & strategy engine
├── api/                 # C++ wrappers for Kraken REST/WebSockets
├── db/                  # SQLite abstraction layer for Audit Trails
├── web/                 # Embedded Boost.Beast HTTP Server & Web UI
└── util/                # Performance helpers, string obfuscation, and config parsers
