package bigdata.assignment2.problem3;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import java.io.IOException;

public class WordCountMapper3 extends Mapper<LongWritable, Text, Text, IntWritable> {
    private Text word = new Text();
    private IntWritable one = new IntWritable(1);

    // 定义自定义计数器
    enum MapCounters {
        TOTAL_WORDS_PROCESSED,
        TOTAL_LINES_PROCESSED,
        INVALID_LINES_SKIPPED
    }

    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            // 统计处理的行数
            context.getCounter(MapCounters.TOTAL_LINES_PROCESSED).increment(1);

            // 1. 将输入行转换为小写
            String line = value.toString().toLowerCase();

            // 检查是否为空行
            if (line.trim().isEmpty()) {
                context.getCounter(MapCounters.INVALID_LINES_SKIPPED).increment(1);
                return;
            }

            // 2. 按空格分割为单词
            String[] words = line.split("\\s+");

            // 3. 过滤非字母字符，只保留有效单词
            for (String w : words) {
                // 去除标点符号，只保留字母和数字
                String cleanedWord = w.replaceAll("[^a-zA-Z0-9]", "");

                // 4. 输出键值对：(单词, 1)
                if (!cleanedWord.isEmpty()) {
                    word.set(cleanedWord);
                    context.write(word, one);
                    // 统计处理的单词数
                    context.getCounter(MapCounters.TOTAL_WORDS_PROCESSED).increment(1);
                }
            }
        } catch (Exception e) {
            // 5. 异常处理：跳过无效行并记录日志
            context.getCounter(MapCounters.INVALID_LINES_SKIPPED).increment(1);
            System.err.println("Error processing line: " + e.getMessage());
        }
    }
}
