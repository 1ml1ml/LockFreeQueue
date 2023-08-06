#pragma once

#include <atomic>
#include <optional>

template<typename T>
class LockFreeQueue
{
public:
	LockFreeQueue(size_t size);
	~LockFreeQueue();
	
public:
	bool enqueue(const T&& val);
	std::optional<T> dequeue();

	bool empty() const;
	bool full() const;

private:
	using TQueue = T*;
	size_t m_Size = 0;
	std::atomic<size_t> m_ReadIndex = 0;
	std::atomic<size_t> m_WriteIndex = 0;
	TQueue m_Queue = nullptr;
};

template<typename T>
inline LockFreeQueue<T>::LockFreeQueue(size_t size) :
	m_Size(size + 1)
{
	m_Queue = reinterpret_cast<T*>(malloc((m_Size) * sizeof(T)));
}

template<typename T>
inline LockFreeQueue<T>::~LockFreeQueue()
{
	for (auto index = 0; index < m_WriteIndex.load(); ++index)
		m_Queue[index].~T();
	free(m_Queue);
}

template<typename T>
inline bool LockFreeQueue<T>::enqueue(const T&& val)
{
	if (true == full())
		return false;

	size_t wIndex = 0;
	do wIndex = m_WriteIndex.load();
	while (false == m_WriteIndex.compare_exchange_strong(wIndex, (wIndex + 1) % m_Size));

	new (&m_Queue[wIndex]) T(std::forward<const T>(val));
	return true;
}

template<typename T>
inline std::optional<T> LockFreeQueue<T>::dequeue()
{
	if (true == empty())
		return std::nullopt;

	size_t rIndex = 0;
	do rIndex = m_ReadIndex.load();
	while (false == m_ReadIndex.compare_exchange_strong(rIndex, (rIndex + 1) % m_Size));

	return m_Queue[rIndex];
}

template<typename T>
inline bool LockFreeQueue<T>::empty() const
{
	return m_ReadIndex.load() == m_WriteIndex.load();
}

template<typename T>
inline bool LockFreeQueue<T>::full() const
{
	return m_WriteIndex.load() + 1 == m_ReadIndex.load();
}

