#include "frame.h"
#include "MotionVector.h"
#include <limits>



MotionVector estimateMotion(const Frame &current_frame, const Frame &reference_frame, int mbx, int mby){
    int current_block_x = mbx * BLOCK_SIZE;
    int current_block_y = mby * BLOCK_SIZE;

    MotionVector bestMV{0, 0};

    double best_correlation = std::numeric_limits<double>::min();

    double mean_curr = 0.0;
    for(int j = 0; j < BLOCK_SIZE; j++){
        for(int i = 0; i < BLOCK_SIZE; i++){
            mean_curr += current_frame.getPixel(current_block_x + i, current_block_y + j);
        }
    }
    mean_curr /= BLOCK_SIZE * BLOCK_SIZE;

    for(int dy = -SEARCH_RANGE; dy <= SEARCH_RANGE; dy++){
        for(int dx = -SEARCH_RANGE; dx <= SEARCH_RANGE; dx++){
            int ref_block_x = current_block_x + dx;
            int ref_block_y = current_block_y + dy;

            if(ref_block_x < 0 || ref_block_y < 0 || ref_block_x + BLOCK_SIZE > reference_frame.width || ref_block_y + BLOCK_SIZE > reference_frame.height){
                continue;
            }

            double mean_ref = 0.0;
            for(int j = 0; j < BLOCK_SIZE; j++){
                for(int i = 0; i < BLOCK_SIZE; i++){
                    mean_ref += reference_frame.getPixel(ref_block_x + i, ref_block_y + j);
                }
            }

            mean_ref /= BLOCK_SIZE * BLOCK_SIZE;

            double numerator = 0.0;
            double denom_curr = 0.0;
            double denom_ref = 0.0;
            
            for(int j = 0; j < BLOCK_SIZE; j++){
                for(int i = 0; i < BLOCK_SIZE; i++){
                    double diff_curr = current_frame.getPixel(current_block_x + i, current_block_y + j) - mean_curr;
                    double diff_ref = reference_frame.getPixel(ref_block_x + i, ref_block_y + j) - mean_ref;

                    numerator += diff_curr * diff_ref;
                    denom_curr += diff_curr * diff_curr;
                    denom_ref += diff_ref * diff_ref;
                }
            }

            if(denom_curr > 0 && denom_ref > 0){
                double correlation = numerator / (std::sqrt(denom_curr) * std::sqrt(denom_ref));

                if(correlation > best_correlation){
                    best_correlation = correlation;
                    bestMV.dx = dx;
                    bestMV.dy = dy;
                }
            }
        }
    }
    return bestMV;
}