MIT License

Copyright (c) 2025 Scott Arsenault

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---

# BFGP - Barebones Framework for Game Prototyping

**Author**: Scott Arsenault

**License**: MIT

## 🚀 Overview
BFGP is a high-performance, tile-based game engine prototype written in C++ using SDL2, EnTT, and a TMX map parser. It’s designed as a flexible foundation for building roguelikes, tactical RPGs, or simulation games with modding support and a fast, terminal-driven workflow.

## 🔧 Features
- ✅ TMX map loading with multiple layers, tilesets, and objects
- ✅ Integrated hot-reloading system for maps
- ✅ Turn- and tick-based scheduler system using EnTT
- ✅ Real-time and turn-based ready
- ✅ Modular architecture for clean expansion

## 🛠 Getting Started

### Requirements
- CMake 3.16+
- SDL2, SDL2_image, SDL2_ttf, SDL2_mixer
- TmxParser (https://github.com/libgdx/tiled/tree/master/tools/libtmx)
- A C++20-capable compiler (Clang or GCC recommended)

### Build
```bash
git clone https://github.com/YOURNAME/bfgp.git
cd bfgp
cmake -B build
cmake --build build -j$(nproc)
./build/scheduler_demo
```

### Example
When you run the demo, it:
- Loads a `.tmx` map
- Prints tile and layer info
- Hot-reloads maps as you edit and save in Tiled
- Moves a player character using SDL input

## 🧠 Philosophy
This project emphasizes clarity and low overhead. It’s meant to be dissected, hacked, and evolved — a kind of spiritual successor to Zachtronics design ethos, tailored for procedural storytelling.

## 📁 Project Layout
```
├── assets/                # Your TMX maps and tilesets
├── include/              # EnTT and Tmx headers
├── src/                  # Core engine source files
├── main.cpp              # Entry point with demo usage
├── CMakeLists.txt        # Build configuration
└── LICENSE               # MIT license
```

## 🤝 Contributing
This is currently a solo dev project. Fork away, mod it, ship it — and let me know if you build something rad!

---
Let’s prototype some weird, beautiful shit.
—Scott


