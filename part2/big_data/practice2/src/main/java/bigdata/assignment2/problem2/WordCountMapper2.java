package bigdata.assignment2.problem2;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

public class WordCountMapper2 extends Mapper<LongWritable, Text, Text, IntWritable> {
    private final static IntWritable one = new IntWritable(1);

    final private Text word = new Text();

    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        // 1. 获取输入行并转换为小写
        String line = value.toString().toLowerCase();

        // 2. 按空格分割单词
        String[] words = line.split("\\s+");

        // 3. 清理单词：去除标点符号，过滤空字符串
        for (String w : words) {
            String cleanedWord = w.replaceAll("[^a-zA-Z0-9]", "");

            // 4. 输出 (单词, 1) 键值对
            if (!cleanedWord.isEmpty()) {
                word.set(cleanedWord);
                context.write(word, one);
            }
        }
    }
}
