#ifndef __logger
#define __logger
#include <fstream>
#include <string>
#include "Vertices.h"
#include "common.h"

namespace engine{
	/*
		Caution: Logger doesn't implement any locking mechanism.
		So you can use Logger only in one thread simultaneously.
	*/
	class Logger{
	public:
		enum Mode{
			Normal,
			List
		};
		static Logger *Main();
		static void Release();
		void file(const wchar_t *name);
		void log(const wchar_t *value);
		void log(_ value);
		void log(float value);
		void log(_vector3 *value);
		void log(simple_vertex *value);
		void log(textured_vertex *value);
		void enable();
		void disable();
		void begin(Mode mode);
		void end();
		void sub();
		void add();
	protected:
		void prefix();
		void suffix();
		void disable_info();
		void enable_info();
		std::wstring indentation();
		Mode mode;
		bool info;
		_ deep;
		std::wofstream *out;
		static Logger *current;
		bool enabled;
		Logger();
		~Logger();
		/*
			List Mode specific fields.
		*/
		bool list_mode_start;
	};
}
#endif