#include "./core/SocketInterface.h"

int main(int argc, char** argv)
{
	SocketInterface::getInstance()->Initialize("127.0.0.1", 8888, "hello", "mac");
	return 0;
}