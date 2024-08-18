#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <tesseract/publictypes.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>

int main(int argc, char *argv[])
{
    // cv::Mat          img       = cv::imread("./chez_henri.jpg");
    cv::VideoCapture video_cap = cv::VideoCapture(0);

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(NULL, "eng")) {
        std::cout << "Failed to initialise api\n";
        return 1;
    }

    // play around with values
    api->SetPageSegMode(tesseract::PSM_SPARSE_TEXT);

    cv::Mat img;
    while (true) {
        video_cap >> img;

        if (!img.data)
            break;

        api->SetImage(img.data, img.cols, img.rows, 3, img.step);
        api->Recognize(0);

        tesseract::ResultIterator   *ri    = api->GetIterator();
        tesseract::PageIteratorLevel level = tesseract::PageIteratorLevel::RIL_WORD;

        int x1, y1, x2, y2;

        if (ri != nullptr) {
            do {
                ri->BoundingBox(level, &x1, &y1, &x2, &y2);
                float conf = ri->Confidence(level);

                if (conf > 70) {
                    cv::rectangle(img, cv::Point2i(x1, y1), cv::Point2i(x2, y2), cv::Scalar(0, 255, 0));
                }
            } while (ri->Next(level));
        }

        cv::imshow("test", img);
        cv::waitKey(1000 / 60);
    }
    video_cap.release();
    api->End();
    delete api;
}
