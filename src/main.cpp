#include <iostream>
#include <set>
#include <boost/filesystem.hpp>

#include "photo_database.h"
#include "gui.h"
#include "md5sum.h"

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
    db.setPhotoPrefixPath(image_dir);

//    for(int i = 100; i < 105; ++i)
//    {
//        cv::Mat img = cv::imread(image_dir + "/" + db.photos()[i].rel_filename);
//        double f = std::min(800.0 / img.cols, 600.0 / img.rows);
//        cv::Mat img_resized;
//        cv::resize(img, img_resized, cv::Size(), f, f);
//        cv::imshow("photo", img_resized);
//        cv::waitKey();
//    }

    GUI gui(&db);
    gui.run();

//    boost::filesystem::path p(image_dir);
//    boost::filesystem::recursive_directory_iterator it_dir(p);
//    boost::filesystem::recursive_directory_iterator end;

//    std::set<std::string> photo_exts;
//    photo_exts.insert(".jpg");
//    photo_exts.insert(".png");
//    photo_exts.insert(".jpeg");

//    std::set<std::string> found_exts;

//    int i_photo = 0;
//    while(it_dir != end)
//    {
//        if (boost::filesystem::is_regular_file(*it_dir))
//        {
//            if (it_dir->path().filename().string()[0] != '.')
//            {
//                std::string ext = it_dir->path().extension().string();
//                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

//                found_exts.insert(ext);

//                if (photo_exts.find(ext) != photo_exts.end())
//                {
//                    std::string abs_filename = it_dir->path().string();
//                    std::string rel_filename = abs_filename.substr(image_dir.size() + 1);
//                    std::string msum = md5sum(abs_filename);

//                    PhotoData* p = db.findPhoto(msum);
//                    if (!p)
//                    {
//                        // New photo
//                        std::cout << "New photo: " << rel_filename << std::endl;
//                        p = db.addPhoto();
//                        p->md5sum = msum;
//                        p->rel_filename = rel_filename;
//                        db.registerPhoto(p);
//                    }
//                    else
//                    {
//                        // Old photo, update filename
//                        p->rel_filename = rel_filename;
//                    }

//                    ++i_photo;
//                    if (i_photo % 100 == 0)
//                        std::cout << i_photo << " photos scanned" << std::endl;

////                    std::cout << md5sum(abs_filename) << ": " << abs_filename << std::endl;
//                }
//            }
//        }

//        ++it_dir;
//    }

    writeDatabase(db, database_filename);

    return 0;
}
