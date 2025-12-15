package bigdata.assignment2.problem3;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import java.io.IOException;

public class WordCountCombiner3 extends Reducer<Text, IntWritable, Text, IntWritable> {
    private IntWritable result = new IntWritable();

    enum CombinerCounters {
        COMBINER_INPUT_RECORDS,
        COMBINER_OUTPUT_RECORDS
    }

    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        // 1. 初始化计数器为 0
        int sum = 0;
        int inputCount = 0;

        // 2. 遍历相同单词的计数值，累加求和
        for (IntWritable val : values) {
            sum += val.get();
            inputCount++;
        }

        // 4. 添加计数器统计 Combiner 的输入输出记录数
        context.getCounter(CombinerCounters.COMBINER_INPUT_RECORDS).increment(inputCount);
        context.getCounter(CombinerCounters.COMBINER_OUTPUT_RECORDS).increment(1);

        // 3. 输出键值对：(单词, 局部计数)
        result.set(sum);
        context.write(key, result);
    }
}
