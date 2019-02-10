#pragma once

typedef enum { RF24_PA_MIN = 0, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR } rf24_pa_dbm_e;

int millis() { return 0; }

class RF24
{
public:

	bool begin( int a, int b ) { return true; }
	void setPALevel( int a ) { }

	void openWritingPipe( const void* ) { }
	void openReadingPipe( int, const void* ) { }

	void startListening() {}
	void stopListening() { }

	bool available() { return false; }

	void read( const void*, int ) { }
	void write( const void*, int ) { }
};

class SerialInterface
{
public:

	void print( const char* ) { }
	void print( conct::uintreg ) { }
	void println( const char* ) { }
	void println( conct::uintreg ) { }
};

static SerialInterface Serial;
