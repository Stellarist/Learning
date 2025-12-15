package bigdata.assignment2.problem2;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import java.io.IOException;

/**
 * WordCountCombiner - Map 端本地聚合
 * 在 Shuffle 之前对 Map 输出进行本地合并，减少网络传输数据量
 * Combiner 逻辑与 Reducer 完全相同
 */
public class WordCountCombiner2 extends Reducer<Text, IntWritable, Text, IntWritable> {
    private final IntWritable result = new IntWritable();

    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        // 1. 初始化计数器为 0
        int sum = 0;

        // 2. 遍历 values 中的所有计数值并累加
        for (IntWritable val : values) {
            sum += val.get();
        }

        // 3. 将累加结果写入 result 变量
        result.set(sum);

        // 4. 输出合并后的 (单词, 本地总计数) 结果
        context.write(key, result);
    }
}
