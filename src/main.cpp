#include <iostream>
#include <set>

// Visualization
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// File operations
#include "boost/filesystem.hpp"
#include <fstream>

// MD5sum
#include "md5sum.h"

typedef unsigned long Id;

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

struct PhotoData
{
    PhotoData(Id id) : id_(id), tags_complete(false), starred(false) {}

    std::string md5sum;
    std::string rel_filename;
    bool tags_complete;
    bool starred;

    void addTag(Id tag)
    {
        if (tags_.empty() || tags_.back() < tag)
        {
            tags_.push_back(tag);
        }
        else
        {
            tags_.push_back(tag);
            std::sort(tags_.begin(), tags_.end());
        }
    }

    Id id() const { return id_; }

    const std::vector<Id>& tags() const { return tags_; }

private:

    Id id_;

    std::vector<Id> tags_;
};

// ----------------------------------------------------------------------------------------------------

class PhotoDatabase
{

public:

    PhotoData* addPhoto()
    {
        Id id = photos_.size();
        photos_.push_back(PhotoData(id));
        return &photos_.back();
    }

    void registerPhoto(PhotoData* p)
    {
        md5sum_to_photo_[p->md5sum] = p->id();
    }

    PhotoData* findPhoto(const std::string& md5sum)
    {
        auto it = md5sum_to_photo_.find(md5sum);
        if (it != md5sum_to_photo_.end())
            return &photos_[it->second];
        else
            return nullptr;
    }

    void addConcept(const std::string& concept, Id id)
    {
        concepts_.resize(id + 1);
        concepts_[id] = concept;
        concept_to_id_[concept] = id;
    }

    const std::string& getConcept(Id id) const
    {
        return concepts_[id];
    }

    bool getConceptId(const std::string& s, Id& id) const
    {
        auto it = concept_to_id_.find(s);
        if (it != concept_to_id_.end())
        {
            id = it->second;
            return true;
        }

        return false;
    }

    const std::vector<std::string>& concepts() const { return concepts_; }

    const std::vector<PhotoData>& photos() const { return photos_; }

private:

    std::vector<PhotoData> photos_;

    std::map<std::string, Id> md5sum_to_photo_;

    std::vector<std::string> concepts_;

    std::map<std::string, Id> concept_to_id_;

};

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

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "usage: photo_manager [DATABASE FILE] [IMAGE DIRECTORY]" << std::endl;
        return 1;
    }

    std::string database_filename = argv[1];
    std::string image_dir = argv[2];

    PhotoDatabase db;
    loadDatabase(database_filename, db);

//    for(int i = 100; i < 105; ++i)
//    {
//        cv::Mat img = cv::imread(image_dir + "/" + db.photos()[i].rel_filename);
//        double f = std::min(800.0 / img.cols, 600.0 / img.rows);
//        cv::Mat img_resized;
//        cv::resize(img, img_resized, cv::Size(), f, f);
//        cv::imshow("photo", img_resized);
//        cv::waitKey();
//    }

    boost::filesystem::path p(image_dir);
    boost::filesystem::recursive_directory_iterator it_dir(p);
    boost::filesystem::recursive_directory_iterator end;

    std::set<std::string> photo_exts;
    photo_exts.insert(".jpg");
    photo_exts.insert(".png");
    photo_exts.insert(".jpeg");

    std::set<std::string> found_exts;

    int i_photo = 0;
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
                    std::string abs_filename = it_dir->path().string();
                    std::string rel_filename = abs_filename.substr(image_dir.size() + 1);
                    std::string msum = md5sum(abs_filename);

                    PhotoData* p = db.findPhoto(msum);
                    if (!p)
                    {
                        // New photo
                        std::cout << "New photo: " << rel_filename << std::endl;
                        p = db.addPhoto();
                        p->md5sum = msum;
                        p->rel_filename = rel_filename;
                        db.registerPhoto(p);
                    }
                    else
                    {
                        // Old photo, update filename
                        p->rel_filename = rel_filename;
                    }

                    ++i_photo;
                    if (i_photo % 100 == 0)
                        std::cout << i_photo << " photos scanned" << std::endl;

//                    std::cout << md5sum(abs_filename) << ": " << abs_filename << std::endl;
                }
            }
        }

        ++it_dir;
    }

    writeDatabase(db, database_filename);

    return 0;
}
