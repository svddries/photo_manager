#include "md5sum.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <openssl/md5.h>

std::string md5sum(const std::string& filename)
{
    int file_descript = open(filename.c_str(), O_RDONLY);
    if(file_descript < 0)
        return "";

    struct stat statbuf;
    if (fstat(file_descript, &statbuf) < 0)
        return "";

    unsigned long file_size = statbuf.st_size;

    char* file_buffer = (char*)mmap(0, file_size, PROT_READ, MAP_SHARED, file_descript, 0);

    unsigned char result[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)file_buffer, file_size, result);

    static char hex[] = "0123456789abcdef";

    std::string res;
    res.resize(MD5_DIGEST_LENGTH * 2);

    for(int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        int b = result[i];
        res[i * 2] = hex[b / 16];
        res[i * 2 + 1] = hex[b % 16];
    }

    close(file_descript);

    return res;
}
