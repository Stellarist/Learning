import cv2
import numpy as np
import os
import random

def adjust_brightness(image, brightness_factor):
    """
    调整图像亮度
    :param image: 输入图像
    :param brightness_factor: 亮度系数（>1变亮，<1变暗）
    :return: 调整亮度后的图像
    """
    # 转换为浮点计算，避免溢出
    image = image.astype(np.float32)
    image = image * brightness_factor
    # 限制在[0,255]
    image = np.clip(image, 0, 255)
    return image.astype(np.uint8)


def generate_brightness_augmentation(input_images, output_dir, num_variants=10, min_factor=0.5, max_factor=1.5):
    """
    生成亮度扩增数据（不需要标签）
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
        for i in range(num_variants):
            # 生成随机亮度系数
            factor = random.uniform(min_factor, max_factor)
            adjusted_image = adjust_brightness(image, factor)
            
            # 新文件名
            new_image_name = f"{base_name}_bright_{i+1:02d}_{factor:.2f}{extension}"
            new_image_path = os.path.join(output_dir, new_image_name)
            
            cv2.imwrite(new_image_path, adjusted_image)
            total_generated += 1
            print(f"生成: {new_image_name} (亮度系数: {factor:.2f})")
    
    print(f"\n数据扩增完成! 总共生成 {total_generated} 个亮度样本")
    print(f"输出目录: {output_dir}")


if __name__ == "__main__":
    # 输入图片路径列表
    input_images = [
        "a.png",
        "b.png",
        "c.png"
    ]
    
    output_dir = "augmented_3"

    # 执行亮度增强
    generate_brightness_augmentation(
        input_images=input_images,
        output_dir=output_dir,
        num_variants=3,    # 每张图生成3个亮度版本
        min_factor=0.3,    # 最暗0.3倍
        max_factor=0.7     # 最亮0.7倍
    )

