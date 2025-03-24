#include "frame.h"
#include "MotionVector.h"
#include "Residual.h"
#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>


Frame convertMattoFrame(const cv::Mat &mat, FrameType type){
    Frame frame(mat.cols, mat.rows, type);

    cv::Mat gray_mat;
    if(mat.channels() == 3){
        cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
    }else{
        gray_mat = mat.clone();
    }

    for(int y = 0; y < frame.height; y++){
        for(int x = 0; x < frame.width; x++){
            frame.setPixel(x, y, gray_mat.at<uchar>(y, x));
        }
    }

    return frame;
}


cv::Mat convertFrametoMat(const Frame &frame){
    cv::Mat mat(frame.height, frame.width, CV_8UC1);
    for(int y = 0; y < frame.height; y++){
        for(int x = 0; x < frame.width; x++){
            mat.at<uchar>(y, x) = frame.getPixel(x, y);
        }
    }
    return mat;
}


int main(int argc, char** argv){
    if(argc < 3){
        std::cerr << "Provide input video and output directory" << std::endl;
        return -1;
    }

    std::string video_path = argv[1];
    std::string output_path = argv[2];

    cv::VideoCapture cap(video_path);
    if(!cap.isOpened()){
        std::cerr << "Cannot open file:" << video_path << std::endl;
        return -1;
    }

    int frameWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frameHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
    double fps = cap.get(cv::CAP_PROP_FPS);

    std::cout << "Video info: " << frameWidth << "x" << frameHeight << ", " << totalFrames << " frames, " << fps << " fps" << std::endl;

    cv::VideoWriter reconstructedWriter(output_path + "/reconstructed_video.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps, cv::Size(frameWidth, frameHeight), false);
    int frameCount = 0;
    cv::Mat first_mat;
    cap >> first_mat;

    if(first_mat.empty()){
        std::cerr << "Failed to read the first frame." << std::endl;
        return -1;
    }

    Frame reference_frame = convertMattoFrame(first_mat, I_frame);
    reconstructedWriter.write(convertFrametoMat(reference_frame));

    while(true){
        std::cout << "Processing frame " << frameCount + 1 << std::endl;
        cv::Mat current_mat;
        cap >> current_mat;

        if(current_mat.empty()){
            break;
        }

        Frame current_frame = convertMattoFrame(current_mat, P_frame);
        std::vector<MotionVector> motion_vectors;
        std::vector<std::vector<int>> residuals;

        encodeP_Frame(current_frame, reference_frame, motion_vectors, residuals);
        Frame reconstructed_frame(current_frame.width, current_frame.height, P_frame);
        decodeP_Frame(motion_vectors, residuals, reference_frame, reconstructed_frame);

        cv::Mat reconstructed_mat = convertFrametoMat(reconstructed_frame);
        reconstructedWriter.write(reconstructed_mat);

        reference_frame = reconstructed_frame;
        frameCount++;
        if (cv::waitKey(30) >= 0) break;
    }
    cap.release();
    reconstructedWriter.release();
    std::cout << "Video processing done" << std::endl;
    return 0;
}