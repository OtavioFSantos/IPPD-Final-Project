import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class TextProcessingServiceImpl extends UnicastRemoteObject implements TextProcessingService {
    protected TextProcessingServiceImpl() throws RemoteException {
        super();
    }

    @Override
    public int processText(String text) throws RemoteException {
        // Aqui você pode realizar suas operações de processamento de texto
        // Por exemplo, contar o número de palavras no texto
        return text.split("\\s+").length;
    }
}
