# test_all_augmentations.py

import cv2
import numpy as np
import os
import torch
from ais_bench.infer.interface import InferSession
from det_utils import letterbox, scale_coords, nms

def preprocess_image(image, cfg, bgr2rgb=True):
    """图片预处理"""
    img, scale_ratio, pad_size = letterbox(image, new_shape=cfg['input_shape'])
    if bgr2rgb:
        img = img[:, :, ::-1]
    img = img.transpose(2, 0, 1)
    img = np.ascontiguousarray(img, dtype=np.float32)
    return img, scale_ratio, pad_size

def draw_bbox(bbox, img0, color, wt, names):
    """在图片上画预测框"""
    for idx, class_id in enumerate(bbox[:, 5]):
        if float(bbox[idx][4] < float(0.05)):
            continue
        img0 = cv2.rectangle(img0, (int(bbox[idx][0]), int(bbox[idx][1])),
                             (int(bbox[idx][2]), int(bbox[idx][3])), color, wt)
        img0 = cv2.putText(img0, names[int(class_id)],
                             (int(bbox[idx][0]), int(bbox[idx][1] + 16)),
                             cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1)
        img0 = cv2.putText(img0, '{:.4f}'.format(bbox[idx][4]),
                             (int(bbox[idx][0]), int(bbox[idx][1] + 32)),
                             cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1)
    return img0

def get_labels_from_txt(path):
    """从txt文件获取图片标签"""
    labels_dict = dict()
    with open(path) as f:
        for cat_id, label in enumerate(f.readlines()):
            labels_dict[cat_id] = label.strip()
    return labels_dict

def infer_image_and_save(img_path, model, class_names, cfg, output_path=None):
    """图片推理并保存结果"""
    image = cv2.imread(img_path)
    if image is None:
        print(f"  [警告] 无法读取图像: {img_path}")
        return None, []

    img, scale_ratio, pad_size = preprocess_image(image, cfg)

    try:
        output = model.infer([img])[0]
    except Exception as e:
        print(f"  [错误] 推理失败: {e}")
        return None, []
        
    output = torch.tensor(output)
    boxout = nms(output, conf_thres=cfg["conf_thres"], iou_thres=cfg["iou_thres"])
    pred_all = boxout[0].numpy()

    scale_coords(cfg['input_shape'], pred_all[:, :4], image.shape, ratio_pad=(scale_ratio, pad_size))
    img_with_boxes = draw_bbox(pred_all, image.copy(), (0, 255, 0), 2, class_names)

    if output_path:
        cv2.imwrite(output_path, img_with_boxes)

    return img_with_boxes, pred_all

def test_all_augmentations():
    """
    测试所有YOLOv5的等价蜕变测试数据 (需求 1.1, 1.2, 1.3)
    """
    cfg = {
        'conf_thres': 0.4,
        'iou_thres': 0.5,
        'input_shape': [640, 640],
    }

    model_path = 'yolo.om'
    label_path = './coco_names.txt'
    
    # 定义所有要测试的目录
    test_suites = [
        {
            "name": "旋转测试 (1.1)",
            "input_dir": "./augmented_1",
            "output_dir": "./augmented_output_1"
        },
        {
            "name": "缩放测试 (1.2)",
            "input_dir": "./augmented_2",
            "output_dir": "./augmented_output_2"
        },
        {
            "name": "亮度测试 (1.3)",
            "input_dir": "./augmented_3",
            "output_dir": "./augmented_output_3"
        }
    ]

    # 检查模型和标签文件是否存在
    if not os.path.exists(model_path):
        print(f"错误: 模型文件不存在 {model_path}")
        return
    if not os.path.exists(label_path):
        print(f"错误: 标签文件不存在 {label_path}")
        return

    # 初始化模型
    print(f"正在加载模型: {model_path} ...")
    model = InferSession(0, model_path)
    labels_dict = get_labels_from_txt(label_path)
    print("模型加载完毕。")

    # 遍历所有测试套件
    for suite in test_suites:
        name = suite["name"]
        input_dir = suite["input_dir"]
        output_dir = suite["output_dir"]

        print(f"\n" + "="*50)
        print(f"开始执行: {name}")
        print(f"输入目录: {input_dir}")
        print(f"输出目录: {output_dir}")
        print("="*50)

        if not os.path.exists(input_dir):
            print(f"[警告] 输入目录不存在，跳过: {input_dir}")
            continue

        os.makedirs(output_dir, exist_ok=True)

        all_files = os.listdir(input_dir)
        image_files = [f for f in all_files if f.endswith(('.jpg', '.jpeg', '.png')) and not f.startswith('._')]

        if not image_files:
            print(f"[警告] 在 {input_dir} 中没有找到图像文件。")
            continue

        print(f"开始测试所有 {len(image_files)} 张图像...")

        for i, img_file in enumerate(image_files):
            img_path = os.path.join(input_dir, img_file)
            output_path = os.path.join(output_dir, f"result_{img_file}")

            print(f"  测试 {i+1}/{len(image_files)}: {img_file}")

            result_img, detections = infer_image_and_save(img_path, model, labels_dict, cfg, output_path)

            if result_img is not None:
                print(f"    检测到 {len(detections)} 个目标")
            else:
                print(f"    处理失败")

        print(f"\n{name} 完成! 所有结果保存在: {output_dir}")

    print("\n所有YOLOv5测试执行完毕。")

if __name__ == "__main__":
    test_all_augmentations()
