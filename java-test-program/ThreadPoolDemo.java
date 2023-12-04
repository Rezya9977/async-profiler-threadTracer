import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ThreadPoolDemo {

    public static void main(String[] args) throws InterruptedException {

        while (true) {
            ExecutorService executorService = Executors.newFixedThreadPool(100);
            // 提交任务给线程池
            for (int i = 0; i < 100; i++) {
                executorService.execute(() -> {
                    try {
                        Thread.sleep(10);
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                });
            }
            Thread.sleep(1000);
            executorService.shutdownNow();
        }

    }

}
