#include "common.h"


std::shared_ptr<logger> logger::auxlog = std::make_shared<logger>("synqeraTest.log");
std::chrono::time_point<std::chrono::system_clock> timer::start_time = std::chrono::system_clock::now();
    

logger::logger(const std::string & filename)
{
    std::cout << "----- create logger -----" << std::endl;
    logfile.open (filename);
    if (!logfile.is_open()) 
        throw std::runtime_error("can't open logfile " + filename);
}

logger::~logger()
{
    std::cout << "----- close logger -----" << std::endl;
    logfile.close();
}

#define BUFFER_SIZE 4096
std::string logger::format(const char *fmt, ...)
{
    char buff[BUFFER_SIZE];
    if (!fmt) return "";
    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buff, BUFFER_SIZE, fmt, arg);
    va_end(arg);
    auto ret = std::string(buff);
    return ret;
}

std::string timer::timestamp()
{
    static char buff[32]; 
    time_t t;
    std::time(&t);
    struct tm *ti = std::localtime(&t);
    std::sprintf(buff,"[%2d:%.2d:%.2d]",ti->tm_hour,ti->tm_min,ti->tm_sec);
    return std::string(buff);
}