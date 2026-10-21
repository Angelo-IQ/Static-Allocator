#pragma once

#include <iostream>
#include <vector>
#include <cstdint>
#include <cassert>
using namespace std;

class StaticAllocator
{
private:

	uint8_t* m_Buffer;
	size_t m_Capacity;
	size_t m_Offset;

public:

	StaticAllocator(size_t Capacity) : m_Capacity(Capacity), m_Offset(0)
	{
		m_Buffer = new uint8_t[m_Capacity];
	}

	~StaticAllocator()
	{
		delete[] m_Buffer;
	}

	StaticAllocator(const StaticAllocator&) = delete;
	StaticAllocator& operator = (const StaticAllocator&) = delete;

	void* Allocator(size_t size, size_t alignment = alignof(max_align_t))
	{
		uintptr_t currentAddress = reinterpret_cast<uintptr_t>(m_Buffer + m_Offset);
		
		size_t padding = 0;

		uintptr_t alignmentAddress = (currentAddress + alignment - 1) & ~(alignment - 1);

		padding += alignmentAddress - currentAddress;

		if (m_Offset + padding + size > m_Capacity)
		{
			cerr << "[Linear] Error: Out of memory!\n";
			return nullptr;
		}

		m_Offset += padding + size;

		return reinterpret_cast<void*>(alignmentAddress);
	}

	void Reset()
	{
		m_Offset = 0;
	}

	const size_t GetMarker() const
	{
		return m_Offset;
	}

	void FreeToMarker(size_t Marker)
	{
		assert(Marker <= m_Offset && "Invalid marker position!");
		m_Offset = Marker;
	}

	const size_t GetUsedMemory() const { return m_Offset; }
	const size_t GetCapacity() const { return m_Capacity; }
};
