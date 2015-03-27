public class TestAll {

	public static void main(String[] args) {

		// start the server
		System.out.println("Starting Server...");
		new Thread(new Runnable() {
			public void run() {
				Server.main(null);

			}
		}).start();

		// start n clients
		System.out.println("Starting Client Connection...");
		for (int i = 0; i < 10; i++) {
			new Thread(new Runnable() {
				public void run() {
					Client.main(null);
				}
			}).start();
		}

	}

}
