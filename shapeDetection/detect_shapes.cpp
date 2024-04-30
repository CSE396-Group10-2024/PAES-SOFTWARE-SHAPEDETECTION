#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    // Load and preprocess an Image
    Mat image = imread("image.png");
    if (image.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    Mat gray, blur;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, Size(5, 5), 0);

    // Perform edge detection
    Mat edges;
    Canny(blur, edges, 30, 150);

    // Find contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(edges.clone(), contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Classifying shapes and printing the result
    bool found = false;
    for (size_t i = 0; i < contours.size(); i++) {
        double epsilon = 0.04 * arcLength(contours[i], true);
        vector<Point> approx;
        approxPolyDP(contours[i], approx, epsilon, true);
        size_t vertices = approx.size();

        string shape;
        if (vertices == 2)
            shape = "Line";
        else if (vertices == 3)
            shape = "Triangle";
        else if (vertices == 4) {
            Rect rect = boundingRect(approx);
            float aspectRatio = float(rect.width) / rect.height;
            shape = (aspectRatio >= 0.95 && aspectRatio <= 1.05) ? "Square" : "Rectangle";
        }
        else if (vertices == 5)
            shape = "Pentagon";
        else {
            // Check if contour resembles a circle
            double area = contourArea(contours[i]);
            double perimeter = arcLength(contours[i], true);
            double circularity = 4 * CV_PI * area / (perimeter * perimeter);
            if (circularity > 0.85)
                shape = "Circle";
            else
                shape = "Unknown";
        }

        // Print the detected shape
        cout << "Detected Shape: " << shape << endl;
        found = true;
        break; // Stop after processing the first contour
    }

    if (!found) {
        cout << "No shape detected" << endl;
    }

    return 0;
}

