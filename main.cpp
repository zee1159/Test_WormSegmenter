#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <argp.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Test.h"

using namespace std;
using namespace cv;


struct Arguments {
    string project;
    string input;
    string output;
    int padding;
    int frames;
    string extension;
    int width;
    int height;
    int area_min;
    int area_max;
    int search_win_size;
    int blur_radius;
    int threshold_win_size;
    float threshold_ratio;
    string log;
    bool verbose;

    Arguments():    input("data/"),
                    output("output.txt"),
                    padding(7),
                    frames(1000),
                    extension(".jpg"),
                    width(640),
                    height(480),
                    area_min(200),
                    area_max(400),
                    search_win_size(100),
                    blur_radius(3),
                    threshold_win_size(25),
                    threshold_ratio(0.9),
                    log("wormSeg.log"),
                    verbose(true) {}
} cla;


int findCentroidFromImage(Mat, int*, int*, int*);

static int parse_opt (int key, char *arg, struct argp_state *state) {
    switch(key) {
        case 'P':
            cla.project = arg;
            break;

        case ARGP_KEY_END:
            if (state->arg_num < 1){
                cout << "Too few arguments!" << endl;
                argp_usage(state);
            }
            break;

        case 'i':
            cla.input = arg;
            break;

        case 'o':
            cla.output = arg;
            break;

        case 'p':
            cla.padding = atoi(arg);
            break;

        case 'f':
            cla.frames = atoi(arg);
            break;

        case 'e':
            cla.extension = arg;
            break;

        case 'w':
            cla.width = atoi(arg);
            break;

        case 'h':
            cla.height = atoi(arg);
            break;

        case 'a':
            cla.area_min = atoi(arg);
            break;

        case 'A':
            cla.area_max = atoi(arg);
            break;

        case 's':
            cla.search_win_size = atoi(arg);
            break;

        case 'b':
            cla.blur_radius = atoi(arg);
            break;

        case 't':
            cla.threshold_win_size = atoi(arg);
            break;

        case 'T':
            cla.threshold_ratio = atof(arg);
            break;

        case 'l':
            cla.log = arg;
            break;

        case 'v':
            cla.verbose = true;             //Needs correction.
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}


template <typename T> string NumberToString(T pNumber) {
    ostringstream oOStrStream;
    oOStrStream << pNumber;

    return oOStrStream.str();
}


string intToFileName(string fileNameFormat, int fileNumber) {
    string temp = NumberToString(fileNumber);

    return fileNameFormat.replace(fileNameFormat.size() - temp.size(), temp.size(), temp);
}


string wormSegmenter(const char *fname) {

    int x = -1, y = -1, area = -1;
    int adjustX = 0, adjustY = 0;


    // File name of each file including the path.
    string fileName = cla.input + fname;

    // Read and convert the image into gray scale and copy into the matrix.
    Mat src = imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);

    // Exit.
    if (!src.data) {
        cout << endl << "Exited." << endl;
        exit(1);
    }

    if((x == -1) && (y == -1)) {
        findCentroidFromImage(src, &x, &y, &area);
        src = imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
        adjustX = x - (cla.search_win_size / 2);
        adjustY = y - (cla.search_win_size / 2);
    }
    else{
        src = src(Rect(x - (cla.search_win_size / 2), y - (cla.search_win_size / 2), cla.search_win_size, cla.search_win_size));

        findCentroidFromImage(src, &x, &y, &area);

        if((x > 0) && (y > 0)) {
            x += adjustX;
            y += adjustY;
            adjustX = x - (cla.search_win_size / 2);
            adjustY = y - (cla.search_win_size / 2);
        }
    }

    //printf("%s\t%s\t%d\t%d\t%d\n", cla.input.c_str(), fname, x, y, area);
        
    //std::stringstream out;
    //out << fname << " " << x << " " << y << " " << area;
    string temp = fname;
    std::string output = temp + " " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(area);
        
    return output;
}

