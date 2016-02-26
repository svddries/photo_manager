#include <iostream>
#include <set>
#include <boost/filesystem.hpp>

#include "photo_database.h"
#include "gui.h"
#include "md5sum.h"

// ----------------------------------------------------------------------------------------------------

void printUsage()
{
    std::cerr << std::endl;
    std::cerr << "usage: photo_manager <DATABASE FILE> <IMAGE DIRECTORY> <COMMAND> [ARGS...]" << std::endl;
    std::cerr <<  std::endl;
    std::cerr << "    gui [PHOTO IDX]                Start annotation gui" << std::endl;
    std::cerr << "    scan                           Scans for images and adds new images to the database" << std::endl;
    std::cerr << "    search <TAG1> - <TAG2> - ...   Search for photos containing all these tags" << std::endl;
    std::cerr << std::endl;
}

// ----------------------------------------------------------------------------------------------------

void search(const PhotoDatabase& db, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        printUsage();
        return;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Parse tags from command-line arguments

    std::vector<std::string> tags;
    tags.push_back("");
    for(unsigned int i = 0; i < args.size(); ++i)
    {
        if (args[i] == "-")
            tags.push_back("");
        else if (tags.back().empty())
            tags.back() += args[i];
        else
            tags.back() += " " + args[i];
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Convert tags to tag ids

    bool all_concepts_exist = true;
    std::vector<Id> tag_ids(tags.size());

    for(unsigned int i = 0 ; i < tags.size(); ++i)
    {
        Id tag_id;
        if (!db.getConceptId(tags[i], tag_id))
        {
            std::cout << "Unknown concept: '" << tags[i] << "'" << std::endl;
            all_concepts_exist = false;
        }

        tag_ids[i] = tag_id;
    }

    if (!all_concepts_exist)
        return;

    // - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Search photos

    for(const PhotoData& photo_data : db.photos())
    {
        bool has_tags = true;
        for(const Id& tag_id : tag_ids)
        {
            if (!photo_data.hasTag(tag_id))
            {
                has_tags = false;
                break;
            }
        }

        if (has_tags)
        {
            std::cout << db.photoPrefixPath() << photo_data.rel_filename << std::endl;
        }
    }
}

// ----------------------------------------------------------------------------------------------------

void scan(PhotoDatabase& db, const std::string& image_dir)
{
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
}

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        printUsage();
        return 1;
    }

    std::string database_filename = argv[1];
    std::string image_dir = argv[2];
    std::string command = argv[3];

    std::vector<std::string> args;
    for(int i = 4; i < argc; ++i)
        args.push_back(argv[i]);

    PhotoDatabase db;
    loadDatabase(database_filename, db);
    db.setPhotoPrefixPath(image_dir);

    if (command == "gui")
    {
        unsigned int photo_idx_start = 0;
        if (args.size() > 0)
            photo_idx_start = atoi(args[0].c_str());

        GUI gui(&db, photo_idx_start);
        gui.run();
    }
    else if (command == "search")
    {
        search(db, args);
    }
    else if (command == "scan")
    {
        scan(db, image_dir);
    }
    else
    {
        std::cout << "Unknown command: " << command << std::endl;
        return 0;
    }

    writeDatabase(db, database_filename);

    return 0;
}
