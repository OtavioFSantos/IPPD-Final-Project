import java.rmi.Remote;
import java.rmi.RemoteException;

public interface TextProcessingService extends Remote {
    int processText(String text) throws RemoteException;
}
