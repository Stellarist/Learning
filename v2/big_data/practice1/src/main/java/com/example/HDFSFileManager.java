package com.example;

import java.io.File;
import java.io.IOException;
import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class HDFSFileManager {
    private static final Logger logger = LoggerFactory.getLogger(HDFSFileManager.class);

    private final FileSystem fileSystem;
    private final Configuration configuration;

    public HDFSFileManager(String hdfsUri) throws IOException {
        configuration = new Configuration();
        configuration.set("fs.defaultFS", hdfsUri);

        fileSystem = FileSystem.get(URI.create(hdfsUri), configuration);
        logger.info("Connected to HDFS at {}", hdfsUri);
    }

    public boolean uploadFile(String localPath, String hdfsPath, boolean overwrite) {
        File localFile = new File(localPath);
        if (!localFile.exists()) {
            logger.error("Local file doesn't exist: {}", localPath);
            return false;
        }

        try {
            Path destPath = new Path(hdfsPath);
            Path parent = destPath.getParent();
            if (!fileSystem.exists(parent))
                fileSystem.mkdirs(parent);

            fileSystem.copyFromLocalFile(overwrite, true, new Path(localPath), destPath);
            logger.info("File uploaded suceess: {} -> {}", localPath, hdfsPath);
            return true;

        } catch (IOException e) {
            logger.error("File uploaded failed", e);
            return false;
        }
    }

    public boolean downloadFile(String hdfsPath, String localPath, boolean overwrite) {
        File localFile = new File(localPath);
        if (localFile.exists() && !overwrite) {
            logger.warn("Local file exists and overwrite not allowed: {}", localPath);
            return false;
        }

        try {
            fileSystem.copyToLocalFile(false, new Path(hdfsPath), new Path(localPath), overwrite);
            logger.info("File downloaded successfully: {} -> {}", hdfsPath, localPath);
            return true;

        } catch (IOException e) {
            logger.error("File download failed", e);
            return false;
        }
    }

    public boolean deleteFile(String hdfsPath, boolean recursive) {
        try {
            boolean result = fileSystem.delete(new Path(hdfsPath), recursive);

            if (result) {
                logger.info("Deleted successfully: {}", hdfsPath);
            } else {
                logger.warn("Deletion failed or file does not exist: {}", hdfsPath);
            }

            return result;

        } catch (IOException e) {
            logger.error("Deletion error", e);
            return false;
        }
    }

    public void listDirectory(String hdfsPath, int depth) {
        try {
            Path path = new Path(hdfsPath);
            if (!fileSystem.exists(path)) {
                logger.warn("Path does not exist: {}", hdfsPath);
                return;
            }

            FileStatus[] statuses = fileSystem.listStatus(path);
            String prefix = "  ".repeat(depth);

            for (FileStatus status : statuses) {
                if (status.isDirectory()) {
                    System.out.println(prefix + "[DIR] " + status.getPath().getName());
                    listDirectory(status.getPath().toString(), depth + 1);
                } else {
                    System.out.println(prefix + "[FILE] " + status.getPath().getName());
                }
            }

        } catch (IOException e) {
            logger.error("Directory listing failed", e);
        }
    }

    public DirectoryStats getDirectoryStats(String hdfsPath) {
        DirectoryStats stats = new DirectoryStats();
        try {
            Path path = new Path(hdfsPath);
            if (!fileSystem.exists(path))
                return stats;
            FileStatus[] statuses = fileSystem.listStatus(path);
            for (FileStatus status : statuses) {
                if (status.isDirectory()) {
                    stats.incrementDirectory();
                    DirectoryStats subStats = getDirectoryStats(status.getPath().toString());
                    stats.add(subStats);
                } else {
                    stats.incrementFile();
                    stats.addSize(status.getLen());
                }
            }
        } catch (IOException e) {
            logger.error("Directory statistics failed", e);
        }
        return stats;
    }

    public void close() {
        try {
            if (fileSystem != null) {
                fileSystem.close();
                logger.info("HDFS connection closed");
            }
        } catch (IOException e) {
            logger.error("Error closing HDFS", e);
        }
    }

    public static class DirectoryStats {
        private long fileCount;
        private long directoryCount;
        private long totalSize;

        public void incrementFile() {
            fileCount++;
        }

        public void incrementDirectory() {
            directoryCount++;
        }

        public void addSize(long size) {
            totalSize += size;
        }

        public void add(DirectoryStats other) {
            this.fileCount += other.fileCount;
            this.directoryCount += other.directoryCount;
            this.totalSize += other.totalSize;
        }

        @Override
        public String toString() {
            return String.format("File count: %d, Directory count: %d, Total size: %d bytes", fileCount, directoryCount,
                    totalSize);
        }
    }

    public static void main(String[] args) {
        HDFSFileManager manager = null;

        try {
            manager = new HDFSFileManager("hdfs://localhost:9000");

            // Test upload
            manager.uploadFile("a.txt", "/user/student/project/input/a.txt", true);

            // List directory
            System.out.println("Project directory structure:");
            manager.listDirectory("/user/student/project", 0);

            // Test download
            manager.downloadFile("/user/student/project/input/a.txt", "a_copy.txt", true);

            // Get directory statistics
            DirectoryStats stats = manager.getDirectoryStats("/user/student/project");
            System.out.println("Directory statistics: " + stats);

            // Test file deletion
            manager.deleteFile("/user/student/project/input/a.txt", false);

        } catch (IOException e) {
            logger.error("Execution error", e);

        } finally {
            if (manager != null)
                manager.close();
        }
    }
}
