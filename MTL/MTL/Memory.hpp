#pragma once
#include <utility>
#include <atomic>
#include <new>

namespace mtl
{
	template <typename T>
	class unique_ptr
	{
	public:
		unique_ptr() = default;
		~unique_ptr()
		{
			delete m_Data;
		}
		explicit unique_ptr(T* data)
			: m_Data(data)
		{
		}
		unique_ptr(const unique_ptr& rhs) = delete;
		unique_ptr(unique_ptr&& rhs) noexcept
		{
			swap(*this, rhs);
		}
		unique_ptr& operator=(const unique_ptr& rhs) = delete;
		unique_ptr& operator=(unique_ptr&& rhs) noexcept
		{
			unique_ptr tmp(std::move(rhs));
			swap(*this, tmp);
			return *this;
		}
		T& operator*() const
		{
			return *m_Data;
		}
		T* operator->() const
		{
			return m_Data;
		}
		T* release() noexcept
		{
			return std::exchange(m_Data, nullptr);
		}
		T* get() const noexcept
		{
			return m_Data;
		}
		void reset(T* data)
		{
			delete m_Data;
			m_Data = data;
		}

		explicit operator bool() const
		{
			return m_Data != nullptr;
		}

	private:
		friend void swap(unique_ptr& lhs, unique_ptr& rhs) noexcept
		{
			std::swap(lhs.m_Data, rhs.m_Data);
		}
	private:
		T* m_Data{ nullptr };
	};

	template <typename T>
	class unique_ptr<T[]>
	{
	public:
		unique_ptr() = default;
		~unique_ptr()
		{
			delete[] m_Data;
		}
		unique_ptr(T* data)
			: m_Data(data)
		{
		}
		unique_ptr(const unique_ptr& rhs) = delete;
		unique_ptr(unique_ptr&& rhs) noexcept
		{
			swap(*this, rhs);
		}
		unique_ptr& operator=(const unique_ptr& rhs) = delete;
		unique_ptr& operator=(unique_ptr&& rhs) noexcept
		{
			unique_ptr tmp(std::move(rhs));
			swap(*this, tmp);
			return *this;
		}
		T& operator[](size_t index)
		{
			return m_Data[index];
		}
		T* release() noexcept
		{
			return std::exchange(m_Data, nullptr);
		}
		T* get() const noexcept
		{
			return m_Data;
		}
		void reset(T* data)
		{
			delete[] m_Data;
			m_Data = data;
		}

		explicit operator bool() const
		{
			return m_Data != nullptr;
		}

	private:
		friend void swap(unique_ptr& lhs, unique_ptr& rhs) noexcept
		{
			std::swap(lhs.m_Data, rhs.m_Data);
		}

	private:
		T* m_Data{ nullptr };
	};

	template <typename T>
	concept non_array = !std::is_array_v<T>;

	template <typename T>
	class shared_ptr
	{
		template <non_array T, typename... Args>
		friend shared_ptr<T> make_shared(Args&&... args);

	public:
		shared_ptr() = default;
		explicit shared_ptr(T* data)
			: m_Data(data), m_ControlBlock(new control_block(data))
		{
		}
		~shared_ptr()
		{
			DecRef();
		}
		shared_ptr(const shared_ptr& rhs)
			: m_Data(rhs.m_Data), m_ControlBlock(rhs.m_ControlBlock)
		{
			IncRef();
		}
		shared_ptr(shared_ptr&& rhs) noexcept
		{
			swap(*this, rhs);
		}
		shared_ptr& operator=(const shared_ptr& rhs)
		{
			if (this != &rhs)
			{
				DecRef();
				m_Data = rhs.m_Data;
				m_ControlBlock = rhs.m_ControlBlock;
				IncRef();
			}
			return *this;
		}
		shared_ptr& operator=(shared_ptr&& rhs)
		{
			shared_ptr temp(std::move(rhs));
			swap(*this, temp);
			return *this;
		}

		T& operator*() const 
		{ 
			return *m_Data; 
		}
		T* operator->() const 
		{ 
			return m_Data;
		}
		T* get() const noexcept
		{ 
			return m_Data;
		}
		size_t use_count() const noexcept
		{
			return m_ControlBlock ? m_ControlBlock->RefCount() : 0;
		}
		explicit operator bool() const 
		{
			return m_Data != nullptr;
		}

	private:
		class control_block_base
		{
			friend class shared_ptr;

		public:
			virtual ~control_block_base() = default;
			void IncRef()
			{
				m_RefCount++;
			}
			void DecRef()
			{
				if (--m_RefCount == 0)
				{
					Destroy();
					DeleteThis();
				}
			}
			size_t RefCount() const
			{
				return m_RefCount;
			}
			virtual void DeleteThis() = 0;
			virtual void Destroy() = 0;

		private:
			std::atomic<size_t> m_RefCount{ 1 };
		};

		class control_block : public control_block_base
		{
		public:
			control_block(T* data)
				: m_Data(data)
			{
			}
			void Destroy() override
			{
				delete m_Data;
			}
			void DeleteThis() override
			{
				delete this;
			}

		private:
			T* m_Data;
		};

		class control_block_shared : public control_block_base
		{
			friend class shared_ptr;

		public:
			template<typename... Args>
			control_block_shared(Args&&... args)
			{
				new(m_Data) T(std::forward<Args>(args)...);
			}
			void Destroy() override
			{
				ObjPtr()->~T();
			}
			void DeleteThis() override
			{
				delete this;
			}
			T* ObjPtr()
			{
				return reinterpret_cast<T*>(&m_Data);
			}
		private:
			alignas(T) std::byte m_Data[sizeof(T)];
		};

		shared_ptr(control_block_shared* block)
			: m_Data(block->ObjPtr()), m_ControlBlock(block)
		{
		}
		friend void swap(shared_ptr& lhs, shared_ptr& rhs) noexcept
		{
			std::swap(lhs.m_Data, rhs.m_Data);
			std::swap(lhs.m_ControlBlock, rhs.m_ControlBlock);
		}
		void DecRef()
		{
			if (m_ControlBlock)
			{
				m_ControlBlock->DecRef();
			}
		}
		void IncRef()
		{
			if (m_ControlBlock)
			{
				m_ControlBlock->IncRef();
			}
		}
	private:
		T* m_Data{ nullptr };
		control_block_base* m_ControlBlock{ nullptr };
	};

	template <non_array T, typename... Args>
	unique_ptr<T> make_unique(Args&&... args)
	{
		return unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
	template <typename T>
	unique_ptr<T> make_unique(size_t size)
	{
		return unique_ptr<T>(new std::remove_extent_t<T>[size]());
	}

	template <non_array T, typename... Args>
	shared_ptr<T> make_shared(Args&&... args)
	{
		auto block = new shared_ptr<T>::control_block_shared(std::forward<Args>(args)...);
		return shared_ptr<T>(block);
	}

}