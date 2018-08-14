# coding: utf-8

# # Imports

import numpy as np
import sys
import tensorflow as tf
import cv2
from timeit import default_timer as timer


# imports for object_detection
sys.path.append('D:/Pobrane/models-master/research/')
sys.path.append('D:/Pobrane/models-master/research/slim/')

if tf.__version__ < '1.4.0':
  raise ImportError('Please upgrade your tensorflow installation to v1.4.* or later!')


# ## Object detection imports
# Here are the imports from the object detection module.

from object_detection.utils import label_map_util

# ## Variables
# What model to download.
MODEL_NAME = 'D:/Pobrane/models-master/research/object_detection/mkx_inference_graph'

# Path to frozen detection graph. This is the actual model that is used for the object detection.
PATH_TO_CKPT = MODEL_NAME + '/frozen_inference_graph.pb'

# List of the strings that is used to add correct label for each box.
PATH_TO_LABELS = 'D:/Pobrane/models-master/research/object_detection/data/mkx_label_map.pbtxt'

NUM_CLASSES = 2

# ## Loading label map

label_map = label_map_util.load_labelmap(PATH_TO_LABELS)
categories = label_map_util.convert_label_map_to_categories(label_map, max_num_classes=NUM_CLASSES, use_display_name=True)
category_index = label_map_util.create_category_index(categories)

def DetectObjects(image_np, sess, detection_graph):
    # Expand dimensions since the model expects images to have shape: [1, None, None, 3]
    image_np_expanded = np.expand_dims(image_np, axis=0)
    image_tensor = detection_graph.get_tensor_by_name('image_tensor:0')

    # Each box represents a part of the image where a particular object was detected.
    boxes = detection_graph.get_tensor_by_name('detection_boxes:0')

    # Each score represent how level of confidence for each of the objects.
    # Score is shown on the result image, together with the class label.
    scores = detection_graph.get_tensor_by_name('detection_scores:0')
    classes = detection_graph.get_tensor_by_name('detection_classes:0')
    num_detections = detection_graph.get_tensor_by_name('num_detections:0')

    # Actual detection.
    (boxes, scores, classes, num_detections) = sess.run(
        [boxes, scores, classes, num_detections],
        feed_dict={image_tensor: image_np_expanded})

    # 0 - no error
    # 1 - one of the objects is not an array
    # 2 - invalid array dimensions
    # 4 - no detections
    errorCode = 0.0
    if boxes[0] is None or scores[0] is None or classes[0] is None or num_detections[0] is None:
        errorCode = 1.0
    elif len(boxes[0].shape) != 2 or len(scores[0].shape) != 1 or len(classes[0].shape) != 1:
        errorCode = 2.0
    elif num_detections[0] == 0:
        errorCode = 4.0

    return (boxes[0], scores[0], classes[0], num_detections[0], errorCode)

# Creating session
gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.2)
detection_graph = tf.Graph()
sess = None
with detection_graph.as_default():
    od_graph_def = tf.GraphDef()
    with tf.gfile.GFile(PATH_TO_CKPT, 'rb') as fid:
        serialized_graph = fid.read()
        od_graph_def.ParseFromString(serialized_graph)
        tf.import_graph_def(od_graph_def, name='')

    sess = tf.Session(graph=detection_graph, config=tf.ConfigProto(gpu_options=gpu_options))


def Eval(image):
    (im_height, im_width, _) = image.shape
    image_np = image.reshape((im_height, im_width, 3)).astype(np.uint8)
    output_dict = DetectObjects(image_np, sess, detection_graph)
    return output_dict

def Time():
    image = cv2.imread("D:/Pobrane/models-master/research/object_detection/test_images/1.jpg", cv2.IMREAD_COLOR )
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    evals = 1000
    start = timer()
    for x in range(evals):
        if( x % 100 == 0 ):
            print( x/10, "%" )
        DetectObjects(image, sess, detection_graph)
    print("Time per evaluation: ", (timer() - start) / evals)

# Close the session when finished
def Exit():
    if( not sess._closed ):
        sess.close()

#image = cv2.imread("D:/Pobrane/models-master/research/object_detection/test_images/1.jpg", cv2.IMREAD_COLOR )
#image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
#ret = Eval(image)
#print( "A1 -2d: ", ret[0][0].dtype, "A2 - 1d: ", ret[1][0].dtype, "A3 - 1d: ", ret[2][0].dtype, "A4 - float32: ", ret[3][0] )
