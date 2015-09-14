#include "Logger.h"
#include <fstream>
#include "Vertices.h"
#include "common.h"
#include <string>

namespace engine{
	Logger *Logger::current = _n;
	Logger *Logger::Main(){
		if(Logger::current == _n){
			Logger::current = new Logger();
			Logger::current->file(L"dungeon.log");
			Logger::current->enable();
		}
		return Logger::current;
	}
	void Logger::Release(){
		if(Logger::current != _n){
			delete Logger::current;
		}
	}
	void Logger::file(const wchar_t *name){
		if(!this->enabled)
			return;
		this->out = new std::wofstream(name);
	}
	void Logger::log(const wchar_t *value){
		if(!this->enabled)
			return;
		this->prefix();
		*out << value;
		this->suffix();
	}
	void Logger::log(_ value){
		if(!this->enabled)
			return;
		this->prefix();
		*out << value;
		this->suffix();
	}
	void Logger::log(float value){
		if(!this->enabled)
			return;
		this->prefix();
		*out << value;
		this->suffix();
	}
	void Logger::log(_vector3 *value){
		if(!this->enabled)
			return;
		this->prefix();
		*out << L"(" << value->x << ", " << value->y << ", " << value->z << ")";
		this->suffix();
	}
	void Logger::log(simple_vertex *value){
		if(!this->enabled)
			return;
		this->prefix();
		this->disable_info();
		this->log(&value->position);
		this->log(L", ");
		this->log(&value->normal);
		this->log(L"\n");
		this->enable_info();
		this->suffix();
	}
	void Logger::log(textured_vertex *value){
		if(!this->enabled)
			return;
		this->prefix();
		this->disable_info();
		this->log(&value->position);
		this->log(L", ");
		this->log(&value->normal);
		this->log(L"\n");
		this->enable_info();
		this->suffix();
	}
	Logger::Logger(){
		this->out = _n;
		this->enabled = true;
		this->mode = Normal;
		this->info = true;
		this->deep = 0;
	}
	Logger::~Logger(){
		if(this->out != _n){
			*out << std::endl;
			delete this->out;
		}
	}
	void Logger::enable(){
		this->enabled = true;
	}
	void Logger::disable(){
		this->enabled = false;
	}
	void Logger::begin(Mode mode){
		switch(this->mode){
			case List:
				this->list_mode_start = true;
				this->mode = mode;
				break;
			case Normal:
				this->mode = mode;
				break;
		}
	}
	void Logger::end(){
		switch(this->mode){
			case List:
				this->list_mode_start = true;
				break;
		}
		this->mode = Normal;
	}
	void Logger::prefix(){
		if(!this->info)
			return;
		switch(this->mode){
			case Normal:
				*out << this->indentation();
				return;
			case List:
				if(!this->list_mode_start)
					*out <<  L", ";
				else
					this->list_mode_start = false;
				return;
		}
	}
	void Logger::suffix(){
		if(!this->info)
			return;
	}
	void Logger::disable_info(){
		this->info = false;
	}
	void Logger::enable_info(){
		this->info = true;
	}
	void Logger::add(){
		this->deep++;
	}
	void Logger::sub(){
		this->deep--;
	}
	std::wstring Logger::indentation(){
		std::wstring result(L"");
		for(_ i = 0; i < this->deep; i++){
			result.push_back(L'\t');
		}
		return result;
	}
}