#pragma once
#include "Logger.hpp"
namespace base
{
	class mem
	{
	public:
		template <typename ptr_t = void*>
		mem* setPtr(ptr_t p) {
			m_ptr = (void*)p;
			return this;
		}

		mem(void* p = nullptr)
		{
			setPtr(p);
		}

		mem(const uintptr_t p)
		{
			setPtr(p);
		}

		template <typename t> std::enable_if_t<std::is_pointer_v<t>, t> as()
		{
			return static_cast<t>(m_ptr);
		}

		template <typename t> std::enable_if_t<std::is_lvalue_reference_v<t>, t> as()
		{
			return *static_cast<std::add_pointer_t<std::remove_reference_t<t>>>(m_ptr);
		}

		template <typename t> std::enable_if_t<std::is_same_v<t, uintptr_t>, t> as()
		{
			return t(m_ptr);
		}

		mem add(size_t offset)
		{
			return { as<uintptr_t>() + offset };
		}

		mem sub(size_t offset)
		{
			return { as<uintptr_t>() - offset };
		}

		mem rip()
		{
			return add(as<int32_t&>()).add(4);
		}

		operator bool() noexcept
		{
			return m_ptr;
		}

		void* m_ptr{};
	};

	/*class hmod
	{
	public:
		hmod(HMODULE hmod) {
			m_base.setPtr(hmod);
			auto dosHdr = reinterpret_cast<IMAGE_DOS_HEADER*>(m_base.as<char*>());
			auto ntHdr = reinterpret_cast<IMAGE_NT_HEADERS64*>(m_base.as<char*>() + dosHdr->e_lfanew);
			m_size = ntHdr->OptionalHeader.SizeOfImage;
			m_end = m_base.as<char*>() + m_size;
		}
		hmod(const char* hmod) : hmod(GetModuleHandleA(hmod)) {}
		hmod(std::string_view hmod) : hmod(hmod.data()) {}
		hmod() : hmod(GetModuleHandleA(NULL)) {}
	public:
		mem m_base{}, m_end{};
		size_t m_size{};
	};*/

	class hmod
	{
	public:
		hmod(std::string_view name = {}) : m_name(name), m_handle(get()), m_base(m_handle)
		{
			auto dosHeader = m_base.as<IMAGE_DOS_HEADER*>();
			auto ntHeader = m_base.add(dosHeader->e_lfanew).as<IMAGE_NT_HEADERS*>();
			m_size = ntHeader->OptionalHeader.SizeOfImage;
		}

		mem begin()
		{
			return m_base;
		}

		mem end()
		{
			return begin().add(size());
		}

		size_t size()
		{
			return m_size;
		}

		FARPROC getProcess(std::string search)
		{
			return GetProcAddress(m_handle, search.c_str());
		}

		HMODULE get()
		{
			if (m_name.empty())
			{
				return GetModuleHandleA(NULL);
			}
			return GetModuleHandleA(m_name.data());
		}

	private:
		std::string_view m_name{};
		HMODULE m_handle{};
		mem m_base{};
		size_t m_size{};
	};

	constexpr std::optional<uint8_t> charToHex(char const c)
	{
		switch (c) {
		case '0': return static_cast<std::uint8_t>(0x0);
		case '1': return static_cast<std::uint8_t>(0x1);
		case '2': return static_cast<std::uint8_t>(0x2);
		case '3': return static_cast<std::uint8_t>(0x3);
		case '4': return static_cast<std::uint8_t>(0x4);
		case '5': return static_cast<std::uint8_t>(0x5);
		case '6': return static_cast<std::uint8_t>(0x6);
		case '7': return static_cast<std::uint8_t>(0x7);
		case '8': return static_cast<std::uint8_t>(0x8);
		case '9': return static_cast<std::uint8_t>(0x9);
		case 'a': return static_cast<std::uint8_t>(0xa);
		case 'b': return static_cast<std::uint8_t>(0xb);
		case 'c': return static_cast<std::uint8_t>(0xc);
		case 'd': return static_cast<std::uint8_t>(0xd);
		case 'e': return static_cast<std::uint8_t>(0xe);
		case 'f': return static_cast<std::uint8_t>(0xf);
		case 'A': return static_cast<std::uint8_t>(0xA);
		case 'B': return static_cast<std::uint8_t>(0xB);
		case 'C': return static_cast<std::uint8_t>(0xC);
		case 'D': return static_cast<std::uint8_t>(0xD);
		case 'E': return static_cast<std::uint8_t>(0xE);
		case 'F': return static_cast<std::uint8_t>(0xF);
		default: return std::nullopt;
		}
	}

