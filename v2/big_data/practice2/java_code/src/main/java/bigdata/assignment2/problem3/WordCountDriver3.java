package bigdata.assignment2.problem3;

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
import java.util.*;

public class WordCountDriver3 {
    public static void main(String[] args) throws Exception {
        String inputPath = "/public/data/wordcount";
        String outputPath = "/users/s522025320111/homework1/problem3/output/temp";
        String studentId = "s522025320111";

        // 是否启用 Combiner（可配置）
        boolean enableCombiner = true;

        // 配置参数
        int numReduceTasks = 4;
        long splitSize = 128 * 1024 * 1024; // 128MB

        runWordCount(inputPath, outputPath, studentId, enableCombiner, numReduceTasks, splitSize);
    }

    private static void runWordCount(String inputPathStr, String outputPathStr, String studentId,
            boolean enableCombiner, int numReduceTasks, long splitSize) throws Exception {
        // 创建 Configuration 和 Job 对象
        Configuration conf = new Configuration();

        // 设置输入分片大小以控制 Map 任务数量
        conf.setLong("mapreduce.input.fileinputformat.split.maxsize", splitSize);
        conf.setLong("mapreduce.input.fileinputformat.split.minsize", splitSize);

        Job job = Job.getInstance(conf, "Word Count 3");

        // 设置基本 Job 参数
        job.setMapperClass(WordCountMapper3.class);
        if (enableCombiner) {
            job.setCombinerClass(WordCountCombiner3.class);
        }
        job.setReducerClass(WordCountReducer3.class);

        // 设置输出键值对类型
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);
        job.setJarByClass(WordCountDriver3.class);

        // 设置 Reduce 任务数量
        job.setNumReduceTasks(numReduceTasks);

        // 实现 HDFS 操作
        Path inputPath = new Path(inputPathStr);
        Path outputPath = new Path(outputPathStr);
        FileSystem fs = FileSystem.get(conf);

        // 检查输入目录是否存在
        if (!fs.exists(inputPath)) {
            System.err.println("Input path does not exist: " + inputPath);
            System.exit(-1);
        }

        // 删除已存在的输出目录
        if (fs.exists(outputPath)) {
            System.err.println("Output path already exists, deleting: " + outputPath);
            fs.delete(outputPath, true);
        }

        // 设置输入输出路径
        FileInputFormat.addInputPath(job, inputPath);
        FileOutputFormat.setOutputPath(job, outputPath);

        // 性能监控和作业执行
        System.out.println("========Starting MapReduce Job=========");
        System.out.println("Configuration:");
        System.out.println("  - Combiner Enabled: " + enableCombiner);
        System.out.println("  - Reduce Tasks: " + numReduceTasks);
        System.out.println("  - Split Size: " + (splitSize / 1024 / 1024) + " MB");
        System.out.println("======================================");

        // 记录开始时间
        long startTime = System.currentTimeMillis();

        // 提交作业并等待完成
        boolean success = job.waitForCompletion(true);

        // 记录结束时间
        long endTime = System.currentTimeMillis();
        long processingTime = endTime - startTime;

        // 输出性能统计信息
        if (success) {
            System.out.println("\n========Job Statistics=========");

            // 从 Job 中获取 Counters 信息
            Counters counters = job.getCounters();

            long inputRecords = counters.findCounter(TaskCounter.MAP_INPUT_RECORDS).getValue();
            long mapOutputRecords = counters.findCounter(TaskCounter.MAP_OUTPUT_RECORDS).getValue();
            long reduceInputRecords = counters.findCounter(TaskCounter.REDUCE_INPUT_RECORDS).getValue();
            long reduceOutputRecords = counters.findCounter(TaskCounter.REDUCE_OUTPUT_RECORDS).getValue();
            long combinerInputRecords = 0;
            long combinerOutputRecords = 0;

            if (enableCombiner) {
                combinerInputRecords = mapOutputRecords;
                combinerOutputRecords = counters.findCounter(TaskCounter.COMBINE_OUTPUT_RECORDS).getValue();
            }

            // 获取 Map 和 Reduce 任务数量（使用可用的计数器）
            long mapTasksCount = 0;
            long reduceTasksCount = numReduceTasks; // 使用配置的数量

            // 尝试获取 Map 任务数量，如果不可用则估算
            try {
                mapTasksCount = counters.findCounter("org.apache.hadoop.mapreduce.JobCounter",
                        "TOTAL_LAUNCHED_MAPS").getValue();
            } catch (Exception e) {
                // 如果获取失败，估算为输入记录数除以分片大小
                mapTasksCount = (inputRecords > 0) ? Math.max(1, inputRecords / 100000) : 1;
            }

            // 计算总单词数
            long totalWords = mapOutputRecords;

            // 打印统计信息
            System.out.println("Total Processing Time: " + processingTime + " ms");
            System.out.println("Map Tasks Count: " + mapTasksCount);
            System.out.println("Reduce Tasks Count: " + reduceTasksCount);
            System.out.println("Input Records: " + inputRecords);
            System.out.println("Output Records (Unique Words): " + reduceOutputRecords);
            System.out.println("Total Words: " + totalWords);
            System.out.println("Combiner Enabled: " + enableCombiner);

            if (enableCombiner) {
                System.out.println("Combiner Input Records: " + combinerInputRecords);
                System.out.println("Combiner Output Records: " + combinerOutputRecords);
                double compressionRatio = (1 - (double) combinerOutputRecords / combinerInputRecords) * 100;
                System.out.println("Combiner Compression Ratio: " + String.format("%.2f", compressionRatio) + "%");
            }

            // 计算处理速度
            double throughputMBPerSec = 0;
            long inputBytes = 0;

            try {
                inputBytes = counters.findCounter("org.apache.hadoop.mapreduce.FileSystemCounter",
                        "FILE_BYTES_READ").getValue();
            } catch (Exception e) {
                // 如果获取失败，使用估算值
                inputBytes = inputRecords * 100; // 假设每行平均100字节
            }

            if (processingTime > 0 && inputBytes > 0) {
                throughputMBPerSec = (inputBytes / 1024.0 / 1024.0) / (processingTime / 1000.0);
            }
            System.out.println("Throughput: " + String.format("%.2f", throughputMBPerSec) + " MB/s");

            System.out.println("===============================");

            // 确定用户输出目录
            String userOutputDir;
            if (studentId.startsWith("/")) {
                userOutputDir = studentId;
            } else {
                userOutputDir = "/users/" + studentId + "/homework1/problem3";
            }

            // 保存结果到个人目录
            saveResultsToDirectory(fs, outputPath, processingTime, mapTasksCount, reduceTasksCount,
                    inputRecords, reduceOutputRecords, totalWords, enableCombiner,
                    combinerInputRecords, combinerOutputRecords, userOutputDir);
        }

