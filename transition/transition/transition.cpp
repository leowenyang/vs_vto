#include <iostream>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

int main(int argc, char* argv[]) {
	printf("%s\n", avcodec_configuration());
	return 0;
}