#include <iostream>
#include <cmath>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>

int main()
{
    cv::Mat frame, gray, dst, roi;
    cv::VideoCapture cap("night.mp4");
    while(1)
    {
        cap >> frame;
        if (frame.empty())
            break;
        // Display the resulting frame
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray,gray,cv::Size(3, 3),0);
        cv::Canny(gray, dst, 50, 500);
        for(int i=0; i<0.6*dst.rows; i++) for(int j=0; j<dst.cols; j++) dst.at<uchar>(i,j) = 0;// setting ROI
        std::vector<cv::Vec4i> lines;
        // detect the lines
        HoughLinesP(dst, lines, 1, CV_PI/180, 50, 30, 10 );
        for( size_t i = 0; i < lines.size(); i++ )
        {
            cv::Vec4i l = lines[i];
            float m = fabs((float)(l[3]-l[1])/(float)(l[2]-l[0]));
            if( m > 0.3)
            {
                if( m > 1.3 ) cv::line( frame, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, cv::LINE_AA, 0);
                else cv::line( frame, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,0,0), 3, cv::LINE_AA, 0);
            }

        }
        // Press  ESC on keyboard to exit
        cv::namedWindow( "Source", cv::WINDOW_AUTOSIZE);
        cv::imshow( "Source", frame );
        char c = (char) cv::waitKey(1);
        if(c==27)
            break;
    }
    cap.release();// When everything done, release the video capture object
    cv::destroyAllWindows();// Closes all the frames
    return 0;
}
