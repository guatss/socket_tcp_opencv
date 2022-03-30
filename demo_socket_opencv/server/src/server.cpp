#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

const int PORT = 6670;
struct sockaddr_in server_addr, client_addr;
const int Width = 640;
const int Heigth = 360;
cv::Mat GetImage(int sockfd);
void SendImage(const cv::Mat &mat, int sockfd);
void InitAddr(int sockfd);

int main(int argc, char **argv)
{
    char client_IP[1024];
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    InitAddr(sockfd);
    int number = 0;
    std::string filePath = "/home/staug/project/demo_socket_opencv/server/frames/";//folder luu anh nhan tu client
    std::string styefile = ".jpg";
    while (true)
    {
        bzero(&client_addr, sizeof(client_addr));
        socklen_t client_len = sizeof(client_len);
        std::cout << "wait" << std::endl;
        int connectfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (connectfd == -1)
        {
            perror("accept error");
            exit(1);
        }
        printf("client ip:%s port:%d\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_IP, 1024), ntohs(client_addr.sin_port));
        for (int j = 0; j < 901; j++)
        {
            std::string path = " ";
            path = filePath + std::to_string(j) + styefile;
            cv::Mat image = GetImage(connectfd);//nhan anh tu client
            cv::Mat brigt = image + cv::Scalar(51, 51, 51); //tang do sang
            cv::imwrite(path, brigt);//ghi frames da xu ly vao folder Frames
            SendImage(brigt, sockfd);//gui anh da xu ly den client
            //close(connectfd);
        }
        // for (int j = 0; j < 901; j++)
        // {
        //     std::string path = " ";
        //     path = filePath + std::to_string(j) + styefile;
        //     cv::Mat image = cv::imread(path, 1);
        //     SendImage(image, sockfd);
        //     imshow("read image", image);
        //     cv::waitKey(0);
        //     close(connectfd);
        // }
        
    }
    
    return 0;
}
void InitAddr(int sockfd)
{
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1)
    {
        perror("bind error");
        exit(1);
    }
    ret = listen(sockfd, 10);
    if (ret == -1)
    {
        perror("listen error");
        exit(1);
    }
}

cv::Mat GetImage(int connectfd)
{
    cv::Mat mat = cv::Mat::zeros(Heigth, Width, CV_8UC3);
    const int imgSize = mat.total() * mat.elemSize();
    uchar bufferData[imgSize * 2];
    int bytes = 0;
    for (int i = 0; i < imgSize; i += bytes)
    {
        bytes = recv(connectfd, bufferData + i, imgSize - i, 0);
        if (bytes == -1)
        {
            std::cout << "status == -1 error ==" << errno << "in socket::recv\n";
            exit(EXIT_FAILURE);
        }
    }
    mat = cv::Mat(Heigth, Width, CV_8UC3, bufferData);
    return mat;
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