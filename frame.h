#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <cstdint>

enum FrameType{
    I_frame,P_frame
};

class Frame{
    public:
        int width;
        int height;
        std::vector<uint8_t>data;
        FrameType type;
        Frame(int w, int h, FrameType type):width(w), height(h), type(type){
            data.resize(width*height, 0);
        }
        uint8_t getPixel(int x, int y) const{
            if(x>=0 && x<width && y>=0 && y<height){
                return data[y*width+x];
            }
            else{
                return 0;
            }
        }
        void setPixel(int x, int y, int value){
            if(x>=0 && x<width && y>=0 && y<height){
                data[y*width+x] = value;
            }
        }

};

#endif