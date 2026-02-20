#include "Memory.hpp"
#include <iterator>

namespace mtl
{
	template <typename T>
	class list
	{
		struct node_base;
		struct node;
		class iterator;

	public:
		list() = default;
		~list()
		{
			clear();
		}
		list(const list& rhs)
		{
			node_base* it = rhs.m_Sentinel.next;
			while (it != &rhs.m_Sentinel)
			{
				push_back(static_cast<node*>(it)->data);
				it = it->next;
			}
		}
		list(std::initializer_list<T> rhs)
		{
			for (const auto& item : rhs)
			{
				push_back(item);
			}
		}
		list(list&& rhs) noexcept
		{
			swap(*this, rhs);
		}
		list& operator=(list rhs) noexcept
		{
			swap(*this, rhs);
			return *this;
		}
		void push_back(const T& value)
		{
			emplace(end(), value);
		}
		void push_back(T&& value)
		{
			emplace(end(), std::move(value));
		}
		template <typename... Args>
		void emplace_back(Args&&... args)
		{
			emplace(end(), std::forward<Args>(args)...);
		}
		void pop_back()
		{
			if (m_Size != 0)
			{
				node_base* last = m_Sentinel.prev;
				node_base* new_last = last->prev;
				m_Sentinel.prev = new_last;
				new_last->next = &m_Sentinel;
				delete static_cast<node*>(last);
				--m_Size;
			}
		}
		void push_front(const T& value)
		{
			emplace(begin(), value);
		}
		void push_front(T&& value)
		{
			emplace(begin(), std::move(value));
		}
		template <typename... Args>
		void emplace_front(Args&&... args)
		{
			emplace(begin(), std::forward<Args>(args)...);
		}
		void pop_front()
		{
			if (m_Size != 0)
			{
				node_base* first = m_Sentinel.next;
				node_base* new_first = first->next;
				m_Sentinel.next = new_first;
				new_first->prev = &m_Sentinel;
				delete static_cast<node*>(first);
				--m_Size;
			}
		}
		iterator insert(iterator pos, const T& value)
		{
			return emplace(pos, value);
		}
		iterator erase(iterator pos)
		{
			node_base* cur = pos.m_Node;
			node_base* next_node = cur->next;
			node_base* prev_node = cur->prev;

			next_node->prev = prev_node;
			prev_node->next = next_node;
			delete static_cast<node*>(cur);
			--m_Size;

			return iterator(next_node);
		}
		const T& back() const
		{
			if (empty())
				throw std::runtime_error("List is empty");

			return static_cast<node*>(m_Sentinel.prev)->data;
		}
		const T& front() const
		{
			if (empty())
				throw std::runtime_error("List is empty");

			return static_cast<node*>(m_Sentinel.next)->data;
		}
		iterator end()
		{
			return iterator(&m_Sentinel);
		}
		iterator begin()
		{
			return iterator(m_Sentinel.next);
		}
		void clear()
		{
			node_base* it = m_Sentinel.next;
			while (it != &m_Sentinel)
			{
				node* temp = static_cast<node*>(it);
				it = it->next;
				delete temp;
			}
			m_Size = 0;
			fix_sentinel();
		}
		size_t size() const
		{
			return m_Size;
		}
		bool empty() const
		{
			return m_Size == 0;
		}
	private:
		struct node_base
		{
			node_base* prev{ this };
			node_base* next{ this };
		};
		struct node : public node_base
		{
			T data;

			node(const T& value)
				: data(value)
			{
			}
			template <typename... Args>
			node(Args&&... args)
				: data(std::forward<Args>(args)...)
			{
			}
		};
		class iterator
		{
			friend class list;
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;

			explicit iterator(node_base* node)
				: m_Node(node)
			{
			}
			reference operator*() const
			{
				return static_cast<node*>(m_Node)->data;
			}
			pointer operator->() const
			{
				return &static_cast<node*>(m_Node)->data;
			}
			iterator& operator++()
			{
				m_Node = m_Node->next;
				return *this;
			}
			iterator operator++(int)
			{
				iterator temp = *this;
				++(*this);
				return temp;
			}
			iterator& operator--()
			{
				m_Node = m_Node->prev;
				return *this;
			}
			iterator operator--(int)
			{
				iterator temp = *this;
				--(*this);
				return temp;
			}
			friend bool operator==(const iterator& lhs, const iterator& rhs)
			{
				return lhs.m_Node == rhs.m_Node;
			}
			friend bool operator!=(const iterator& lhs, const iterator& rhs)
			{
				return lhs.m_Node != rhs.m_Node;
			}

		private:
			node_base* m_Node{ nullptr };
		};
		template <typename... Args>
		iterator emplace(iterator pos, Args&&... args)
		{
			node* new_node = new node(std::forward<Args>(args)...);

			node_base* next_node = pos.m_Node;
			node_base* prev_node = next_node->prev;

			new_node->next = next_node;
			new_node->prev = prev_node;
			prev_node->next = new_node;
			next_node->prev = new_node;

			++m_Size;
			return iterator(new_node);
		}
		void fix_sentinel()
		{
			if (m_Size > 0)
			{
				m_Sentinel.next->prev = &m_Sentinel;
				m_Sentinel.prev->next = &m_Sentinel;
			}
			else
				m_Sentinel.next = m_Sentinel.prev = &m_Sentinel;
		}
		friend void swap(list& lhs, list& rhs)
		{
			std::swap(lhs.m_Size, rhs.m_Size);
			std::swap(lhs.m_Sentinel.next, rhs.m_Sentinel.next);
			std::swap(lhs.m_Sentinel.prev, rhs.m_Sentinel.prev);
			lhs.fix_sentinel();
			rhs.fix_sentinel();
		}
		
	private:
		node_base m_Sentinel;
		size_t m_Size{ 0 };
	};
}