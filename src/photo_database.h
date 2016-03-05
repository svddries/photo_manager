#ifndef _PHOTO_DATABASE_H_
#define _PHOTO_DATABASE_H_

#include <vector>
#include <string>
#include <set>
#include <map>

#include <algorithm>

typedef unsigned long Id;

// ----------------------------------------------------------------------------------------------------

struct PhotoData
{
    PhotoData(Id id) : id_(id), done_(false) {}

    std::string md5sum;
    std::string rel_filename;

    void addTag(Id tag)
    {
        tags_.insert(tag);
    }

    Id id() const { return id_; }

    const std::set<Id>& tags() const { return tags_; }

    bool hasTag(Id tag) const { return tags_.find(tag) != tags_.end(); }

    void setDone(bool b = true) { done_ = b; }

    bool isDone() const { return done_; }

private:

    Id id_;

    std::set<Id> tags_;

    bool done_;

//    time_t last_seen_;


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
        filename_to_photo_[p->rel_filename] = p->id();
    }

    PhotoData* findPhoto(const std::string& md5sum)
    {
        auto it = md5sum_to_photo_.find(md5sum);
        if (it != md5sum_to_photo_.end())
            return &photos_[it->second];
        else
            return nullptr;
    }

    PhotoData* findPhotoByFilename(const std::string& filename)
    {
        auto it = filename_to_photo_.find(filename);
        if (it != filename_to_photo_.end())
            return &photos_[it->second];
        else
            return nullptr;
    }

    void addConcept(const std::string& concept, Id id)
    {
        if (id >= concepts_.size())
            concepts_.resize(id + 1);
        concepts_[id] = concept;
        concept_to_id_[concept] = id;
    }

    Id addConcept(const std::string& concept)
    {
        for(Id i = 0; i < concepts_.size(); ++i)
        {
            if (concepts_[i].empty())
            {
                addConcept(concept, i);
                return i;
            }
        }

        Id id = concepts_.size();
        addConcept(concept, id);
        return id;
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

    bool conceptExists(const std::string& concept) const
    {
        return concept_to_id_.find(concept) != concept_to_id_.end();
    }

    std::vector<PhotoData>& photos() { return photos_; }

    const std::vector<PhotoData>& photos() const { return photos_; }

    void setPhotoPrefixPath(const std::string& path)
    {
        photo_prefix_path_ = path;
        if (photo_prefix_path_.back() != '/')
            photo_prefix_path_ += '/';
    }

    const std::string& photoPrefixPath() const { return photo_prefix_path_; }

private:

    std::vector<PhotoData> photos_;

    std::map<std::string, Id> md5sum_to_photo_;

    std::map<std::string, Id> filename_to_photo_;

    std::vector<std::string> concepts_;

    std::map<std::string, Id> concept_to_id_;

    std::string photo_prefix_path_;

};

// ----------------------------------------------------------------------------------------------------

bool loadDatabase(const std::string filename, PhotoDatabase& db);

void writeDatabase(const PhotoDatabase& db, const std::string& filename);

#endif
