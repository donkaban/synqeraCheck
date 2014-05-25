#ifndef _SYNQERA_TESTER_COMMON_
#define _SYNQERA_TESTER_COMMON_

#include <chrono>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

using uint = unsigned int;

template <typename F, typename ... A>
static float trace(F && f, A &&... args)
{
    auto start = std::chrono::system_clock::now();
    std::bind(std::forward<F>(f),std::forward<A>(args)...)();
    std::chrono::duration<float> dt(std::chrono::system_clock::now() - start);
    return dt.count();
} 
struct timer
{
public:	
 	static float get() 
    {
        std::chrono::duration<float> dt(std::chrono::system_clock::now() - start_time);
        return dt.count();
    }
    static std::string timestamp();

private:
	static std::chrono::time_point<std::chrono::system_clock> start_time;
};

template<class T>
void replace(T& src, const T& find, const T& replace)
{
    size_t fLen = find.size();
    size_t rLen = replace.size();
    for (size_t pos = 0; (pos=src.find(find, pos))!=T::npos; pos+=rLen)
        src.replace(pos, fLen, replace);
}

class logger
{
public:	
	logger(const std::string &);
	~logger();
	template<typename ... T>
    void message(const char* fmt, T && ... args)
    {
    	auto msg = format(fmt, args...);
       	std::cout << msg << std::endl;
    	logfile << msg << std::endl;
    }
	template<typename ... T>
    static void info(const char* fmt, T && ... args)
    {
    	 logger::auxlog->message(fmt,args...);
    }
	template<typename ... T>
	static void fatal(const char* fmt, T && ... args)
    {
  	 	logger::auxlog->message(fmt,args...);
    	exit(-1);
    }
private:
	std::string format(const char *,...);
	static std::shared_ptr<logger> auxlog;
	std::ofstream   			   logfile;
};




#endif