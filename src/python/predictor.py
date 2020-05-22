# Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved
import atexit
import bisect
import multiprocessing as mp
from collections import deque
import cv2
import torch
import numpy as np
import os

from detectron2.config import get_cfg
from detectron2.data import MetadataCatalog
from detectron2.engine.defaults import DefaultPredictor
from detectron2.utils.video_visualizer import VideoVisualizer
from detectron2.utils.visualizer import ColorMode, Visualizer
'''
python demo/demo.py --config-file configs/COCO-InstanceSegmentation/mask_rcnn_R_50_FPN_3x.yaml \
  --input /media/gm/Data/SLAM/dataset/TUM/rgbd_dataset_freiburg3_sitting_halfsphere/rgb/1341845948.747856.png \
  --output ./result \
  --opts MODEL.WEIGHTS /home/gm/mask_rcnn/model/facebookresearch_detectron2/model_final_f10217.pkl
'''
class VisualizationDemo:

    def __init__(self, instance_mode=ColorMode.IMAGE, parallel=False):
        """
        Args:
            cfg (CfgNode):
            instance_mode (ColorMode):
            parallel (bool): whether to run the model in different processes from visualization.
                Useful since the visualization logic can be slow.
        """
        print("init start\n")
        
        cfg = get_cfg()
        cfg.merge_from_file("/home/gm/mask_rcnn/detectron2-master/configs/COCO-InstanceSegmentation/mask_rcnn_R_50_FPN_3x.yaml")
        cfg.merge_from_list(['MODEL.WEIGHTS', '/home/gm/mask_rcnn/model/facebookresearch_detectron2/model_final_f10217.pkl'])
        # Set score_threshold for builtin models
        cfg.MODEL.RETINANET.SCORE_THRESH_TEST = 0.5
        cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.5
        cfg.MODEL.PANOPTIC_FPN.COMBINE.INSTANCES_CONFIDENCE_THRESH = 0.5
        cfg.freeze()
        self.metadata = MetadataCatalog.get(
            cfg.DATASETS.TEST[0] if len(cfg.DATASETS.TEST) else "__unused"
        )
        self.cpu_device = torch.device("cpu")
        self.instance_mode = instance_mode

        self.parallel = parallel
        self.predictor = DefaultPredictor(cfg)
        print("init finished\n")


    def test22(self,name):
        print("hello mask"+name)

    def run_on_image(self, image_path,path=None):
        """
        Args:
            image (np.ndarray): an image of shape (H, W, C) (in BGR order).
                This is the format used by OpenCV.

        Returns:
            predictions (dict): the output of the model.
            vis_output (VisImage): the visualized image output.
        """
        print("predictor start")
        dynamic_class=['person','bicycle','car','motorcycle','airplane','bus','train','truck','boat','bird','cat','dog','horse','sheep','cow','elephantbear','zebra','giraffe']
        vis_output = None
        print(image_path)
        image=cv2.imread(image_path)
        predictions = self.predictor(image)
        print("predictor finish")

        # print(type(predictions))
        # print(predictions)
        # print(predictions["instances"].pred_classes)
        predictions=predictions["instances"]
        boxes = predictions.pred_boxes if predictions.has("pred_boxes") else None
        classes = predictions.pred_classes if predictions.has("pred_classes") else None
        scores = predictions.scores if predictions.has("scores") else None
        labels = [self.metadata.get("thing_classes", None)[i] for i in classes]
        print(labels)
        pred_masks_np=(predictions.pred_masks).cpu().numpy()
        result=np.zeros(np.shape(image)[:-1])

        for i in range(len(labels)):
            label=labels[i]
            if label in dynamic_class:
                result[pred_masks_np[i] > 0]=1

        print("sum ",np.sum(result))
        cv2.imwrite(path,result)
        print("path ",path)

        return result