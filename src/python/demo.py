# Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved
import argparse
import glob
import multiprocessing as mp
import os
import time
import cv2
import tqdm
import time

from detectron2.config import get_cfg
from detectron2.data.detection_utils import read_image
from detectron2.utils.logger import setup_logger

from predictor import VisualizationDemo

# constants
WINDOW_NAME = "COCO detections"



if __name__ == "__main__":
    
    init_start=time.time()
    demo = VisualizationDemo()
    init_end=time.time()
    print("init time %.4f"%(init_end-init_start))
    dir_name="/media/gm/Data/SLAM/dataset/TUM/rgbd_dataset_freiburg3_sitting_halfsphere/rgb"
    file_name=os.listdir(dir_name)
    process_start=time.time()
    file_num=0
    for name in file_name:
        result=demo.run_on_image(os.path.join(dir_name,name),"result.png")
        file_num+=1
        if file_num>=10:
            break
    process_end=time.time()
    average_time=(process_end-process_start)/file_num
    print("average time %.4f"%average_time)
