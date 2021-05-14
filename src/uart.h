#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED


#include <Windows.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>

class Uart
{
private:
    HANDLE handle;
    bool connected = false;
    COMSTAT status;
    DWORD errors;
    OVERLAPPED overlap;
    bool pending = false;

public:
    ~Uart() { close(); }
    bool open( const char *port, int baud, int stop, char parity );  // eg "COM1", 115200, 1, 'N'
    unsigned int poll();
    unsigned int read( const char *buffer, unsigned int buf_size );
    bool write( const char *buffer, unsigned int buf_size );
    void close();
};

#endif // UART_H_INCLUDED
