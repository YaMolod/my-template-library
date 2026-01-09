#pragma once

#include <iostream>
#include "Memory.hpp"
#include <variant>
#include <array>
#include <optional>

namespace mtl
{
	class string
	{
		static constexpr size_t SMALL_STRING{ 16 };
		using small_string = std::array<char, SMALL_STRING>;
		using large_string = mtl::unique_ptr<char[]>;

	public:
		string() = default;
		~string() = default;
		string(const char* data)
		{
			copy_init(data);
		}
		string(const string& rhs)
		{
			copy_init(rhs.c_str());
		}
		string(string&& rhs) noexcept
		{
			swap(*this, rhs);
		}
		string& operator=(const string& rhs)
		{
			if (this != &rhs)
				return operator=(rhs.c_str());

			return *this;
		}
		string& operator=(string&& rhs) noexcept
		{
			string tmp(std::move(rhs));
			swap(*this, tmp);
			return *this;
		}
		string& operator=(const char* rhs)
		{
			if (auto length = std::strlen(rhs); m_Capacity > length)
			{
				clear();
				std::copy_n(rhs, length + 1, data());
				m_Length = length;
			}
			else
			{
				string tmp(rhs);
				swap(*this, tmp);
			}
			return *this;
		}
		char& operator[](size_t index)
		{
			return data()[index];
		}
		const char& operator[](size_t index) const
		{
			return data()[index];
		}

		friend bool operator==(const string& lhs, const string& rhs)
		{
			return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
		}
		friend auto operator<=>(const string& lhs, const string& rhs)
		{
			return std::strcmp(lhs.c_str(), rhs.c_str());
		}
		friend string operator+(const string& lhs, const string& rhs)
		{
			string res(lhs.c_str(), lhs.m_Length + rhs.m_Length);
			std::copy_n(rhs.c_str(), rhs.m_Length + 1, res.data() + lhs.m_Length);
			return res;
		}
		friend string& operator+=(string& lhs, const string& rhs)
		{
			return operator+=(lhs, rhs.c_str());
		}
		friend string& operator+=(string& lhs, const char* rhs)
		{
			auto length = std::strlen(rhs);
			if (lhs.m_Capacity > lhs.m_Length + length)
			{
				std::copy_n(rhs, length + 1, lhs.data() + lhs.m_Length);
				lhs.m_Length += length;
			}
			else
			{
				lhs = lhs + rhs;
			}
			return lhs;
		}
		friend std::ostream& operator<<(std::ostream& os, const string& str)
		{
			return os << str.c_str();
		}

	public:
		const char* c_str() const noexcept
		{
			if (m_Capacity > SMALL_STRING)
				return std::get<large_string>(m_SSO).get();
			else
				return std::get<small_string>(m_SSO).data();
		}
		char* data() noexcept
		{ 
			if (m_Capacity > SMALL_STRING)
				return std::get<large_string>(m_SSO).get();
			else
				return std::get<small_string>(m_SSO).data();
		}
		const char* data() const noexcept
		{
			if (m_Capacity > SMALL_STRING)
				return std::get<large_string>(m_SSO).get();
			else
				return std::get<small_string>(m_SSO).data();
		}
		void clear() noexcept
		{
			if (auto buffer = data())
			{
				buffer[0] = '\0';
				m_Length = 0;
			}
		}
		size_t size() const noexcept
		{
			return m_Length;
		}
		bool empty() const noexcept
		{
			return m_Length == 0;
		}

	private:
		string(const char* data, size_t buffer_size)
		{
			copy_init(data, buffer_size);
		}
		friend void swap(string& lhs, string& rhs) noexcept
		{
			std::swap(lhs.m_SSO, rhs.m_SSO);
			std::swap(lhs.m_Length, rhs.m_Length);
			std::swap(lhs.m_Capacity, rhs.m_Capacity);
		}
		void copy_init(const char* data, std::optional<size_t> buffer_size = std::nullopt)
		{
			size_t string_length = std::strlen(data);
			m_Length = buffer_size.value_or(string_length);

			if (SMALL_STRING > m_Length)
			{
				std::array<char, SMALL_STRING> stack;
				std::copy_n(data, string_length + 1, stack.data());
				m_SSO = std::move(stack);
			}
			else
			{
				m_Capacity = m_Length + m_Length / 2;
				auto heap = mtl::make_unique<char[]>(m_Capacity + 1);
				std::copy_n(data, string_length + 1, heap.get());
				m_SSO = std::move(heap);
			}
		}
		
	private:
		std::variant<small_string, large_string> m_SSO;
		size_t m_Length{ 0 };
		size_t m_Capacity{ SMALL_STRING };
	};
}