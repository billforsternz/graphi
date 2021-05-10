//
//  Very simple Windows COM port routines
//

#include "uart.h"

// eg open( "COM1", 115200, 1, 'N' );
bool Uart::open( const char *port, int baud, int stop, char parity )
{
    if( connected )
        close();
    bool ok = false;
    char sport[80];
    strcpy( sport, "\\\\.\\" );
    strcat( sport, port );  // eg "\\\\.\\COM1"
    handle = CreateFileA( static_cast<LPCSTR>(sport),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);
    if( handle != INVALID_HANDLE_VALUE )
    {
        connected = true;
        pending = false;
        DCB dcbSerialParameters;
        if( GetCommState( handle, &dcbSerialParameters) )
        {
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.fBinary = TRUE;
            switch( baud )
            {
				default:
                case 115200:    dcbSerialParameters.BaudRate = CBR_115200;  break;
                case 1200:      dcbSerialParameters.BaudRate = CBR_1200;    break;
                case 2400:      dcbSerialParameters.BaudRate = CBR_2400;    break;
                case 4800:      dcbSerialParameters.BaudRate = CBR_4800;    break;
                case 9600:      dcbSerialParameters.BaudRate = CBR_9600;    break;
                case 19200:     dcbSerialParameters.BaudRate = CBR_19200;   break;
                case 38400:     dcbSerialParameters.BaudRate = CBR_38400;   break;
                case 57600:     dcbSerialParameters.BaudRate = CBR_57600;   break;
            }
			switch( stop )
			{
				default:
				case 1: dcbSerialParameters.StopBits = ONESTOPBIT;  break;
				case 2: dcbSerialParameters.StopBits = TWOSTOPBITS; break;
			}
			switch( parity )
			{
				default:
				case 'N':
				case 'n':	  dcbSerialParameters.Parity = NOPARITY;   break;
				case 'E':
				case 'e':	  dcbSerialParameters.Parity = EVENPARITY; break;
				case 'O':
				case 'o':	  dcbSerialParameters.Parity = ODDPARITY;  break;
			}
            if( SetCommState(handle, &dcbSerialParameters) )
                ok = true;
        }
    }
    return ok;
}

// Poll serial port, return bool characters available
bool Uart::poll()
{
    ClearCommError( handle, &errors, &status);
    return status.cbInQue > 0;
}

// Reading bytes from serial port to buffer;
// returns read bytes count, or if error occurs, returns 0
int Uart::read( const char *buffer, unsigned int buf_size )
{
    DWORD bytes_read = 0;
    unsigned int nbr_to_read = 0;

    ClearCommError( handle, &errors, &status);
    if( status.cbInQue > 0 )
    {
        if( status.cbInQue > buf_size )
            nbr_to_read = buf_size;
        else
            nbr_to_read = status.cbInQue;
    }
    if( ReadFile( handle, (void*)buffer, nbr_to_read, &bytes_read, NULL ) )
        return bytes_read;
    return 0;
}

// Write data to serial port;
// returns bool ok
bool Uart::write( const char *buffer, unsigned int buf_size )
{
    DWORD bytes_sent;
    if( !WriteFile(handle, (void*)buffer, buf_size, &bytes_sent, 0) )
        return false;
    return true;
}

void Uart::close()
{
    if( connected )
    {
        CloseHandle(handle);
        connected = false;
        pending = false;
    }
}