/* Function to get the centroid in the image */
int findCentroidFromImage(Mat src, int *pX, int *pY, int *pArea) {
    // Smoothing the image.
    blur(src, src, Size(cla.blur_radius, cla.blur_radius));     //Blur radius 3 in original java worm segmenter.

    // Convert the image into binary image.
    threshold(src, src, int(cla.threshold_ratio * 255), 255, THRESH_BINARY_INV);

    // Vector for storing contour
    vector<vector <Point> > contours;

    vector<Vec4i> hierarchy;

    // Find contours in the image.
    findContours(src, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    if (contours.size() > 0) {
        int largest_contour_index = 0;
        int largest_area = 0;

        // Iterate through each contour.
        for(int i = 0; i < contours.size(); i ++) {
            //  Find the area of contour
            double a = contourArea(contours[i], false);

            if(a > largest_area) {
                largest_area = a;

                // Store the index of largest contour
                largest_contour_index = i;
            }
        }

        Rect bRect = boundingRect(contours[largest_contour_index]);

        *pX = bRect.x + (bRect.width / 2);
        *pY = bRect.y + (bRect.height / 2);
        *pArea = largest_area;
    }
    else {
        *pX = -1;
        *pY = -1;
        *pArea = -1;
    }

    return 0;
}

/* JNI method for the java wrapper class */
JNIEXPORT jstring JNICALL Java_Test_run(JNIEnv *env, jobject obj, jstring path, jstring fname){
    jboolean iscopy;
    const char *file_path;
    const char *file_name;
    string res;
    
    //copying the arguments passed from the java method call
    file_path = (env)->GetStringUTFChars(path, &iscopy);
    file_name = (env)->GetStringUTFChars(fname, &iscopy);
    
    //providing input directory path
    cla.input = file_path;
    
    try{
        res = wormSegmenter(file_name);
    }
    catch(int e){
        printf("Exception occured: %d\n", e);
    }

    return (env)->NewStringUTF(res.c_str());
}

int main(int argc, char **argv) {
    const char *argp_program_version = "Worm Segmenter JNI 1.0.0";
    const char *argp_program_bug_address = "<zkhavas@hawk.iit.edu>";

    /*
    * Deprecated lines of code, not required when using as JNI library
    struct argp_option options[] = {
        {"project",               'P', "NAME",       0,                    "The name of the project to process."},
        {"input",                 'i', "PATH",       OPTION_ARG_OPTIONAL,  "Path to input images."},
        {"output",                'o', "FILE PATH",  OPTION_ARG_OPTIONAL,  "Output file path."},
        {"padding",               'p', "NUMBER",     OPTION_ARG_OPTIONAL,  "Number of digits in the file name."},
        {"frames",                'f', "FRAMES",     OPTION_ARG_OPTIONAL,  "Number of frames to be processed."},
        {"extension",             'e', "STRING",     OPTION_ARG_OPTIONAL,  "Extension of the image files."},
        {"width",                 'w', "WIDTH",      OPTION_ARG_OPTIONAL,  "The horizontal resolution of the image."},
        {"height",                'h', "HEIGHT",     OPTION_ARG_OPTIONAL,  "The vertical resolution of the image."},
        {"area_min",              'a', "NUMBER",     OPTION_ARG_OPTIONAL,  "The lower bound for a candidate worm component."},
        {"area_max",              'A', "NUMBER",     OPTION_ARG_OPTIONAL,  "The upper bound for a candidate worm component."},
        {"search_win_size",       's', "NUMBER",     OPTION_ARG_OPTIONAL,  "Width and height of crop area."},
        {"blur_radius",           'b', "NUMBER",     OPTION_ARG_OPTIONAL,  "Width and height of the sliding window used in the box blur."},
        {"threshold_win_size",    't', "NUMBER",     OPTION_ARG_OPTIONAL,  "Width and height of the sliding window used in the dynamic threshold."},
        {"threshold_ratio",       'T', "FLOAT",      OPTION_ARG_OPTIONAL,  "Pixel intensity."},
        {"log",                   'l', "FILE PATH",  OPTION_ARG_OPTIONAL,  "Path to log file."},
        {"verbose",               'v', "BOOLEAN",    OPTION_ARG_OPTIONAL,  "Produce verbose output."},
        { 0 }
    };

    struct argp argp = {options, parse_opt, 0, 0};

    argp_parse (&argp, argc, argv, 0, 0, 0);
    */
    //wormSegmenter();

    //return 0;
}
