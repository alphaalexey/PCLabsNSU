#include <chrono>
#include <format>
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace cv;

void detect(Mat &img, CascadeClassifier &cascade, double scale) {
    std::vector<Rect> faces, faces2;
    Mat gray, smallImg;

    cvtColor(img, gray, COLOR_BGR2GRAY); // Convert to Gray Scale
    double fx = 1 / scale;

    // Resize the Grayscale Image
    resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR);
    equalizeHist(smallImg, smallImg);

    // Detect faces of different sizes using cascade classifier
    cascade.detectMultiScale(smallImg, faces, 1.1, 2, CASCADE_SCALE_IMAGE);

    // Draw circles around the faces
    for (const auto &r : faces) {
        Point center;
        Scalar color = Scalar(255, 0, 0); // Color for Drawing tool
        int radius;

        Rect real_r =
            Rect(r.x * scale, r.y * scale, r.width * scale, r.height * scale);

        Mat imgROI = img(real_r);
        blur(imgROI, imgROI, Size(30, 30));
        imgROI.copyTo(img(real_r));

        double aspect_ratio = (double)r.width / r.height;
        if (0.75 < aspect_ratio && aspect_ratio < 1.3) {
            center.x = cvRound((r.x + r.width * 0.5) * scale);
            center.y = cvRound((r.y + r.height * 0.5) * scale);
            radius = cvRound((r.width + r.height) * 0.25 * scale);
            circle(img, center, radius, color, 3);
        } else {
            rectangle(img, Point(cvRound(r.x * scale), cvRound(r.y * scale)),
                      Point(cvRound((r.x + r.width - 1) * scale),
                            cvRound((r.y + r.height - 1) * scale)),
                      color, 3);
        }
    }
}

int main(int argc, char *argv[]) {
    // VideoCapture class for playing video for which faces to be detected
    VideoCapture capture;
    Mat frame, image;

    // PreDefined trained XML classifiers with facial features
    CascadeClassifier cascade;
    double scale = 1, mainScale = 2;

    // Change path before execution
    cascade.load(
        "/usr/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml");

    if (!capture.open(0)) {
        std::cerr << "Could not Open Camera" << std::endl;
        return EXIT_FAILURE;
    }

    const size_t count_max = 10;
    size_t count = 0;
    auto start = std::chrono::high_resolution_clock::now();
    auto curr = start;
    double fps = 0;

    // Capture frames from video and detect faces
    std::cout << "Face Detection Started...." << std::endl;
    while (true) {
        capture >> frame;
        if (frame.empty()) {
            break;
        }

        detect(frame, cascade, scale);

        count++;
        if (count >= count_max) {
            curr = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<long double> diff = curr - start;
            fps = count / diff.count();

            std::cout << diff << " \t" << fps << " fps" << std::endl;

            count = 0;
            start = curr;
        }

        putText(frame, std::format("{} fps", cvRound(fps)), Point(10, 30),
                FONT_HERSHEY_SCRIPT_COMPLEX, 1.0, Scalar(255, 255, 0), 2, 8);

        resize(frame, image, Size(), mainScale, mainScale, INTER_LINEAR);

        // Show Processed Image with detected faces
        imshow("Face Detection", image);

        char c = (char)waitKey(10);
        if (c == 27 || c == 'q' || c == 'Q') {
            break;
        }
    }

    destroyAllWindows();

    return EXIT_SUCCESS;
}
