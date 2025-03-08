#pragma once
#include "Common.hpp"
namespace fs = std::filesystem;
enum logColor : std::uint16_t {
	red = FOREGROUND_RED,
	green = FOREGROUND_GREEN,
	blue = FOREGROUND_BLUE,
	intensify = FOREGROUND_INTENSITY,
	white = red | green | blue | intensify,
	grey = intensify,
	lightRed = red | intensify,
	lightGreen = green | intensify,
	lightBlue = blue | intensify,
	Black = 0,
	Darkblue = 0x0001,
	Darkgreen = 0x0002,
	Darkcyan = 0x0002 | 0x0001,
	Darkred = 0x0004,
	Darkmagenta = 0x0004 | 0x0001,
	Darkyellow = 0x0004 | 0x0002,
	Darkgray = 0x0004 | 0x0002 | 0x0001,
	//Gray = 0x0008,
	//Blue = 0x0008 | 0x0001,
	//Green = 0x0008 | 0x0002,
	Cyan = 0x0008 | 0x0002 | 0x0001,
	//Red = 0x0008 | 0x0004,
	Magenta = 0x0008 | 0x0004 | 0x0001,
	Yellow = 0x0008 | 0x0004 | 0x0002,
	//White = 0x0008 | 0x0004 | 0x0002 | 0x0001,
};
inline logColor operator|(logColor a, logColor b) {
	return static_cast<logColor>(static_cast<std::underlying_type_t<logColor>>(a) | static_cast<std::underlying_type_t<logColor>>(b));
}
namespace base {
	class logger {
	public:
		logger() {
			if (!AttachConsole(GetCurrentProcessId()))
				AllocConsole();
			SetConsoleTitleA(brandingName);
			freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
			SetConsoleCP(CP_UTF8);
			SetConsoleOutputCP(CP_UTF8);
			m_console.open("CONOUT$");
			try {
				m_path.append("C:\\");
				m_path.append("LightningFiles");
				if (!fs::exists(m_path))
					fs::create_directory(m_path);
				m_path.append(shortBrandingName ".log");
			}
			catch (std::filesystem::filesystem_error const& err) {
				std::cout << "std::filesystem error " << err.what() << '\n';
			}
			m_file.open(m_path, std::ios_base::out | std::ios_base::app);
		}
		~logger() {
			FreeConsole();
			fclose(stdout);
			m_path.clear();
			m_console.close();
			m_file.close();
		}
	public:
		template <typename ...arguments>
		void send(logColor col, std::string type, std::string fmt, arguments... args) {
			auto getTime = std::time(nullptr);
			auto timeStruct = std::localtime(&getTime);
			auto getFormattedTime = std::format("{:02}:{:02}:{:02}", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
			auto msg = getFormattedTime + " | " + type + " | " + std::vformat(fmt, std::make_format_args(args...));
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<uint16_t>(col));
			m_console << msg << std::endl;
			m_file << msg << std::endl;
			m_messages.push_back(msg);
		}
	public:
		std::vector<std::string> m_messages;
	public:
		fs::path m_path;
		std::ofstream m_file;
		std::ofstream m_console;
	};
	inline std::unique_ptr<logger> g_logger;
}