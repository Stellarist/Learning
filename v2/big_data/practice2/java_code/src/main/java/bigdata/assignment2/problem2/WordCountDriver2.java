package bigdata.assignment2.problem2;

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

public class WordCountDriver2 {
    public static void main(String[] args) throws Exception {
        String inputPath = "/public/data/wordcount";
        String outputPath = "/users/s522025320111/homework1/problem2/output/temp";
        String studentId = "s522025320111";

        runWordCount(inputPath, outputPath, studentId);
    }

    private static void runWordCount(String inputPathStr, String outputPathStr, String studentId) throws Exception {
        // 创建 Configuration 和 Job 对象
        Configuration conf = new Configuration();

        Job job = Job.getInstance(conf, "Word Count 2");

        // 设置 Mapper, Combiner, Reducer, Partitioner 类和 Reduce 任务数
        job.setMapperClass(WordCountMapper2.class);
        job.setCombinerClass(WordCountCombiner2.class);
        job.setReducerClass(WordCountReducer2.class);
        job.setPartitionerClass(AlphabetPartitioner2.class);
        job.setNumReduceTasks(4);

        // 设置输出键值类型
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);
        job.setJarByClass(WordCountDriver2.class);

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

            // Map 输出记录数 = Combiner 输入记录数
            long mapOutputRecords = counters.findCounter(TaskCounter.MAP_OUTPUT_RECORDS).getValue();
            // Combine 输出记录数 = Combiner 输出记录数
            long combineOutputRecords = counters.findCounter(TaskCounter.COMBINE_OUTPUT_RECORDS).getValue();
            // Reduce 输入记录数
            long reduceInputRecords = counters.findCounter(TaskCounter.REDUCE_INPUT_RECORDS).getValue();
            // Reduce 输出记录数
            long reduceOutputRecords = counters.findCounter(TaskCounter.REDUCE_OUTPUT_RECORDS).getValue();

            // 打印统计信息
            System.out.println("Input Files: " + inputFileCount);
            System.out.println("Map Output Records (Combiner Input): " + mapOutputRecords);
            System.out.println("Combiner Output Records: " + combineOutputRecords);
            System.out.println("Reduce Input Records: " + reduceInputRecords);
            System.out.println("Reduce Output Records (Unique Words): " + reduceOutputRecords);
            System.out.println("Processing Time: " + processingTime + " ms");

            // 计算数据压缩率
            if (mapOutputRecords > 0) {
                double compressionRatio = (1 - (double) combineOutputRecords / mapOutputRecords) * 100;
                System.out.println("Combiner Compression Ratio: " + String.format("%.2f", compressionRatio) + "%");
            }

            System.out.println("===============================");

            // 确定用户输出目录
            String userOutputDir;
            if (studentId.startsWith("/")) {
                userOutputDir = studentId;
            } else {
                userOutputDir = "/users/" + studentId + "/homework1/problem2";
            }

            // 保存结果到个人目录
            saveResultsToDirectory(fs, outputPath, mapOutputRecords, combineOutputRecords,
                    reduceOutputRecords, inputFileCount, processingTime, userOutputDir);
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
            long combinerInputRecords, long combinerOutputRecords,
            long uniqueWords, int inputFileCount, long processingTime,
            String userOutputDir) throws Exception {

        // 使用 TreeMap 保存词频统计结果（自动按字典序排序）
        TreeMap<String, Integer> wordCountMap = new TreeMap<>();

        // 统计各分区的记录数
        long[] partitionRecords = new long[4];

        // 读取 outputPath 下所有 part-r-* 文件并合并结果
        FileStatus[] statuses = fs.listStatus(outputPath);
        for (FileStatus status : statuses) {
            String name = status.getPath().getName();
            if (name.startsWith("part-r-")) {
                // 提取分区号（part-r-00000 -> 0, part-r-00001 -> 1, ...）
                int partitionId = Integer.parseInt(name.substring(7, 12));

                long recordCount = 0;
                try (BufferedReader br = new BufferedReader(
                        new InputStreamReader(fs.open(status.getPath())))) {
                    String line;
                    while ((line = br.readLine()) != null) {
                        recordCount++;
                        String[] parts = line.split("\\s+");
                        if (parts.length >= 2) {
                            String key = parts[0];
                            Integer val = Integer.valueOf(parts[parts.length - 1]);
                            wordCountMap.put(key, wordCountMap.getOrDefault(key, 0) + val);
                        }
                    }
                }

                // 记录该分区的记录数
                if (partitionId < 4) {
                    partitionRecords[partitionId] = recordCount;
                }
            }
        }

        // 计算总单词数（所有词频之和）
        long totalWords = 0;
        for (Integer count : wordCountMap.values()) {
            totalWords += count;
        }

        // 确保用户目标目录存在
        Path userDirPath = new Path(userOutputDir);
        if (!fs.exists(userDirPath)) {
            fs.mkdirs(userDirPath);
        }

        // 保存 words.txt（按字典序排列）
        Path wordsFile = new Path(userDirPath, "words.txt");
        try (FSDataOutputStream wordsOut = fs.create(wordsFile, true)) {
            for (Map.Entry<String, Integer> entry : wordCountMap.entrySet()) {
                String output = entry.getKey() + "\t" + entry.getValue() + "\n";
                wordsOut.write(output.getBytes());
            }
        }
        System.out.println("Words file saved to: " + wordsFile.toString());

        // 保存 statistics.txt（按统计项名称字典序排列）
        Path statsFile = new Path(userDirPath, "statistics.txt");
        try (FSDataOutputStream statsOut = fs.create(statsFile, true)) {
            TreeMap<String, String> statsMap = new TreeMap<>();

            // 添加必需的统计项
            statsMap.put("combiner_input_records", String.valueOf(combinerInputRecords));
            statsMap.put("combiner_output_records", String.valueOf(combinerOutputRecords));
            statsMap.put("partition_0_records", String.valueOf(partitionRecords[0]));
            statsMap.put("partition_1_records", String.valueOf(partitionRecords[1]));
            statsMap.put("partition_2_records", String.valueOf(partitionRecords[2]));
            statsMap.put("partition_3_records", String.valueOf(partitionRecords[3]));
            statsMap.put("total_words", String.valueOf(totalWords));
            statsMap.put("unique_words", String.valueOf(uniqueWords));

            // 写入文件
            for (Map.Entry<String, String> entry : statsMap.entrySet()) {
                String output = entry.getKey() + "\t" + entry.getValue() + "\n";
                statsOut.write(output.getBytes());
            }
        }
        System.out.println("Statistics file saved to: " + statsFile.toString());

        // 打印分区统计信息
        System.out.println("\n========Partition Statistics=========");
        for (int i = 0; i < 4; i++) {
            System.out.println("Partition " + i + ": " + partitionRecords[i] + " records");
        }
        System.out.println("=====================================");
    }
}
