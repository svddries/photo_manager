#include <iostream>
#include <set>

// Visualization
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// File operations
#include "boost/filesystem.hpp"

// MD5sum
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

// ----------------------------------------------------------------------------------------------------

std::string idToStr(unsigned long id)
{
    static char c[] = "0123456789abcdefghijklmnopqrstuvwxyz";

    std::string s;

    unsigned long i = id;
    while(true)
    {
        s += c[i % 36];
        if (i < 36)
            break;
        i /= 36;
    }

    return s;
}

// ----------------------------------------------------------------------------------------------------

unsigned long strToId(const std::string& s)
{
    unsigned long id = 0;
    unsigned long f = 1;
    for(unsigned int i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c <= '9')
            id += f * (c - '0');
        else
            id += f * ((c - 'a') + 10);
        f *= 36;
    }

    return id;
}

// ----------------------------------------------------------------------------------------------------

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

    std::stringstream s;
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++)
        s << std::hex << (int)result[i];

    return s.str();
}

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Please provide image directory" << std::endl;
        return 1;
    }

    std::string image_dir = argv[1];

    boost::filesystem::path p(image_dir);
    boost::filesystem::recursive_directory_iterator it_dir(p);
    boost::filesystem::recursive_directory_iterator end;

    std::set<std::string> photo_exts;
    photo_exts.insert(".jpg");
    photo_exts.insert(".png");
    photo_exts.insert(".jpeg");

    std::set<std::string> found_exts;

    while(it_dir != end)
    {
        if (boost::filesystem::is_regular_file(*it_dir))
        {
            if (it_dir->path().filename().string()[0] != '.')
            {
                std::string ext = it_dir->path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                found_exts.insert(ext);

                if (photo_exts.find(ext) != photo_exts.end())
                {
                    std::string filename = it_dir->path().string();
                    std::cout << filename << std::endl;
                    std::cout << md5sum(filename) << std::endl;
                }

                std::cout << ext << std::endl;
            }
        }

        ++it_dir;
    }

    return 0;
}
