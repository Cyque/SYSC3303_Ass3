import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.concurrent.Semaphore;

public class ClientConnection {

	//maximum 4 client connection threads
	private static Semaphore sem = new Semaphore(4, true);

	private static int totalClientConnections = 0;

	private Socket socket;
	private int id;

	public ClientConnection(Socket socket, int id) {
		this.socket = socket;
		this.id = id;
	}

	public Integer run() {

		long totalTime = 0;// = new
		try {
			sem.acquire();
			long startTime = System.currentTimeMillis();
			PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
			BufferedReader in = new BufferedReader(new InputStreamReader(
					socket.getInputStream()));

			out.println(id);

			System.out.println(in.readLine());
			totalClientConnections++;
			System.out.println("Total Client Connections: "
					+ totalClientConnections);
			System.out.println(in.readLine());
			socket.close();

			totalClientConnections--;
			totalTime = System.currentTimeMillis() - startTime;
			sem.release();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return (int) totalTime;
	}
}