	inline bool doesMemoryMatch(uint8_t* target, std::optional<uint8_t> const* sig, size_t len)
	{
		for (size_t i{}; i != len; ++i)
			if (sig[i] && *sig[i] != target[i])
				return false;
		return true;
	}

	inline char* findPattern(std::optional<uint8_t>* elements, size_t bCount, hmod mod = {})
	{
		for (uintptr_t i{}; i < mod.size() - bCount; ++i)
		{
			if (doesMemoryMatch(mod.begin().add(i).as<uint8_t*>(), elements, bCount))
			{
				return mod.begin().as<char*>() + i;
			}
		}
		return nullptr;
	}

	inline std::vector<std::optional<uint8_t>> createBytesFromString(std::string_view ptr)
	{
		std::vector<std::optional<uint8_t>> bytes{};
		for (size_t i{}; i != ptr.size(); ++i)
		{
			if (ptr[i] == ' ')
			{
				continue;
			}
			if (ptr[i] != '?')
			{
				if (i != ptr.size() - 1)
				{
					auto c1 = charToHex(ptr[i]);
					auto c2 = charToHex(ptr[i + 1]);
					if (c1 && c2)
					{
						bytes.emplace_back(static_cast<uint8_t>((*c1 * 0x10) + *c2));
					}
				}
			}
			else
			{
				bytes.emplace_back(std::nullopt);
			}
		}
		return bytes;
	}

	class signature
	{
	public:
		signature(std::string_view ptr) {
			rawPatternString = ptr;

			m_bytes = createBytesFromString(ptr);

			return;

			const auto size = ptr.size();
			for (std::size_t i{}; i != size; ++i)
			{
				if (ptr[i] == ' ')
				{
					continue;
				}

				bool last = (i == ptr.size() - 1);

				try
				{
					if (ptr.at(i) != '?')
					{
						if (!last)
						{
							auto c1 = charToHex(ptr[i]);
							auto c2 = charToHex(ptr[i + 1]);
							if (c1 && c2)
							{
								//m_bytes.emplace_back(static_cast<std::uint8_t>((*c1 * 0x10) + *c2));
							}
						}
					}
				}
				catch (const std::out_of_range& ex)
				{
					g_logger->send(logColor::red, "Failed", "");
				}

				//else
				//{
				//	m_bytes.push_back({});
				//	// add support for double question mark sigs
				//	if (ptr[i + 1] == '?') {
				//		++i;
				//	}
				//}
			}
		}

		signature(const void* bytes, std::string_view mask)
		{
			const auto size = mask.size();
			for (std::size_t i{}; i != size; ++i)
			{
				if (mask[i] != '?')
				{
					m_bytes.emplace_back(static_cast<const std::uint8_t*>(bytes)[i]);
				}
				else
				{
					m_bytes.emplace_back(std::nullopt);
				}
			}
		}

		signature(const char* ida_sig) :
			signature(std::string_view(ida_sig))
		{
			rawPatternString = ida_sig;
		}

		mem get(hmod mod = {})
		{
			std::size_t maxShift = m_bytes.size();
			std::size_t maxIdx = maxShift - 1;

			//Get wildcard index, and store max shiftable byte count
			std::size_t wildCardIdx{static_cast<size_t>(-1)};
			for (int i{ static_cast<int>(maxIdx - 1) }; i >= 0; --i)
			{
				if (!m_bytes.at(i).has_value())
				{
					maxShift = maxIdx - i;
					wildCardIdx = i;
					break;
				}
			}

			//Store max shiftable bytes for non wildcards.
			std::size_t shiftTable[UINT8_MAX + 1]{};
			for (std::size_t i{}; i <= UINT8_MAX; ++i)
			{
				shiftTable[i] = maxShift;
			}

			//Fill shift table with sig bytes
			for (std::size_t i{wildCardIdx + 1}; i != maxIdx; ++i)
			{
				shiftTable[*m_bytes[i]] = maxIdx - i;
			}

			//Loop data
			const auto scan_end = mod.begin() - m_bytes.size();
			for (std::size_t current_idx{}; current_idx <= scan_end;)
			{
				for (std::ptrdiff_t sig_idx{static_cast<std::ptrdiff_t>(maxIdx)}; sig_idx >= 0; --sig_idx)
				{
					if (m_bytes[sig_idx] && *mod.begin().add(current_idx + sig_idx).as<uint8_t*>() != *m_bytes[sig_idx])
					{
						current_idx += shiftTable[*mod.begin().add(current_idx + maxIdx).as<uint8_t*>()];
						break;
					}
					if (sig_idx == NULL)
					{
						return mod.begin().add(current_idx);
					}
				}
			}

			return {};
		}

