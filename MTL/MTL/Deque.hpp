#include <utility>
#include <iterator>

namespace mtl
{
	template <typename T>
	class deque
	{
	public:
		class iterator;

		deque()
		{
			m_Map = new T*[m_MapCapacity];
			for (size_t i = 0; i < m_MapCapacity; ++i)
				m_Map[i] = nullptr;

			m_FrontBlock = m_BackBlock = m_MapCapacity / 2;
			m_Map[m_FrontBlock] = static_cast<T*>(::operator new[](BLOCK_SIZE * sizeof(T)));
		}
		~deque()
		{
			clear();
			for (size_t i = m_FrontBlock; i < m_BackBlock; ++i)
				::operator delete[](m_Map[i]);

			delete[] m_Map;
		}
		deque(std::initializer_list<T> list)
			: deque()
		{
			for (auto& val : list)
				push_back(val);
		}
		deque(const deque& rhs)
			: m_MapCapacity(rhs.m_MapCapacity), m_Size(rhs.m_Size), m_FrontBlock(rhs.m_FrontBlock)
			, m_FrontPos(rhs.m_FrontPos), m_BackBlock(rhs.m_BackBlock), m_BackPos(rhs.m_BackPos)
		{
			m_Map = new T*[m_MapCapacity];
			for (size_t i = 0; i < m_MapCapacity; ++i)
			{
				if (rhs.m_Map[i])
				{
					m_Map[i] = static_cast<T*>(::operator new[](BLOCK_SIZE * sizeof(T)));
					size_t start = (i == m_FrontBlock) ? m_FrontPos : 0;
					size_t end = (i == m_BackBlock) ? m_BackPos : BLOCK_SIZE;
					for (; start < end; ++start)
						new (&m_Map[i][start]) T(rhs.m_Map[i][start]);
				}
				else
					m_Map[i] = nullptr;
			}
		}
		deque(deque&& rhs) noexcept
		{
			swap(*this, rhs);
		}
		deque& operator=(deque rhs)
		{
			swap(*this, rhs);
			return *this;
		}
		void push_back(const T& value)
		{
			if (m_BackPos == BLOCK_SIZE)
			{
				allocate_back();
			}
			new (&m_Map[m_BackBlock][m_BackPos++]) T(value);
			++m_Size;
		}
		void push_back(T&& value)
		{
			if (m_BackPos == BLOCK_SIZE)
			{
				allocate_back();
			}
			new (&m_Map[m_BackBlock][m_BackPos++]) T(std::move(value));
			++m_Size;
		}
		template <typename... Args>
		void emplace_back(Args&&... args)
		{
			if (m_BackPos == BLOCK_SIZE)
			{
				allocate_back();
			}
			new (&m_Map[m_BackBlock][m_BackPos++]) T(std::forward<Args>(args)...);
			++m_Size;
		}
		void pop_back()
		{
			if (m_Size == 0)
				return;
			if (m_BackPos == 0)
			{
				--m_BackBlock;
				m_BackPos = BLOCK_SIZE;
			}
			m_Map[m_BackBlock][--m_BackPos].~T();
			if (--m_Size == 0)
				reset_offsets();
		}
		void push_front(const T& value)
		{
			if (m_FrontPos == 0)
			{
				allocate_front();
			}
			new (&m_Map[m_FrontBlock][--m_FrontPos]) T(value);
			++m_Size;
		}
		void push_front(T&& value)
		{
			if (m_FrontPos == 0)
			{
				allocate_front();
			}
			new (&m_Map[m_FrontBlock][--m_FrontPos]) T(std::move(value));
			++m_Size;
		}
		template <typename... Args>
		void emplace_front(Args&&... args)
		{
			if (m_FrontPos == 0)
			{
				allocate_front();
			}
			new (&m_Map[m_FrontBlock][--m_FrontPos]) T(std::forward<Args>(args)...);
			++m_Size;
		}
		void pop_front()
		{
			if (m_Size == 0)
				return;
			m_Map[m_FrontBlock][m_FrontPos++].~T();
			if (--m_Size == 0)
			{
				reset_offsets();
			}
			else if (m_FrontPos == BLOCK_SIZE)
			{
				++m_FrontBlock;
				m_FrontPos = 0;
			}
		}
		size_t size() const
		{
			return m_Size;
		}
		bool empty() const
		{
			return m_Size == 0;
		}
		void clear()
		{
			for (size_t i = 0; i < m_Size; ++i)
				operator[](i).~T();

			m_Size = 0;
		}
		T& operator[](size_t index)
		{
			size_t absolute_start = m_FrontPos + index;
			size_t block = m_FrontBlock + absolute_start / BLOCK_SIZE;
			size_t block_offset = absolute_start % BLOCK_SIZE;
			return m_Map[block][block_offset];
		}
		const T& operator[](size_t index) const
		{
			size_t absolute_start = m_FrontPos + index;
			size_t block = m_FrontBlock + absolute_start / BLOCK_SIZE;
			size_t block_offset = absolute_start % BLOCK_SIZE;
			return m_Map[block][block_offset];
		}
		iterator begin() const
		{
			return iterator(m_Map + m_FrontBlock, m_Map[m_FrontBlock] + m_FrontPos);
		}
		iterator end() const
		{
			return iterator(m_Map + m_BackBlock, m_Map[m_BackBlock] + m_BackPos);
		}
		T& front() const
		{
			return m_Map[m_FrontBlock][m_FrontPos];
		}
		T& back() const
		{
			return m_Map[m_BackBlock][m_BackPos - 1];
		}

	public:
		class iterator
		{
		public:
			using iterator_category = std::random_access_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;

