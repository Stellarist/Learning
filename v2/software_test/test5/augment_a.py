import cv2
import numpy as np
import os
import random

def rotate_image(image, angle):
    """
    旋转图像
    :param image: 输入图像
    :param angle: 旋转角度
    :return: 旋转后的图像
    """
    height, width = image.shape[:2]
    center = (width // 2, height // 2)
    
    # 计算旋转矩阵
    rotation_matrix = cv2.getRotationMatrix2D(center, angle, 1.0)
    
    # 执行旋转
    rotated_img = cv2.warpAffine(image, rotation_matrix, (width, height), 
                                flags=cv2.INTER_LINEAR, 
                                borderMode=cv2.BORDER_CONSTANT,
                                borderValue=(114, 114, 114))  # 使用YOLO的灰色填充
    
    return rotated_img

def generate_rotation_augmentation(input_images, output_dir, num_rotations=10, max_angle=150):
    """
    生成旋转扩增数据（简化版，不需要标签）
    """
    # 创建输出目录
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
        # 读取图像
        image = cv2.imread(image_path)
        
        if image is None:
            print(f"无法读取图像: {image_path}")
            continue
            
        # 获取文件名（不含扩展名）
        image_file = os.path.basename(image_path)
        base_name = os.path.splitext(image_file)[0]
        extension = os.path.splitext(image_file)[1]
        
        print(f"\n处理图像: {image_file}")
        # 生成旋转样本
        for i in range(num_rotations):
            # 生成随机角度 (0到max_angle度)
            angle = random.uniform(0, max_angle)
            # 旋转图像
            rotated_image = rotate_image(image, angle)
            
            # 生成新文件名
            new_image_name = f"{base_name}_rot_{i+1:02d}_angle_{angle:.1f}{extension}"
            new_image_path = os.path.join(output_dir, new_image_name)          
            # 保存旋转后的图像
            cv2.imwrite(new_image_path, rotated_image)
            
            total_generated += 1
            print(f"生成: {new_image_name} (角度: {angle:.1f}°)")
    
    print(f"\n数据扩增完成! 总共生成 {total_generated} 个旋转样本")
    print(f"输出目录: {output_dir}")

if __name__ == "__main__":
    # 输入图片路径列表
    input_images = [
        "a.png",
        "b.png",
        "c.png"
    ]
    
    # 输出文件夹
    output_dir = "augmented_1"

    # 调用函数进行数据增强
    generate_rotation_augmentation(
        input_images=input_images,
        output_dir=output_dir,
        num_rotations=3,   # 每张图生成3张旋转图像
        max_angle=180      # 最大旋转角度
    )
