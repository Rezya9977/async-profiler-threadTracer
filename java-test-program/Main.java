
public class Main {
    public static void main(String[] args) throws InterruptedException {
        Thread UserThread1 = new Thread(() -> {
            System.out.println(1); 
            try {
                Thread.sleep(100000);
            } catch (Exception e) {
                // TODO: handle exception
            }
        });
        UserThread1.setName("User-Thread-1");
        UserThread1.start();

        Thread UserThread2 = new Thread(() -> {System.out.println(2);});
        UserThread2.setName("User-Thread-2");
        UserThread2.start();

        Thread UserThread3 = new Thread(() -> {System.out.println(3);});
        UserThread3.setName("User-Thread-3");
        UserThread3.start();

        Thread UserThread4 = new Thread(() -> {System.out.println(4);});
        UserThread4.setName("User-Thread-4");
        UserThread4.start();
        
        Thread.sleep(1000000000);

    }
}