			iterator() = default;
			iterator(T** block_, T* cur_)
				: block(block_), cur(cur_)
			{
				front = *block;
				back = front + BLOCK_SIZE;
			}
			reference operator*() const
			{
				return *cur;
			}
			pointer operator->() const
			{
				return cur;
			}
			iterator& operator+=(difference_type n)
			{
				ptrdiff_t absolute_offset = (cur - front) + n;
				if (absolute_offset >= 0 && absolute_offset < BLOCK_SIZE)
				{
					cur += n;
				}
				else
				{
					ptrdiff_t block_offset = (absolute_offset > 0) ? absolute_offset / BLOCK_SIZE : -ptrdiff_t((- absolute_offset - 1) / BLOCK_SIZE) - 1;
					ptrdiff_t current_offset = absolute_offset - block_offset * BLOCK_SIZE;
					set_block(block + block_offset);
					cur = front + (current_offset);
				}
				return *this;
			}
			iterator& operator-=(difference_type n)
			{
				return operator+=(-n);
			}
			iterator& operator++()
			{
				if (++cur == back)
				{
					set_block(block + 1);
					cur = front;
				}
				return *this;
			}
			iterator operator++(int)
			{
				iterator tmp(*this);
				if (++cur == back)
				{
					set_block(block + 1);
					cur = front;
				}
				return tmp;
			}
			iterator& operator--()
			{
				if (cur == front)
				{
					set_block(block - 1);
					cur = back;
				}
				--cur;
				return *this;
			}
			iterator operator--(int)
			{
				iterator tmp(*this);
				if (cur == front)
				{
					set_block(block - 1);
					cur = back;
				}
				--cur;
				return tmp;
			}
			T& operator[](difference_type n) const
			{
				return *(*this + n);
			}
			friend iterator operator+(iterator it, difference_type n)
			{
				return it += n;
			}
			friend iterator operator+(difference_type n, iterator it)
			{
				return it += n;
			}
			friend bool operator==(const iterator& lhs, const iterator& rhs)
			{
				return lhs.cur == rhs.cur;
			}
			friend bool operator!=(const iterator& lhs, const iterator & rhs)
			{
				return lhs.cur != rhs.cur;
			}
			friend bool operator<(const iterator& lhs, const iterator& rhs)
			{
				return (lhs.block == rhs.block) ? lhs.cur < rhs.cur : lhs.block < rhs.block;
			}
			friend bool operator>(const iterator& lhs, const iterator& rhs)
			{
				return operator<(rhs, lhs);
			}
			friend bool operator<=(const iterator& lhs, const iterator& rhs)
			{
				return operator<(lhs, rhs) || operator==(lhs, rhs);
			}
			friend bool operator>=(const iterator& lhs, const iterator& rhs)
			{
				return operator>(lhs, rhs) || operator==(lhs, rhs);
			}

		private:
			void set_block(T** block)
			{
				this->block = block;
				front = *block;
				back = front + BLOCK_SIZE;
			}

		private:
			T** block{ nullptr };
			T* cur{ nullptr };
			T* front{ nullptr };
			T* back{ nullptr };
		};

	private:
		void reallocate_map(size_t new_map_capacity)
		{
			T** new_map = new T*[new_map_capacity];
			for (size_t i = 0; i < new_map_capacity; ++i)
				new_map[i] = nullptr;

			size_t new_front_block = (new_map_capacity - m_MapCapacity) / 2;
			size_t map_size = m_BackBlock - m_FrontBlock + 1;
			for (size_t i = 0; i < map_size; ++i)
				new_map[new_front_block + i] = m_Map[m_FrontBlock + i];

			delete[] m_Map;
			m_Map = new_map;
			m_MapCapacity = new_map_capacity;
			m_FrontBlock = new_front_block;
			m_BackBlock = m_FrontBlock + map_size - 1;
		}
		void reset_offsets()
		{
			m_FrontBlock = m_BackBlock = m_MapCapacity / 2;
			m_FrontPos = m_BackPos = BLOCK_SIZE / 2;
		}
		void allocate_back()
		{
			if (m_BackBlock + 1 == m_MapCapacity)
				reallocate_map(m_MapCapacity * 2);
			if (m_Map[++m_BackBlock] == nullptr)
				m_Map[m_BackBlock] = static_cast<T*>(::operator new[](BLOCK_SIZE * sizeof(T)));
			m_BackPos = 0;
		}
		void allocate_front()
		{
			if (m_FrontBlock == 0)
				reallocate_map(m_MapCapacity * 2);
			if (m_Map[--m_FrontBlock] == nullptr)
				m_Map[m_FrontBlock] = static_cast<T*>(::operator new[](BLOCK_SIZE * sizeof(T)));
			m_FrontPos = BLOCK_SIZE;
		}
		friend void swap(deque& lhs, deque& rhs) noexcept
		{
			std::swap(lhs.m_Map, rhs.m_Map);
			std::swap(lhs.m_MapCapacity, rhs.m_MapCapacity);
			std::swap(lhs.m_Size, rhs.m_Size);
			std::swap(lhs.m_FrontBlock, rhs.m_FrontBlock);
			std::swap(lhs.m_BackBlock, rhs.m_BackBlock);
			std::swap(lhs.m_FrontPos, rhs.m_FrontPos);
			std::swap(lhs.m_BackPos, rhs.m_BackPos);
		}
	private:
		static constexpr size_t BLOCK_SIZE{ 8 };
		static constexpr size_t MAP_MINIMUM_SIZE{ 8 };

		T** m_Map{ nullptr };
		size_t m_MapCapacity{ MAP_MINIMUM_SIZE };
		size_t m_Size{ 0 };
		size_t m_FrontBlock = 0, m_BackBlock = 0;
		size_t m_FrontPos = BLOCK_SIZE / 2, m_BackPos = BLOCK_SIZE / 2;
	};
}