import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ThreadPoolDemo {

    public static void main(String[] args) {
        // 创建三个线程池
        ExecutorService executorService1 = Executors.newFixedThreadPool(5);
        ExecutorService executorService2 = Executors.newFixedThreadPool(5);
        ExecutorService executorService3 = Executors.newFixedThreadPool(5);

        // 启动每个线程池的五个线程
        startThreadsInPool(executorService1, "Pool 1");
        startThreadsInPool(executorService2, "Pool 2");
        startThreadsInPool(executorService3, "Pool 3");

        // 在主线程中阻塞
        blockMainThread();
    }

    private static void startThreadsInPool(ExecutorService executorService, String poolName) {
        for (int i = 1; i <= 5; i++) {
            final int threadNumber = i;
            executorService.execute(() -> {
                System.out.println(poolName + " - Thread " + threadNumber + " is running");
                try {
                    // 模拟线程执行一些任务
                    Thread.sleep(200000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                System.out.println(poolName + " - Thread " + threadNumber + " finished");
            });
        }
    }

    private static void blockMainThread() {
        // 在主线程中阻塞，等待所有线程完成
        try {
            System.out.println("Main thread is waiting...");
            Thread.sleep(1000000); // 这里阻塞主线程10秒
            System.out.println("Main thread resumes after waiting");
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
