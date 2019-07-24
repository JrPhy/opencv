#include <iostream>
#include <cmath>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <string>

int main()
{
    cv::Mat frame, gray, dst, roi;
    cv::Vec4i l;
    std::vector<cv::Vec4i> lines;
    std::vector<cv::Point2f> right, left;
    cv::Vec4f rightLine, leftLine;
    float slopeLow = 0.4, slopeHigh = 1.5;
    int lineLength = 200;
    float x0, y0, x1, y1, k, leftSlope, rightSlope;

    cv::VideoCapture cap("test.mp4");
    while(1)
    {
        cap >> frame;
        if (frame.empty())
            break;
        // Display the resulting frame
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray,gray,cv::Size(3, 3),0);
        cv::Canny(gray, dst, 40, lineLength);
        for(int i=0; i<0.6*dst.rows; i++) for(int j=0; j<dst.cols; j++) dst.at<uchar>(i,j) = 0;// setting ROI
        // detect the lines
        HoughLinesP(dst, lines, 1, CV_PI/180, 50, 30, 10 );

        for(size_t i = 0; i < lines.size(); i++ )
        {
            l = lines[i];
            float m = ((float)(l[3]-l[1])/(float)(l[2]-l[0]));
            if( m > slopeLow )
            {
                if( m > slopeHigh )
                {
                    cv::line( frame, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, cv::LINE_AA, 0);
                    left.push_back(cv::Point2f(l[0], l[1]));
                    left.push_back(cv::Point2f(l[2], l[3]));
                    cv::fitLine(left, leftLine, cv::DIST_L2,0,0.01,0.01);
                    x0 = leftLine[2];
                    y0 = leftLine[3];
                    leftSlope = leftLine[1]/leftLine[0];
                    k = y0-leftSlope*x0; //calculate y = mx+k
                    y0 = frame.rows*0.6;
                    x0 = (y0-k)/leftSlope;
                    y1 = frame.rows;
                    x1 = (y1-k)/leftSlope;
                }
                else
                {
                    cv::line( frame, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,0,0), 3, cv::LINE_AA, 0);
                    left.push_back(cv::Point2f(l[0], l[1]));
                    left.push_back(cv::Point2f(l[2], l[3]));
                    cv::fitLine(left, leftLine, cv::DIST_L2,0,0.01,0.01);
                    x0= leftLine[2];
                    y0= leftLine[3];
                    leftSlope = leftLine[1]/leftLine[0];
                    k = y0-leftSlope*x0;
                    y0 = frame.rows*0.6;
                    x0 = (y0-k)/leftSlope;
                    y1 = frame.rows;
                    x1 = (y1-k)/leftSlope;
                }
            }
            leftSlope = fabs(leftSlope);
            if( slopeLow > leftSlope) continue;
            else if (leftSlope > slopeHigh)
                cv::line( frame, cv::Point(x0,y0),cv::Point(x1,y1), cv::Scalar(0,0,255), 3, cv::LINE_AA, 0);
            else
                cv::line( frame, cv::Point(x0,y0),cv::Point(x1,y1), cv::Scalar(255,0,0), 3, cv::LINE_AA, 0);


            if( m < -slopeLow)
            {
                if( m < -slopeHigh)
                {
                    cv::line( frame, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, cv::LINE_AA, 0);
                    right.push_back(cv::Point2f(l[0], l[1]));
                    right.push_back(cv::Point2f(l[2], l[3]));
                    cv::fitLine(right, rightLine, cv::DIST_L2,0,0.01,0.01);
                    x0= rightLine[2];        // a point on the line
                    y0= rightLine[3];
                    rightSlope = rightLine[1]/rightLine[0];
                    k = y0-rightSlope*x0;
                    y0 = frame.rows*0.6;
                    x0 = (y0-k)/rightSlope;
                    y1 = frame.rows;
                    x1 = (y1-k)/rightSlope;
                }
                else
                {
                    cv::line( frame, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,0,0), 3, cv::LINE_AA, 0);
                    right.push_back(cv::Point2f(l[0], l[1]));
                    right.push_back(cv::Point2f(l[2], l[3]));
                    cv::fitLine(right, rightLine, cv::DIST_L2,0,0.01,0.01);
                    x0= rightLine[2];        
                    y0= rightLine[3];
                    rightSlope = rightLine[1]/rightLine[0];
                    k = y0-rightSlope*x0;
                    y0 = frame.rows*0.6;
                    x0 = (y0-k)/rightSlope;
                    y1 = frame.rows;
                    x1 = (y1-k)/rightSlope;
                }
            }
            rightSlope = fabs(rightSlope);
            if( slopeLow > rightSlope) continue;
            else if (rightSlope > slopeHigh)
                cv::line( frame, cv::Point(x0,y0),cv::Point(x1,y1), cv::Scalar(0,0,255), 3, cv::LINE_AA, 0);
            else
                cv::line( frame, cv::Point(x0,y0),cv::Point(x1,y1), cv::Scalar(255,0,0), 3, cv::LINE_AA, 0);

            while(right.size() > 40) right.erase (right.begin(),right.begin()+32);
            while(left.size() > 40) left.erase (left.begin(),left.begin()+32);
            //both while are for fitline, because I use push_back, the elements in the vector will increase, 
            //so I use it to fix their length.
        }
        // Press  ESC on keyboard to exit
        cv::namedWindow( "Source", cv::WINDOW_AUTOSIZE);
        cv::imshow( "Source", frame );
        char c = (char) cv::waitKey(1);
        if(c==27)
            break;
    }
    cap.release();// When everything done, release the video capture object
    cv::destroyAllWindows();// Closes all the frames*/
    return 0;
}