        System.exit(success ? 0 : 1);
    }

    // 保存结果到指定目录
    private static void saveResultsToDirectory(FileSystem fs, Path outputPath,
            long processingTime, long mapTasksCount, long reduceTasksCount,
            long inputRecords, long outputRecords, long totalWords,
            boolean combinerEnabled, long combinerInputRecords, long combinerOutputRecords,
            String userOutputDir) throws Exception {

        // 读取所有 part-r-* 文件并按词频排序
        Map<String, Integer> wordCountMap = new HashMap<>();

        FileStatus[] statuses = fs.listStatus(outputPath);
        for (FileStatus status : statuses) {
            String name = status.getPath().getName();
            if (name.startsWith("part-r-")) {
                try (BufferedReader br = new BufferedReader(
                        new InputStreamReader(fs.open(status.getPath())))) {
                    String line;
                    while ((line = br.readLine()) != null) {
                        String[] parts = line.split("\\s+");
                        if (parts.length >= 2) {
                            String key = parts[0];
                            Integer val = Integer.valueOf(parts[parts.length - 1]);
                            wordCountMap.put(key, val);
                        }
                    }
                }
            }
        }

        // 按词频降序排序
        List<Map.Entry<String, Integer>> sortedList = new ArrayList<>(wordCountMap.entrySet());
        sortedList.sort((e1, e2) -> e2.getValue().compareTo(e1.getValue()));

        // 确保用户目标目录存在
        Path userDirPath = new Path(userOutputDir);
        if (!fs.exists(userDirPath)) {
            fs.mkdirs(userDirPath);
        }

        // 保存 word-count-results.txt（按词频降序）
        Path resultsFile = new Path(userDirPath, "word-count-results.txt");
        try (FSDataOutputStream out = fs.create(resultsFile, true)) {
            for (Map.Entry<String, Integer> entry : sortedList) {
                String output = entry.getKey() + "\t" + entry.getValue() + "\n";
                out.write(output.getBytes());
            }
        }
        System.out.println("Results file saved to: " + resultsFile.toString());

        // 保存 performance-report.txt（按统计项名称字典序）
        Path reportFile = new Path(userDirPath, "performance-report.txt");
        try (FSDataOutputStream out = fs.create(reportFile, true)) {
            TreeMap<String, String> statsMap = new TreeMap<>();

            statsMap.put("combiner_enabled", String.valueOf(combinerEnabled));
            statsMap.put("combiner_input_records", String.valueOf(combinerInputRecords));
            statsMap.put("combiner_output_records", String.valueOf(combinerOutputRecords));
            statsMap.put("input_records", String.valueOf(inputRecords));
            statsMap.put("map_tasks_count", String.valueOf(mapTasksCount));
            statsMap.put("output_records", String.valueOf(outputRecords));
            statsMap.put("reduce_tasks_count", String.valueOf(reduceTasksCount));
            statsMap.put("total_processing_time", String.valueOf(processingTime));
            statsMap.put("total_words", String.valueOf(totalWords));

            for (Map.Entry<String, String> entry : statsMap.entrySet()) {
                String output = entry.getKey() + "\t" + entry.getValue() + "\n";
                out.write(output.getBytes());
            }
        }
        System.out.println("Performance report saved to: " + reportFile.toString());
    }
}
