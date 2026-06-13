
void c_console::attach(const char* title)
{
	AllocConsole();
	SetConsoleTitle(title);
	freopen_s(&m_stream, "CONOUT$", "w", stdout);
}

void c_console::destroy()
{
	if (m_stream) {
		fclose(m_stream);
		m_stream = nullptr;
	}
	FreeConsole();
}

void c_console::print(const char* file, int line, const char* message, ...)
{
	va_list va;
	va_start(va, message);

	char f_message[1024];
	vsnprintf(f_message, sizeof(f_message), message, va);
	va_end(va);

	const char* filename = strrchr(file, '\\');
	if (!filename) 
		filename = strrchr(file, '/');
	filename = filename ? filename + 1 : file;

	printf("[%s:%d] %s\n", filename, line, f_message);
}