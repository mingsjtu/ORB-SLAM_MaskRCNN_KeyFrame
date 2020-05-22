

import os
import sys

import numpy as np
import cv2 as cv;
import re
from math import ceil

# 逆时针旋转90度
def RotateAntiClockWise90(img):
    trans_img = cv.transpose( img )
    new_img = cv.flip( trans_img, 0 )
    return new_img

def video_to_image(video_path,image_dir,time_start):
    print('\n-----------------------------------------------------------------------------')
    print('正在处理视频：{}\n'.format(video_path))
    
    video_dir = os.path.dirname(video_path)
    video_name = os.path.basename(video_path)
    
    if not os.path.exists(image_dir):
        os.makedirs(image_dir)
    else:
        print("cmd "+"rm -rf {}/*".format(image_dir))
        os.system("rm -rf {}/*".format(image_dir))


    cap = cv.VideoCapture(video_path)
    # Define the codec and create VideoWriter object
    fps = cap.get(5)   #视频帧率
    width = cap.get(3)
    height = cap.get(4)
    
    #print("视频  {}  的帧率为:{:.2f}  分辨率为:{} ✖ {}".format(video_path, fps, width, height))
    print("视频  {} 的帧率为：{}  分辨率为：{} * {}".format(video_path, fps, width, height))
	
    n=0
    new_img_num = 0
    old_img_num = 0
    is_first=1
    while(cap.isOpened()):
        ret, frame = cap.read()
        if ret==False:
            break
        if is_first:
            is_first=0
            print(frame.shape)

        # frame=RotateAntiClockWise90(frame)
        # frame=RotateAntiClockWise90(frame)
        # frame=RotateAntiClockWise90(frame)
        
        frame=cv.resize(frame, (720, 405))
        if ret==True:
            #cv.imshow('frame1', frame)
            n=n+1
            
            # if n%sample_rate == 1:
            image_name = "{:.4f}{}".format(n/fps+time_start,'.jpg')#video_name+'_'+str(n)+'.jpg'
            image_path = os.path.join(image_dir, image_name)

            # print(image_path)
            cv.imwrite(image_path,frame)
            new_img_num += 1
            
            if cv.waitKey(1) & 0xFF == ord('q'):
                break
        else:
            break
        
    # Release everything if job is finished

    cap.release()
    print('视频处理完毕，共{}帧。\n'.format(n))
def generate_rgbtxt(dirpath,txt_name):
    file_list=sorted(os.listdir(dirpath))
    ext=('jpg')
    res_list=[]
    for f in file_list:
        if f.endswith(ext):
            tmp_path=f[0:-4]
            res_list.append((tmp_path,"rgb/"+f))

    with open(txt_name,'w') as txt_f:
        for res in res_list:
            txt_f.write(str(res[0])+"    "+res[1]+"\n")

def rename(dirpath,add_float):
    files=os.listdir(dirpath)
    for f in files:
        old_file=os.path.join(dirpath,f)
        f=os.path.splitext(f)[0]
        #指定新文件的位置，如果没有使用这个方法，则新文件名生成在本项目的目录中
        new_file=os.path.join(dirpath,"%.5f"%(float(f)+add_float)+'.jpg')
        os.rename(old_file,new_file)


video_name="/media/gm/Data/SLAM/self_video/5.16morn/huawei_20200516_081324"
video_to_image(video_name+".mp4",video_name+"/rgb",8*3600+11*60+50)
generate_rgbtxt(video_name+"/rgb",video_name+"/rgb.txt")

