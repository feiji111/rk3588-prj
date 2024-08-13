import argparse
import os
import time
import logging
import torch
import torch.nn as nn
import torch.nn.parallel
import torch.backends.cudnn as cudnn
import torch.optim
import torch.utils.data
import models
from torch.autograd import Variable
from data import get_dataset
from preprocess import get_transform
from utils import *
from datetime import datetime
from ast import literal_eval
from torchvision.utils import save_image
import torchvision.transforms as transforms
from PIL import Image
import time

import cv2

parser = argparse.ArgumentParser(description='ConvNet Inferring')

parser.add_argument('--input', metavar='INPUT_SOURCE', default=0, help='camera or video path')
parser.add_argument('--input_size', type=int, default=None,
                    help='image input size')
parser.add_argument('--dataset', metavar='DATASET', default='imagenet',
                    help='dataset name or folder')
parser.add_argument('--model_config', default='',
                    help='additional architecture configuration')

def inference():
    args = parser.parse_args()
    model = models.__dict__['resnet_binary']
    model_config = {'input_size': args.input_size, 'dataset': args.dataset}

    model = model(**model_config)
    logging.info("created model with configuration: %s", model_config)

    checkpoint = torch.load('./results/resnet18_binary/model_best.pth.tar', map_location=torch.device('cpu'))

    model.load_state_dict(checkpoint['state_dict'], strict=False)

    model.eval()
    
    size = (32, 32)
    mean = [0.485, 0.456, 0.406]  # 均值
    std = [0.229, 0.224, 0.225]  # 标准差

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    
    capture = cv2.VideoCapture(int(args.input))
    if not capture.isOpened():
        print("Unable to open the camera")
    
    capture.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    capture.set(cv2.CAP_PROP_FRAME_HEIGHT, 360)
    capture.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))

    while(True):
        ret, frame = capture.read()
        if not ret:
            print("reading from camera stop")
            break

        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        frame_rgb = Image.fromarray(frame_rgb)
        # print(frame_rgb.size)        
        # frame_rgb = frame_rgb.transpose(2, 0, 1)

        
        preprocess = transforms.Compose([
            transforms.Resize(size),
            transforms.ToTensor(),
            transforms.Normalize(mean, std)
        ])
        input_tensor = preprocess(frame_rgb).unsqueeze(0)  # 添加 batch 维度
        # print(input_tensor.shape)
        input_tensor = input_tensor.to(device)

        with torch.no_grad():
            start_time = time.time()
            output = model(input_tensor)
            end_time = time.time()
            output = output.cpu()
            # print(output)
        
        execution_time = end_time - start_time

        max_value_index = torch.argmax(output)
        if max_value_index != 1:
            print('not car')
        else:
            print('car detected')

        cv2.imshow('Camera', frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break        

    print('detection stop')


if __name__ == '__main__':
    inference()
