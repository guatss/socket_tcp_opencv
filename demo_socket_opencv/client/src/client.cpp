#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include "opencv2/video.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

const int PORT = 6670;
const char *IP = "127.0.0.1";
struct sockaddr_in server_addr;
const int Width = 640;//kich thuoc anh
const int Heigth = 360;
void SendImage(const cv::Mat &mat, int sockfd);
cv::Mat GetImage(int sockfd);
void InitAddr();

int main(int argc, char **argv)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    InitAddr();
    int ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1)
    {
        perror("connect error");
        exit(1);
    }
    std::cout << "connect succedd" << std::endl;
    cv::VideoCapture cap("/home/staug/project/demo_socket_opencv/client/common/video.avi");//read video
    if (!cap.isOpened())
    {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }
    int frames_number = cap.get(cv::CAP_PROP_FRAME_COUNT);//kiem tra so luong frame
    cv::Mat frames;
    std::string filePath = "/home/staug/project/demo_socket_opencv/client/frames/";//folder luu frame
    std::string filePathrecv = "/home/staug/project/demo_socket_opencv/client/frames_recv/";//folder nhan frame tu server
    int number = 0;
    int num = 0;
    std::string fileStyle = ".jpg";
    for (int i = 0; i < frames_number; i++)
    {
        std::string path = " ";
        path = filePath + std::to_string(i) + fileStyle;
        cap.read(frames);
        if (frames.empty())
        {
            std::cerr << "ERRORR\n";
            break;
        }
        else
        {
            cv::imwrite(path, frames);//ghi frame vao folder Frames
        }
        cv::Mat Srcimage = cv::imread(path, 1);
        SendImage(Srcimage, sockfd);//gui frames den server
        close(ret);
    }
    for (int j = 0; j < frames_number; j++)
    {
        std::string path = " ";
        path = filePathrecv + std::to_string(j) + fileStyle;
        cv::Mat mat = cv::Mat::zeros(Heigth, Width, CV_8UC3);
        const int imgSize = mat.total() * mat.elemSize();
        uchar bufferData[imgSize * 2];
        int bytes = 0;
        for (int i = 0; i < imgSize; i += bytes)
        {
            bytes = recv(sockfd, bufferData + i, imgSize - i, 0);//nhan frames tu server
        }
        mat = cv::Mat(Heigth, Width, CV_8UC3, bufferData);
        cv::imwrite(path, mat);//ghi frames nhan duoc vao folder Frames_recv
        close(sockfd);
    }
    return 0;
}
void InitAddr()
{
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    int ret = inet_pton(AF_INET, IP, &server_addr.sin_addr.s_addr);
    if (ret == -1)
    {
        perror("inet_pton error");
        exit(EXIT_FAILURE);
    }
}
void SendImage(const cv::Mat &mat, int sockfd)
{
    int ret = send(sockfd, mat.data, mat.total() * mat.elemSize(), 0);
    if (ret == -1)
    {
        perror("send error");
        exit(EXIT_FAILURE);
    }
}

cv::Mat GetImage(int sockfd)
{
    cv::Mat mat = cv::Mat::zeros(Heigth, Width, CV_8UC3);
    const int imgSize = mat.total() * mat.elemSize();
    uchar bufferData[imgSize * 2];
    int bytes = 0;
    for (int i = 0; i < imgSize; i += bytes)
    {
        bytes = recv(sockfd, bufferData + i, imgSize - i, 0);
        if (bytes == -1)
        {
            std::cout << "status == -1 error == " << errno << "in socket::recv\n";
            exit(EXIT_FAILURE);
        }
    }
    mat = cv::Mat(Heigth, Width, CV_8UC3, bufferData);
    return mat;
}