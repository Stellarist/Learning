package bigdata.assignment2.problem1;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class WordCountReducer extends Reducer<Text, IntWritable, Text, IntWritable> {
    final private IntWritable result = new IntWritable();

    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        // 1. 初始化计数器为 0
        int sum = 0;

        // 2. 遍历 values 中的所有计数值并累加
        for (IntWritable val : values)
            sum += val.get();

        // 3. 将累加结果写入 result 变量
        result.set(sum);

        // 4. 输出最终的 (单词, 总计数) 结果
        context.write(key, result);
    }
}
