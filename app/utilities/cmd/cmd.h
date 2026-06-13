#pragma once

class c_console {
public:
	void attach(const char* title);
	void destroy();

	void print(const char* file, int line, const char* message, ...);
private:
	FILE* m_stream;
};

inline auto console = std::make_shared<c_console>( );

#define log_dbg(...) console->print(__FILE__, __LINE__, __VA_ARGS__)