# Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved
import atexit
import bisect
import multiprocessing as mp
from collections import deque
import cv2
import torch
import numpy as np

from detectron2.data import MetadataCatalog
from detectron2.engine.defaults import DefaultPredictor
from detectron2.utils.video_visualizer import VideoVisualizer
from detectron2.utils.visualizer import ColorMode, Visualizer


class VisualizationDemo(object):

    def __init__(self, cfg, instance_mode=ColorMode.IMAGE, parallel=False):
        """
        Args:
            cfg (CfgNode):
            instance_mode (ColorMode):
            parallel (bool): whether to run the model in different processes from visualization.
                Useful since the visualization logic can be slow.
        """
        self.metadata = MetadataCatalog.get(
            cfg.DATASETS.TEST[0] if len(cfg.DATASETS.TEST) else "__unused"
        )
        self.cpu_device = torch.device("cpu")
        self.instance_mode = instance_mode

        self.parallel = parallel
        self.predictor = DefaultPredictor(cfg)




    def run_on_image(self, image):
        """
        Args:
            image (np.ndarray): an image of shape (H, W, C) (in BGR order).
                This is the format used by OpenCV.

        Returns:
            predictions (dict): the output of the model.
            vis_output (VisImage): the visualized image output.
        """
        dynamic_class=['person','bicycle','car','motorcycle','airplane','bus','train','truck','boat','bird','cat','dog','horse','sheep','cow','elephantbear','zebra','giraffe']
        vis_output = None
        predictions = self.predictor(image)
        print(type(predictions))
        print(predictions)
        print(predictions["instances"].pred_classes)
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
        print(np.shape(result))
        vis_output = np.int8(result)*255
        print("sum ",np.sum(vis_output))

        
        return predictions, vis_output