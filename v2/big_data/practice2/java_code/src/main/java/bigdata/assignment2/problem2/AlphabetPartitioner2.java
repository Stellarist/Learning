package bigdata.assignment2.problem2;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Partitioner;

public class AlphabetPartitioner2 extends Partitioner<Text, IntWritable> {

    @Override
    public int getPartition(Text key, IntWritable value, int numPartitions) {
        // 处理异常情况：null 或空字符串
        if (key == null || key.toString().isEmpty()) {
            return 0;
        }

        // 获取单词首字母并转换为大写
        char firstChar = key.toString().charAt(0);
        char upperChar = Character.toUpperCase(firstChar);

        // 根据首字母分配分区
        if (upperChar >= 'A' && upperChar <= 'F') {
            // A-F -> 分区 0
            return 0 % numPartitions;
        } else if (upperChar >= 'G' && upperChar <= 'N') {
            // G-N -> 分区 1
            return 1 % numPartitions;
        } else if (upperChar >= 'O' && upperChar <= 'S') {
            // O-S -> 分区 2
            return 2 % numPartitions;
        } else if (upperChar >= 'T' && upperChar <= 'Z') {
            // T-Z -> 分区 3
            return 3 % numPartitions;
        } else {
            // 数字(0-9)、特殊字符等 -> 分区 0
            return 0 % numPartitions;
        }
    }
}
