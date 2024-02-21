import java.util.concurrent.atomic.AtomicInteger;

public class TextProcessor {
    public static int processTextInParallel(String text, int numThreads) {
        AtomicInteger totalWords = new AtomicInteger(0);
        Thread[] threads = new Thread[numThreads];
        final String[] textPieces = splitText(text, numThreads);

        for (int i = 0; i < numThreads; i++) {
            final int index = i;
            threads[i] = new Thread(() -> {
                try {
                    TextProcessingService service = (TextProcessingService) java.rmi.Naming
                            .lookup("//localhost/TextProcessingService");
                    int wordCount = service.processText(textPieces[index]);
                    totalWords.addAndGet(wordCount);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            });
            threads[i].start();
        }

        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        return totalWords.get();
    }

    private static String[] splitText(String text, int numPieces) {
        String[] pieces = new String[numPieces];
        int length = text.length();
        int chunkSize = (int) Math.ceil((double) length / numPieces);
        int startPos = 0;

        for (int i = 0; i < numPieces; i++) {
            int endPos = Math.min(startPos + chunkSize, length);
            pieces[i] = text.substring(startPos, endPos);
            startPos = endPos;
        }

        return pieces;
    }
}
