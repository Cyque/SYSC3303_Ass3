import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.concurrent.Callable;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

public class Server {

	static int idCount = 0;

	public static void main(String[] args) {
		ExecutorService executor = Executors.newCachedThreadPool();

		try {
			ServerSocket socket = new ServerSocket(2000);

			boolean findClients = true;

			ArrayList<Future<Integer>> futureStack = new ArrayList<Future<Integer>>();

			while (findClients) {

				Socket clientSocket = socket.accept();
				Future<Integer> result = executor
						.submit(new Callable<Integer>() {

							@Override
							public Integer call() throws Exception {
								return new ClientConnection(clientSocket,
										idCount++).run();
							}
						});
				futureStack.add(result);
				
				//TODO: print exec times
			}

			socket.close();

			executor.shutdown();
			executor.awaitTermination(1, TimeUnit.DAYS);

		} catch (Exception e1) {
			e1.printStackTrace();
		}
	}

	// from user nosid
	// http://stackoverflow.com/questions/23301598/transform-java-future-into-a-completablefuture
	public static <T> CompletableFuture<T> makeCompletableFuture(
			Future<T> future) {
		return CompletableFuture.supplyAsync(() -> {
			try {
				return future.get();
			} catch (InterruptedException | ExecutionException e) {
				throw new RuntimeException(e);
			}
		});
	}
}
