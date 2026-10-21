# C++ Stack Allocator (Marker-Based Arena Allocator)

A lightweight, high-performance **Stack Memory Allocator** written in C++11/C++17. This project extends the linear allocator concept by adding **LIFO (Last-In, First-Out) scoped tracking**, allowing efficient partial memory rollbacks using markers.

---

## 📌 Overview

The **Stack Allocator** pre-allocates a contiguous block of heap memory and handles sequential allocations with custom bitwise alignment. Unlike a standard linear allocator, it introduces **Markers** (`GetMarker` and `FreeToMarker`), enabling developers to clear temporary allocations instantly without resetting the entire buffer.

### Key Features
* **Zero Fragmentation**: All allocations reside within a single contiguous byte buffer.
* **$O(1)$ Operations**: Constant time complexity for both allocation and scoped deallocation (marker rollbacks).
* **Bitwise Memory Alignment**: Utilizes fast bitwise mask operations `(address + align - 1) & ~(align - 1)` to enforce hardware alignment constraints.
* **Scoped Deallocation (LIFO)**: Roll back the allocation offset to any previous marker state, safely discarding all temporary objects created after it.
* **RAII & Safety**: Manages buffer lifecycle via RAII and explicitly disables copy semantics (`operator=` and copy constructor).

---

## 🛠️ How It Works

1. **Initialization**: Allocates a fixed-capacity byte buffer (`uint8_t*`) on the heap and sets `m_Offset` to `0`.
2. **Allocation**:
   * Computes the current address: `currentAddress = m_Buffer + m_Offset`.
   * Applies bitwise alignment padding to get `alignmentAddress`.
   * Validates bounds (`m_Offset + totalRequiredSize <= m_Capacity`).
   * Updates `m_Offset` and returns the aligned pointer.
3. **Markers & Scoped Free (`FreeToMarker`)**:
   * `GetMarker()` captures the current `m_Offset`.
   * `FreeToMarker(marker)` resets `m_Offset` back to the saved checkpoint in $O(1)$ time, instantly freeing everything allocated after that marker.

---

## 🚀 Getting Started

### Inclusion
Include the header file in your project:

```cpp
#include "StackAllocator.h"


#include <iostream>
#include "StackAllocator.h"

int main() {
    // Pre-allocate a 1 KB stack arena
    StackAllocator allocator(1024);

    // 1. Allocate permanent/base data
    int* baseConfig = static_cast<int*>(allocator.Allocate(sizeof(int), alignof(int)));
    *baseConfig = 100;

    // 2. Take a marker before a temporary task
    size_t tempMarker = allocator.GetMarker();

    // 3. Allocate temporary data (e.g., temporary buffers or objects)
    double* tempArray = static_cast<double*>(allocator.Allocate(10 * sizeof(double), alignof(double)));
    
    std::cout << "Used Memory during task: " << allocator.GetUsedMemory() << " bytes\n";

    // 4. Roll back memory to the marker (discards tempArray instantly)
    allocator.FreeToMarker(tempMarker);

    std::cout << "Used Memory after rollback: " << allocator.GetUsedMemory() << " bytes\n";

    return 0;
}
