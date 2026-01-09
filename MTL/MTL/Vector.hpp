#pragma once
#include <algorithm>
#include <memory>

namespace mtl
{
	template<typename T, typename Alloc = std::allocator<T>>
	class vector
	{
	public:
		class iterator;
		using alloc_traits = std::allocator_traits<Alloc>;

		vector() = default;
		~vector()
		{
			clear();
			alloc_traits::deallocate(m_Allocator, m_Container, m_Capacity);
		}
		vector(const vector& rhs)
			: m_Size(rhs.m_Size), m_Capacity(rhs.m_Capacity)
		{
			if (m_Capacity > 0)
			{
				m_Container = alloc_traits::allocate(m_Allocator, m_Capacity);

				size_t i = 0;
				for (T& val : rhs)
					new (&m_Container[i++]) T(val);
			}
		}
		vector(vector&& rhs) noexcept
		{
			swap(*this, rhs);
		}
		explicit vector(size_t size)
			: m_Size(size), m_Capacity(size)
		{
			if (m_Capacity > 0)
			{
				m_Container = alloc_traits::allocate(m_Allocator, m_Capacity);

				for (T& it : *this)
				{
					new (&it) T();
				}
			}
		}
		vector(std::initializer_list<T> list)
			: m_Size(list.size()), m_Capacity(m_Size)
		{
			if (m_Capacity > 0)
			{
				m_Container = alloc_traits::allocate(m_Allocator, m_Capacity);

				size_t i = 0;
				for (auto& val : list)
					new (&m_Container[i++]) T(val);
			}
		}
		vector& operator=(vector rhs)
		{
			swap(*this, rhs);
			return *this;
		}
		void push_back(const T& value)
		{
			if (m_Capacity <= m_Size)
				reallocate(recalc_capacity());
			new (&m_Container[m_Size]) T(value);
			m_Size++;
		}
		void push_back(T&& value)
		{
			if (m_Capacity <= m_Size)
				reallocate(recalc_capacity());
			new (&m_Container[m_Size]) T(std::move(value));
			m_Size++;
		}
		template<typename... Args>
		void emplace_back(Args&&... args)
		{
			if (m_Capacity <= m_Size)
				reallocate(recalc_capacity());
			new (&m_Container[m_Size]) T(std::forward<Args>(args)...);
			m_Size++;
		}
		void pop_back()
		{
			if (m_Size > 0)
			{
				m_Container[m_Size - 1].~T();
				m_Size--;
			}
		}
		void clear()
		{
			for (size_t i = 0; i < m_Size; ++i)
			{
				m_Container[i].~T();
			}
			m_Size = 0;
		}
		void reserve(size_t capacity)
		{
			if (capacity > m_Capacity)
				reallocate(capacity);
		}
		T& operator[](size_t index)
		{
			return m_Container[index];
		}
		const T& operator[](size_t index) const
		{
			return m_Container[index];
		}
		iterator begin() const noexcept
		{
			return m_Container;
		}
		iterator end() const noexcept
		{
			return m_Container + m_Size;
		}
		size_t size() const noexcept
		{
			return m_Size;
		}
		size_t capacity() const noexcept
		{
			return m_Capacity;
		}
		bool empty() const noexcept
		{
			return m_Size == 0;
		}
		T* data() const noexcept
		{
			return m_Container;
		}

	public:
		class iterator
		{
		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;  
			using reference = T&; 

			iterator(pointer ptr)
				: m_Ptr(ptr) 
			{
			}
			reference operator*() const
			{
				return *m_Ptr;
			}
			pointer operator->()
			{
				return m_Ptr;
			}
			iterator& operator++()
			{
				m_Ptr++;
				return *this;
			}
			iterator operator++(int)
			{
				iterator temp = *this;
				++(*this);
				return temp;
			}
			friend bool operator==(const iterator& lhs, const iterator& rhs)
			{
				return lhs.m_Ptr == rhs.m_Ptr;
			}
			friend bool operator!=(const iterator& lhs, const iterator& rhs)
			{
				return lhs.m_Ptr != rhs.m_Ptr;
			}

		private:
			pointer m_Ptr;
		};

	private:
		void reallocate(size_t newCapacity)
		{
			T* newBuffer = alloc_traits::allocate(m_Allocator, newCapacity);
			for (size_t i = 0; i < m_Size; ++i)
			{
				new (&newBuffer[i]) T(std::move(m_Container[i]));
				m_Container[i].~T();
			}
			alloc_traits::deallocate(m_Allocator, m_Container, m_Capacity);
			m_Container = newBuffer;
			m_Capacity = newCapacity;
		}
		size_t recalc_capacity()
		{
			return std::max((m_Capacity + m_Capacity / 2), m_Capacity + 1);
		}
		friend void swap(vector& lhs, vector& rhs) noexcept
		{
			std::swap(lhs.m_Container, rhs.m_Container);
			std::swap(lhs.m_Size, rhs.m_Size);
			std::swap(lhs.m_Capacity, rhs.m_Capacity);
		}

	private:
		T* m_Container{ nullptr };
		size_t m_Size{ 0 };
		size_t m_Capacity{ 0 };
		Alloc m_Allocator;
	};
}
