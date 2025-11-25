package bigdata.assignment2.problem3;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import java.io.IOException;

public class WordCountReducer3 extends Reducer<Text, IntWritable, Text, IntWritable> {
    private IntWritable result = new IntWritable();

    // 定义自定义计数器
    enum ReducerCounters {
        TOTAL_UNIQUE_WORDS,
        TOTAL_WORD_COUNT
    }

    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        // 1. 初始化计数器为 0
        int sum = 0;

        // 2. 遍历来自 Combiner 的局部计数，累加求和
        for (IntWritable val : values) {
            sum += val.get();
        }

        // 4. 添加计数器统计最终输出的单词数
        context.getCounter(ReducerCounters.TOTAL_UNIQUE_WORDS).increment(1);
        context.getCounter(ReducerCounters.TOTAL_WORD_COUNT).increment(sum);

        // 3. 输出最终结果：(单词, 总计数)
        result.set(sum);
        context.write(key, result);
    }
}
