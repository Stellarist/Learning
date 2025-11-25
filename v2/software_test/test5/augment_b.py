import cv2
import numpy as np
import os
import random

def scale_image(image, scale_factor):
    """
    缩放图像
    :param image: 输入图像
    :param scale_factor: 缩放比例（>1放大，<1缩小）
    :return: 缩放后的图像（保持原始大小，用灰色填充或裁剪）
    """
    height, width = image.shape[:2]
    
    # 缩放后的尺寸
    new_width = int(width * scale_factor)
    new_height = int(height * scale_factor)
    
    # 缩放
    scaled_img = cv2.resize(image, (new_width, new_height), interpolation=cv2.INTER_LINEAR)
    
    # 创建与原图同样大小的灰色背景（与YOLO默认一致）
    result = np.full((height, width, 3), (114, 114, 114), dtype=np.uint8)
    
    # 如果缩放后比原图小，则居中放置
    if scale_factor < 1.0:
        x_offset = (width - new_width) // 2
        y_offset = (height - new_height) // 2
        result[y_offset:y_offset+new_height, x_offset:x_offset+new_width] = scaled_img
    else:
        # 如果缩放后比原图大，则裁剪中心区域
        x_start = (new_width - width) // 2
        y_start = (new_height - height) // 2
        result = scaled_img[y_start:y_start+height, x_start:x_start+width]
    
    return result


def generate_scaling_augmentation(input_images, output_dir, num_scales=10, min_scale=0.6, max_scale=1.4):
    """
    生成缩放扩增数据（简化版，不需要标签）
    """
    os.makedirs(output_dir, exist_ok=True)
    
    if not input_images:
        print("没有提供输入图像路径")
        return
    
    print(f"找到 {len(input_images)} 张图像文件")
    
    total_generated = 0
    
    for image_path in input_images:
        if not os.path.exists(image_path):
            print(f"图像文件不存在: {image_path}")
            continue
            
        image = cv2.imread(image_path)
        if image is None:
            print(f"无法读取图像: {image_path}")
            continue
        
        image_file = os.path.basename(image_path)
        base_name = os.path.splitext(image_file)[0]
        extension = os.path.splitext(image_file)[1]
        
        print(f"\n处理图像: {image_file}")
        for i in range(num_scales):
            # 随机缩放比例
            scale_factor = random.uniform(min_scale, max_scale)
            scaled_image = scale_image(image, scale_factor)
            
            # 新文件名
            new_image_name = f"{base_name}_scale_{i+1:02d}_{scale_factor:.2f}{extension}"
            new_image_path = os.path.join(output_dir, new_image_name)
            
            cv2.imwrite(new_image_path, scaled_image)
            total_generated += 1
            print(f"生成: {new_image_name} (缩放比例: {scale_factor:.2f})")
    
    print(f"\n数据扩增完成! 总共生成 {total_generated} 个缩放样本")
    print(f"输出目录: {output_dir}")


if __name__ == "__main__":
    # 示例：输入图片列表
    input_images = [
        "a.png",
        "b.png",
        "c.png"
    ]
    
    output_dir = "augmented_2"

    generate_scaling_augmentation(
        input_images=input_images,
        output_dir=output_dir,
        num_scales=3,    # 每张图生成3个缩放版本
        min_scale=0.6,   # 最小缩放（0.6倍）
        max_scale=1.4    # 最大缩放（1.4倍）
    )
