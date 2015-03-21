import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Random;

public class Client {
	private static Random rndGen = new Random();
	

	public static void main(String[] args) {
		String hostName = "localhost";
		int portNumber = 2000;

		try {
			Socket socket = new Socket(hostName, portNumber);

			PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
			BufferedReader in = new BufferedReader(new InputStreamReader(
					socket.getInputStream()));

			int id = Integer.parseInt(in.readLine());

			out.println("Client " + id + ": Hello. I am doing work.");
			Thread.sleep(1000 + rndGen.nextInt(1000));
			out.println("Client " + id + ": Bye. I am finished.");
			
			socket.close();
			
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

}
