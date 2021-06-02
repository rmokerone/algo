CSHMFIFO
===

# 进程间共享内存 环形缓冲区

# 查看共享内存

cshmfifo$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages    

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status      
0x00000000 11         fucky      600        67108864   2          dest         
0x00000000 15         fucky      600        524288     2          dest         
0x00000000 18         fucky      600        524288     2          dest         
0x00000000 21         fucky      600        524288     2          dest         
0x00000000 24         fucky      600        524288     2          dest         
0x00000000 26         fucky      600        524288     2          dest         
0x00000000 27         fucky      600        524288     2          dest         
0x0000162e 28         fucky      666        10256      0                       

------ Semaphore Arrays --------
key        semid      owner      perms      nsems     
0x0000162e 4          fucky      666        1         
0x0000162f 5          fucky      666        1         
0x00001630 6          fucky      666        1   


# 参考资料

https://www.bilibili.com/video/BV1q5411P7KB?p=1
