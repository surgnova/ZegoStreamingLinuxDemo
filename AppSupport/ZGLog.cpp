#include "ZGLog.h"

#include <string>
#include <stdarg.h>

void ZGLog(const char * format, ...)
{
    char log_buf[1024] = { 0 };
    va_list la;
    va_start(la, format);
    //vsprintf_s(log_buf, format, la);
    vsnprintf(log_buf, 1024, format, la);
    va_end(la);

    std::string log_str = std::string(log_buf) + "\r\n";

    printf(log_str.c_str());
}
