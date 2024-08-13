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

model = models.__dict__['resnet_binary']
model_config = {'input_size': None, 'dataset': 'cifar10'}

model = model(**model_config)
checkpoint = torch.load('./results/resnet18_binary/model_best.pth.tar', map_location=torch.device('cpu'))
model.load_state_dict(checkpoint['state_dict'])
# print(checkpoint['state_dict']['bn1.weight'].dtype)
# torch.save(model, './model.pth')
batch_size = 1
input_shape = (3, 32, 32)

input = torch.randn(batch_size, *input_shape)
torch.onnx.export(model, 
                    input, 
                    'model.onnx', 
                    # opset_version=10,
                    # do_constant_folding=True, 
                    input_names=['input'],
                    output_names=['output'],
                    keep_initializers_as_inputs=True,
                    verbose=True
                )
