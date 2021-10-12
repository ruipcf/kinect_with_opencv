#include "header.h"

int __cdecl main(void) {

    // directory management
    fs::create_directories("Dataset");

    string name = "User";
    int no = 3;

    if (no > 0) {
        CreateDirectory(("Dataset/" + name).c_str(), NULL);
        CreateDirectory(("Dataset/" + name + "/kinect").c_str(), NULL);
        int i = 1;
        while (i <= no) {
            CreateDirectory(("Dataset/" + name + "/kinect/capture" + to_string(i)).c_str(), NULL);
            i++;
        }
    }
    else
        return 1;

    App app(name);

    int i = 1;
    while (i<=no) {
        app.capture = i;
        // function to receive data from Kinect
        while(app.depthKinectImg.empty()) {
          getKinectImage(app);
        }
        i++;
    }
    return 0;
}
