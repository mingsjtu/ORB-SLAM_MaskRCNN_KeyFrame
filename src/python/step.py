import predictor
from predictor import *
import os
import time
demo = VisualizationDemo()
dir_name="/media/gm/Data/SLAM/self_video/5.16morn/xiaomi_20200516_080955/rgb"
# dir_name="/media/gm/Data/SLAM/dataset/TUM/rgbd_dataset_freiburg3_walking_halfsphere/rgb"
file_name=os.listdir(dir_name)
process_start=time.time()
file_num=0
save_dir="/home/gm/SLAM/AIslam/dyna_slam/DynaSLAM-master/DynaSLAM-master/workspace/self_video/5.16morn/xiaomi_20200516_080955/rgb"
# save_dir="/home/gm/SLAM/AIslam/dyna_slam/DynaSLAM-master/DynaSLAM-master/workspace/TUM/rgbd_dataset_freiburg3_walking_halfsphere"
for name in file_name:
    save_path=os.path.join(save_dir,name)
    if not os.path.exists(save_path):
        result=demo.run_on_image(os.path.join(dir_name,name),save_path)
        file_num+=1
print("ave time %.4f\n",(time.time()-process_start)/file_num)
# import cv2
# img=cv2.imread("/home/gm/SLAM/AIslam/dyna_slam/DynaSLAM-master/DynaSLAM-master/workspace/TUM/rgbd_dataset_freiburg3_sitting_halfsphere/1341845948.747856.png")
# img[img==1]=255
# cv2.imshow("pre",img)
# cv2.waitKey(0)

