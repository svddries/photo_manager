#include "gui.h"

#include "photo_database.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

// ----------------------------------------------------------------------------------------------------

GUI::GUI(PhotoDatabase* db) : db_(db), photo_idx_(0), cache_current_idx_(0), cache_break_idx_(0)
{
}

// ----------------------------------------------------------------------------------------------------

GUI::~GUI()
{
}

// ----------------------------------------------------------------------------------------------------

void GUI::run()
{
    if (db_->photos().empty())
        return;

    std::string typed;
    std::string concept;

    cv::Mat photo;
    cv::Mat canvas;

    bool reload = true;
    bool redraw = true;

    while(true)
    {
        if (reload)
        {
            const PhotoData& pdata = db_->photos()[photo_idx_];
            cv::Mat img = cv::imread(db_->photoPrefixPath() + pdata.rel_filename);
            double f = std::min(800.0 / img.cols, 600.0 / img.rows);
            cv::resize(img, photo, cv::Size(), f, f);
            reload = false;
            redraw = true;
        }

        if (redraw)
        {
            const PhotoData& pdata = db_->photos()[photo_idx_];

            cv::Mat canvas(photo.rows, photo.cols + 300, CV_8UC3, cv::Scalar(20, 20, 20));

            int y_tag = 20;
            for(Id tag_id : pdata.tags())
            {
                cv::putText(canvas, db_->getConcept(tag_id), cv::Point(photo.cols + 10, y_tag),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 255), 1);
                y_tag += 20;
            }

            photo.copyTo(canvas(cv::Rect(cv::Point(0, 0), cv::Size(photo.cols, photo.rows))));

            cv::putText(canvas, typed, cv::Point(10, 20),  cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(0, 0, 255), 1);
            cv::putText(canvas, concept, cv::Point(10, 40),  cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255, 0, 0), 1);

            cv::imshow("photo", canvas);
        }

        unsigned char key = cv::waitKey();

//        std::cout << (int)key << std::endl;

        if (key == 27)  // ESC
            return;
        else if (key == 81) // Left-Arrow
        {
            if (photo_idx_ > 0)
                --photo_idx_;
            reload = true;
        }
        else if (key == 83) // Right-Arrow
        {
            if (photo_idx_ + 1 < db_->photos().size())
                ++photo_idx_;
            reload = true;
        }
        else if (key == 8) // Backspace
        {
            if (!typed.empty())
                typed.pop_back();
        }
        else if (key == 10) // Enter
        {
            Id tag_id;
            if (!db_->getConceptId(concept, tag_id))
            {
                tag_id = db_->addConcept(concept);
            }

            PhotoData& pdata = db_->photos()[photo_idx_];
            pdata.addTag(tag_id);

            typed.clear();
        }
        else
        {
            typed += key;
        }

        concept.clear();
        if (!typed.empty())
        {
            for(const std::string& s : db_->concepts())
            {
                if (s.size() >= typed.size() && s.substr(0, typed.size()) == typed)
                {
                    concept = s;
                    redraw = true;
                    break;
                }
            }

            if (concept.empty())
                concept = typed;
        }
        else
        {

        }

    }

}
