public class RMIClient {
    public static void main(String[] args) {
        String text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.";

        int numThreads = 4; // Número de threads para processamento paralelo
        int totalWords = TextProcessor.processTextInParallel(text, numThreads);
        System.out.println("Total de palavras no texto: " + totalWords);
    }
}
