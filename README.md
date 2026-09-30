# NativeGridAccumulator

[![CMake on multiple platforms](https://github.com/OlehKulykov/NativeGridAccumulator/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/OlehKulykov/NativeGridAccumulator/actions/workflows/cmake-multi-platform.yml)
[![Platforms](https://img.shields.io/badge/Platforms-Unix%2FLinux%20%7C%20macOS-brightgreen.svg)](https://en.cppreference.com/)
[![Language](https://img.shields.io/badge/Language-C%2B%2B20%20%2F%20C11-brightgreen.svg)](https://en.cppreference.com/)
[![Telegram integration](https://img.shields.io/badge/Telegram%20integration-deepskyblue.svg)](https://t.me/nativegridaccumulator)

**NativeGridAccumulator** is a lightweight, high-performance C++ UNIX service designed for automated cryptocurrency trading on the [Kraken](https://www.kraken.com/) exchange. 

The bot implements a classical grid/mirror strategy: it monitors currently active buy and sell orders. Once an order is executed, the service automatically creates a mirror order with configurable profit ratios.

---

## 🚀 Key Features

* **Automated Mirror Order Creation**:
  * **On Sell Order Execution** -> Automatically places a **Buy** order for a larger volume at a lower total price.
  * **On Buy Order Execution** -> Automatically places a **Sell** order for a higher total cost based on your profit target.
* **Granular Profit Configuration**: Customize individual profit multipliers (volume and cost rates) per trading pair and per order direction (buy/sell).
* **Exchange Fee Aware**: Automatically incorporates Kraken trading fees (`fee`) into order placement calculations.
* **High Financial Precision**: Powered by `Boost.Decimal` to eliminate standard floating-point (`float`/`double`) rounding errors during price calculations.
* **Telegram Notifications**: Real-time logging and order event notifications delivered straight to your Telegram chat or channel.
* **UNIX Service Ready**: Out-of-the-box daemon support for **systemd**, **OpenRC**, and **macOS Launchd**.
* **Local Order Database**: Persists active/historical order states via an SQLite database.

---

## 🛠 Tech Stack & Dependencies

Built with **C++20** (with C11 components) and managed via **CMake**.

### Dependencies

| Library | Required | Notes / Build Details |
| :--- | :---: | :--- |
| **Boost.Decimal** | **Always Optional** | **Always** fetched from GitHub at build-time to ensure exact precision math. |
| **OpenSSL** | **Yes** | Handles secure HTTPS requests alongside cURL. Must be pre-installed on the host system. |
| **cURL** | **Yes** | Used for Kraken REST API and Telegram HTTP interactions. Must be pre-installed. |
| **libuv** | Optional | Asynchronous I/O. If missing from system, CMake will automatically download and build it from GitHub. |
| **SQLite3** | Optional | Local database for order persistence. Auto-fetched via CMake if not installed. |
| **RapidJSON** | Optional | Fast JSON parsing for config files and API responses. Auto-built via CMake if needed. |

---

## 📦 Building & Installation

### Prerequisites
* C++20 compatible compiler (GCC 10+, Clang 11+, or Apple Clang)
* CMake 3.16+
* System-installed `openssl` and `libcurl` development libraries

### Build Steps

```bash
# Clone the repository
git clone https://github.com/OlehKulykov/NativeGridAccumulator.git
cd NativeGridAccumulator

# Create a build directory
mkdir build && cd build

# Configure and compile
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

---

## ⚙️ Configuration

The service uses a single JSON configuration file to manage API keys, polling intervals, and trading pair rules.

Both Kraken's ```pair-decimals``` and ```lot-decimals``` values for ```ETHUSDC``` pair can be found following [Market Data / Get Tradable Asset Pairs](https://docs.kraken.com/api-reference/market-data/get-tradable-asset-pairs/) or buy using **nga-u** utility application.

### Example `config.json`
```json
{
    "kraken": {
        "api-key": "<KRAKEN API KEY>",
        "private-key": "<KRAKEN API KEY PRIVATE>",
        "check-orders-tick-range": [20000, 30000],
        "update-ask-bid-tick-range": [60000, 120000],
        "update-orders-info-tick-range": [180000, 300000],
        "orders-db-file": "/var/lib/nga/krkn.orders.sqlite",
        "order-settings": {
            "ETHUSDC": {                       // https://api.kraken.com/0/public/AssetPairs?info=info&pair=ETHUSDC
                "sell-volume-rate": "0.995",   // -0.5% ETH volume (99.5%)
                "sell-cost-rate":   "1.005",   // +0.5% USDC cost (100.5%)
                "buy-volume-rate":  "1.005",   // +0.5% ETH volume (100.5%)
                "buy-cost-rate":    "0.995",   // -0.5% USDC cost (99.5%)
                "price-step":       "0.01",    // Minimum price increment (+/-)
                "fee":              "0.4",     // Trading fee (%)
                "pair-decimals":    2,         // Pair decimals (*_USDC)
                "lot-decimals":     8,         // Lot decimals (ETH_*)
                "enabled":          true
            }
            // , "<MY_FAVOURITE_PAIR>": { ... }
        }
    },

    "telegram": {
        "api-key": "<TELEGRAM API BOT KEY>",
        "chat-id": "@<YOUR_CHAT_OR_CHANNEL_ID>"
    },

    "log-file": "/var/log/nga/service.log",
    "pid-file": "/var/run/nga.pid"
}

```

### Rate Multiplier Logic (`order-settings`)

* **When a Sell order closes (ETH/USDC)**:
  * Generates a Buy order increasing the target `ETH` amount by +0.5% (`"buy-volume-rate": "1.005"`) and lowering total `USDC` cost by -0.5% (`"buy-cost-rate": "0.995"`).

* **When a Buy order closes (ETH/USDC)**:
  * Generates a Sell order reducing sold `ETH` amount by -0.5% (`"sell-volume-rate": "0.995"`) and increasing total `USDC` proceeds by +0.5% (`"sell-cost-rate": "1.005"`).

---

## 🚦 Service Management

`NativeGridAccumulator` runs natively as a background service across UNIX environments:

* **OpenRC (Gentoo)**: Deploy the init script to `/etc/init.d/nga`.
* **systemd (Linux)**: Copy your unit service file to `/etc/systemd/system/` and run `systemctl enable --now nga`.
* **macOS (Launchd)**: Load via standard `.plist` agent config in `~/Library/LaunchAgents/`.

### Example Gentoo(OpenRC) `/etc/init.d/nga.json`
```bash
#!/sbin/openrc-run

command="nga-d"
pidfile="/run/nga.pid"
command_args="-c /<PATH_TO_CONFIG>/config.json"
command_background=true

depend() {
    need net
    use logger
}
```
---

## 📄 License
Copyright (C) 2025 - 2026 Oleh Kulykov <olehkulykov@gmail.com>

All Rights Reserved.

Unauthorized copying of this file, transferring or reproduction of the
contents of this project, via any medium, is strictly prohibited.
The contents of this project are proprietary and confidential.
