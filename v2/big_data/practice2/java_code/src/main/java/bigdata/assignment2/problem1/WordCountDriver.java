package bigdata.assignment2.problem1;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.*;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Counters;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.TaskCounter;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Map;
import java.util.TreeMap;

public class WordCountDriver {
    public static void main(String[] args) throws Exception {
        String inputPath = "/public/data/wordcount";
        String outputPath = "/users/s522025320111/homework1/problem1/output/temp";
        String studentId = "s522025320111";

        runWordCount(inputPath, outputPath, studentId);
    }

    private static void runWordCount(String inputPathStr, String outputPathStr, String studentId) throws Exception {
        // 创建 Configuration 和 Job 对象
        Configuration conf = new Configuration();

        Job job = Job.getInstance(conf, "Word Count");

        // 设置 Job 参数
        job.setMapperClass(WordCountMapper.class);
        job.setReducerClass(WordCountReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);
        job.setJarByClass(WordCountDriver.class);

        // 实现 HDFS 操作
        Path inputPath = new Path(inputPathStr);
        Path outputPath = new Path(outputPathStr);
        FileSystem fs = FileSystem.get(conf);

        // 检查输入目录是否存在
        if (!fs.exists(inputPath)) {
            System.err.println("Input path does not exist: " + inputPath);
            System.exit(-1);
        }

        // 统计输入文件数量
        int inputFileCount = countInputFiles(fs, inputPath);

        // 删除已存在的输出目录
        if (fs.exists(outputPath)) {
            System.err.println("Output path already exists, deleting: " + outputPath);
            fs.delete(outputPath, true);
        }

        // 设置输入输出路径
        FileInputFormat.addInputPath(job, inputPath);
        FileOutputFormat.setOutputPath(job, outputPath);

        // 记录开始时间
        long startTime = System.currentTimeMillis();

        // 提交作业并等待完成
        boolean success = job.waitForCompletion(true);

        // 记录结束时间并计算处理时间
        long endTime = System.currentTimeMillis();
        long processingTime = endTime - startTime;

        // 显示处理结果和统计信息
        if (success) {
            System.out.println("========Job Statistics=========");

            // 获取 Counters 信息
            Counters counters = job.getCounters();
            long totalWords = counters.findCounter(TaskCounter.MAP_OUTPUT_RECORDS).getValue();
            long uniqueWords = counters.findCounter(TaskCounter.REDUCE_OUTPUT_RECORDS).getValue();

            // 打印统计信息
            System.out.println("Input Files: " + inputFileCount);
            System.out.println("Total Words: " + totalWords);
            System.out.println("Unique Words: " + uniqueWords);
            System.out.println("Processing Time: " + processingTime + " ms");
            System.out.println("===============================");

            // 确定用户输出目录
            String userOutputDir;
            if (studentId.startsWith("/")) {
                userOutputDir = studentId;
            } else {
                userOutputDir = "/users/" + studentId + "/homework1/problem1";
            }

            // 保存结果到个人目录
            saveResultsToDirectory(fs, outputPath, totalWords, uniqueWords,
                    inputFileCount, processingTime, userOutputDir);
        }

        System.exit(success ? 0 : 1);
    }

    // 统计输入文件数量
    private static int countInputFiles(FileSystem fs, Path inputPath) throws Exception {
        int count = 0;
        FileStatus[] statuses = fs.listStatus(inputPath);
        for (FileStatus status : statuses) {
            if (status.isFile()) {
                count++;
            } else if (status.isDirectory()) {
                count += countInputFiles(fs, status.getPath());
            }
        }
        return count;
    }

    // 保存结果到指定目录
    private static void saveResultsToDirectory(FileSystem fs, Path outputPath,
            long totalWords, long uniqueWords,
            int inputFileCount, long processingTime,
            String userOutputDir) throws Exception {
        TreeMap<String, Integer> wordCountMap = new TreeMap<>();

        // 读取 outputPath 下所有 part-* 文件并合并结果
        FileStatus[] statuses = fs.listStatus(outputPath);
        for (FileStatus status : statuses) {
            String name = status.getPath().getName();
            if (name.startsWith("part-")) {
                try (BufferedReader br = new BufferedReader(
                        new InputStreamReader(fs.open(status.getPath())))) {
                    String line;
                    while ((line = br.readLine()) != null) {
                        String[] parts = line.split("\\s+");
                        if (parts.length >= 2) {
                            String key = parts[0];
                            Integer val = Integer.valueOf(parts[parts.length - 1]);
                            wordCountMap.put(key, wordCountMap.getOrDefault(key, 0) + val);
                        }
                    }
                }
            }
        }

        // 确保用户目标目录存在
        Path userDirPath = new Path(userOutputDir);
        if (!fs.exists(userDirPath)) {
            fs.mkdirs(userDirPath);
        }

        // 保存 words.txt
        Path wordsFile = new Path(userDirPath, "words.txt");
        try (FSDataOutputStream wordsOut = fs.create(wordsFile, true)) {
            for (Map.Entry<String, Integer> entry : wordCountMap.entrySet()) {
                String output = entry.getKey() + "\t" + entry.getValue() + "\n";
                wordsOut.write(output.getBytes());
            }
        }
        System.out.println("Words file saved to: " + wordsFile.toString());

        // 保存 statistics.txt
        Path statsFile = new Path(userDirPath, "statistics.txt");
        try (FSDataOutputStream statsOut = fs.create(statsFile, true)) {
            TreeMap<String, String> statsMap = new TreeMap<>();
            statsMap.put("input_files", String.valueOf(inputFileCount));
            statsMap.put("processing_time", String.valueOf(processingTime));
            statsMap.put("total_words", String.valueOf(totalWords));
            statsMap.put("unique_words", String.valueOf(uniqueWords));

            for (Map.Entry<String, String> entry : statsMap.entrySet()) {
                String output = entry.getKey() + "\t" + entry.getValue() + "\n";
                statsOut.write(output.getBytes());
            }
        }
        System.out.println("Statistics file saved to: " + statsFile.toString());
    }
}
