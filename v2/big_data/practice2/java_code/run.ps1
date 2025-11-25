docker run -d --name hadoop-client ^
  -p 9000:9000 -p 9870:9870 -p 8032:8032 -p 8088:8088 -p 19888:19888 ^
  -v C:\Data\repos\big_data\practice2\java_code:/home/hadoop/java_code ^
  --add-host hadoop-master:172.19.240.155 ^
  --add-host hadoop-worker1:172.19.240.129 ^
  --add-host hadoop-worker2:172.19.241.168 ^
  --add-host hadoop-worker3:172.19.241.70 ^
  --add-host hadoop-worker4:172.19.241.153 ^
  --cap-add=NET_RAW --cap-add=NET_ADMIN ^
  hadoop-client tail -f /dev/null

