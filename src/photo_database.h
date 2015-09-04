#ifndef _PHOTO_DATABASE_H_
#define _PHOTO_DATABASE_H_

#include <vector>
#include <string>
#include <map>

#include <algorithm>

typedef unsigned long Id;

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

    std::vector<std::string> concepts_;

    std::map<std::string, Id> concept_to_id_;

    std::string photo_prefix_path_;

};

// ----------------------------------------------------------------------------------------------------

bool loadDatabase(const std::string filename, PhotoDatabase& db);

void writeDatabase(const PhotoDatabase& db, const std::string& filename);

#endif
