#ifndef PHOTO_MANAGER_GUI_H_
#define PHOTO_MANAGER_GUI_H_

#include <vector>
#include <opencv2/core/core.hpp>

class PhotoDatabase;

class GUI
{

public:

    GUI(PhotoDatabase* db, unsigned int photo_idx = 0);

    ~GUI();

    void run();

private:

    PhotoDatabase* db_;

    unsigned int photo_idx_;


//    // Cache (circular buffer)

//    unsigned int cache_current_idx_;

//    unsigned int cache_break_idx_;

//    std::vector<cv::Mat> photo_cache_;

};

#endif
