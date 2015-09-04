#include "photo_database.h"

#include <iostream>
#include <set>

// File operations
#include <fstream>

// MD5sum
#include "md5sum.h"

// ----------------------------------------------------------------------------------------------------

std::string idToStr(Id id)
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

Id strToId(const std::string& s)
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

std::string nextWord(const std::string& s, std::size_t& idx)
{
    std::string w;
    bool quotes = 0;
    for(; idx < s.size(); ++idx)
    {
        char c = s[idx];
        if (c == '"')
        {
            quotes = !quotes;
        }
        else if (!quotes && (c == ' ' || c == '\n'))
        {
            ++idx;
            return w;
        }
        else
        {
            w += c;
        }
    }
    return w;
}

// ----------------------------------------------------------------------------------------------------

bool loadDatabase(const std::string filename, PhotoDatabase& db)
{
    std::ifstream fin(filename.c_str());

    std::string line;

    // Load concepts
    while(std::getline(fin, line))
    {
        if (line == "")
            break;

        std::size_t idx = 0;
        std::string id_str = nextWord(line, idx);
        std::string concept = nextWord(line, idx);
//        std::cout << strToId(id_str) << " " << concept << std::endl;

        std::transform(concept.begin(), concept.end(), concept.begin(), ::tolower);

        db.addConcept(concept, strToId(id_str));
    }

    // Load photo data
    while(std::getline(fin, line))
    {
        std::size_t idx = 0;

        PhotoData* p = db.addPhoto();

        p->md5sum = nextWord(line, idx);
        p->rel_filename = nextWord(line, idx);

        while(true)
        {
            std::string id_str = nextWord(line, idx);
            if (id_str.empty())
                break;

            Id tag_id = strToId(id_str);
            p->addTag(tag_id);
        }

        db.registerPhoto(p);
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

void writeDatabase(const PhotoDatabase& db, const std::string& filename)
{
    std::ofstream fout(filename.c_str());

    // Write concepts
    for(unsigned int i = 0; i < db.concepts().size(); ++i)
    {
        const std::string& concept = db.concepts()[i];
        if (concept.empty())
            continue;

        fout << idToStr(i) << " \"" << concept << "\"" << std::endl;
    }

    fout << std::endl;

    for(unsigned int i = 0; i < db.photos().size(); ++i)
    {
        const PhotoData& p = db.photos()[i];
        fout << p.md5sum << " \"" << p.rel_filename << "\"";
        for(unsigned int j = 0; j < p.tags().size(); ++j)
        {
            fout << " " << idToStr(p.tags()[j]);
        }
        fout << std::endl;
    }
}