		std::vector<mem> getAll(hmod mod = {})
		{
			std::vector<mem> ptrs{};
			int64_t elementArraySize{ int64_t(m_bytes.size()) };
			int64_t maxShift{ elementArraySize };
			int64_t maxIdx{ maxShift - 1 };

			//Get wildcard index, and store max shifable byte count
			int64_t wildCardIdx{ -1i64 };

			for (int64_t i{ maxIdx - 1 }; i >= 0; i--)
			{
				if (!m_bytes[i])
				{
					maxShift = maxIdx - i;
					wildCardIdx = i;
					break;
				}
			}

			//Store max shiftable bytes for non wildcards.
			int64_t shiftTable[UINT8_MAX + 1]{};
			for (int64_t i{}; i != UINT8_MAX + 1; ++i)
			{
				shiftTable[i] = maxShift;
			}

			for (int64_t i{ wildCardIdx + 1 }; i != maxIdx; ++i)
			{
				shiftTable[*m_bytes[i]] = maxIdx - i;
			}

			//Loop data
			for (int64_t curIdx{}; curIdx != mod.size() - elementArraySize;)
			{
				for (int64_t sigIdx{ maxIdx }; sigIdx >= 0; sigIdx--)
				{
					if (m_bytes[sigIdx] && *mod.begin().add(curIdx + sigIdx).as<uint8_t*>() != *m_bytes[sigIdx])
					{
						curIdx += shiftTable[*mod.begin().add(curIdx + maxIdx).as<uint8_t*>()];
						break;
					}
					if (!sigIdx)
					{
						ptrs.push_back(mod.begin().add(curIdx));
					}
				}
			}
			return ptrs;
		}

		std::vector<mem> getAllLegacy(hmod mod = {})
		{
			std::vector<mem> ptrs{};
			for (uintptr_t i{}; i < mod.size() - m_bytes.size(); ++i)
			{
				if (doesMemoryMatch(mod.begin().add(i).as<uint8_t*>(), m_bytes.data(), m_bytes.size()))
				{
					ptrs.push_back(mod.begin().add(i));
				}
			}
			return ptrs;
		}

		std::vector<std::optional<uint8_t>> m_bytes;

		std::string rawPatternString;
	};

	struct ptrEntry
	{
		std::string name;
		signature pattern;
		std::function<void(mem)> callback;
	};

	inline uint32_t g_foundSigCount{}, g_failedSigCount{}, g_totalSigCount{};

	class patternBatch
	{
	public:

		void add(std::string name_, std::string_view ptr, std::function<void(mem)> callback_)
		{
			m_patterns.push_back(ptrEntry{ name_, ptr, callback_ });
		}

		void run(hmod mod = {})
		{
			for (auto& [name, pattern, callback] : m_patterns)
			{
				//g_logger->send(logColor::lightBlue, "Patterns", "Scanning for pattern {:02} of {}", ++g_totalSigCount, m_patterns.size());

				if (auto res = pattern.get(mod))
				{
					callback(res);
					g_foundSigCount++;
				}
				else
				{
					if (name.empty())
					{
						g_logger->send(logColor::lightBlue, "Patterns", "Failed to find {}", name);
					}
					else
					{
						g_logger->send(logColor::lightBlue, "Patterns", "Failed to find #{}", g_totalSigCount);
					}

					g_failedSigCount++;
				}
			}
			m_patterns.clear();
		}

	public:
		std::vector<ptrEntry> m_patterns;
	};
}