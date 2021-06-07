import java.io.*;
import java.net.*;
import gnu.getopt.Getopt;
//Imports WebService
import java.net.URL;
import java.util.List;

import clientws.ServicioService;
import clientws.ServicioServiceService;


class client extends Thread {

	/********************* TYPES **********************/

	/**
	 * @brief Return codes for the protocol methods
	 */
	private static enum RC {
		OK, ERROR, USER_ERROR
	};

	/******************* ATTRIBUTES *******************/

	private static String _server = null;
	private static int _port = -1;
	public static boolean connected;
	private static ServerSocket soc = null;
	private static Thread th_g = null;
	private static String transmitter = null;

	/********************* METHODS ********************/
	// Pasa un input del socket por parametro y devuelve un string mensaje
	static String ReadString(DataInputStream in) {
		byte[] ch = new byte[1];

		String mensaje = new String();
		try {

			do {
				ch[0] = in.readByte();
				if (ch[0] != '\0') {
					String d = new String(ch);
					mensaje = mensaje + d;
				}
			} while (ch[0] != '\0');
		} catch (Exception e) {
			System.out.println("Excepcion" + e);
		}
		return mensaje;
	}

	/**
	 * @param user - User name to register in the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user is already registered
	 * @return ERROR if another error occurred
	 */
	static RC register(String user) {
		try {
			Socket sc = new Socket(_server, _port);
			DataOutputStream outputStream = new DataOutputStream(sc.getOutputStream());
			// Envio de register
			outputStream.writeBytes("REGISTER");
			outputStream.write('\0');
			outputStream.flush();
			// Envio de usuario
			outputStream.writeBytes(user);
			outputStream.write('\0');
			outputStream.flush();
			DataInputStream inputStream = new DataInputStream(sc.getInputStream());
			String err = ReadString(inputStream);
			if (err.equals("0")) {
				System.out.println("REGISTER OK");

			} else if (err.equals("1")) {
				System.out.println("USERNAME IN USE");
			} else {
				System.out.println("REGISTER FAIL");
			}
			outputStream.close();
			inputStream.close();
			sc.close();
		} catch (Exception e) {
			System.out.println("REGISTER FAIL");
		}
		return RC.ERROR;
	}

	/**
	 * @param user - User name to unregister from the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist
	 * @return ERROR if another error occurred
	 */
	static RC unregister(String user) {
		{
			try {
				Socket sc = new Socket(_server, _port);
				DataOutputStream outputStream = new DataOutputStream(sc.getOutputStream());
				// Envio de unregister
				outputStream.writeBytes("UNREGISTER");
				outputStream.write('\0');
				outputStream.flush();
				// Envio de usuario
				outputStream.writeBytes(user);
				outputStream.write('\0');
				outputStream.flush();
				DataInputStream inputStream = new DataInputStream(sc.getInputStream());
				String err = ReadString(inputStream);
				if (err.equals("0")) {
					System.out.println("UNREGISTER OK");

				} else if (err.equals("1")) {
					System.out.println("USERNAME DOES NOT EXIST");
				} else {
					System.out.println("UNREGISTER FAIL");
				}
				outputStream.close();
				inputStream.close();
				sc.close();
			} catch (Exception e) {
				System.out.println("UNREGISTER FAIL");
			}
			return RC.ERROR;
		}
	}

	/**
	 * @param user - User name to connect to the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist or if it is already connected
	 * @return ERROR if another error occurred
	 */
	static RC connect(String user) {
		// Write your code here
		connected = true;
		transmitter = user;

		try {
			Socket sc = new Socket(_server, _port);
			DataOutputStream outputStream = new DataOutputStream(sc.getOutputStream());
			// Envio de connect
			outputStream.writeBytes("CONNECT");
			outputStream.write('\0');
			outputStream.flush();
			// Envio de usuario
			outputStream.writeBytes(user);
			outputStream.write('\0');
			outputStream.flush();
			// Creo un socket servidor nuevo para el hilo correspondiente
			soc = new ServerSocket(0);
			outputStream.writeBytes(Integer.toString(soc.getLocalPort()));
			outputStream.write('\0');
			outputStream.flush();
			// Envio el puerto de escucha
			Thread th = new Thread() {
				public void run() {
					try {

						String op, user_sender, mid, message, ack;
						while (connected == true) {
							System.out.print("c> ");
							Socket socket_escucha = soc.accept();
							DataInputStream inputStream = new DataInputStream(socket_escucha.getInputStream());

							op = ReadString(inputStream);
							if (op.equals("SEND_MESSAGE")) {
								user_sender = ReadString(inputStream);
								mid = ReadString(inputStream);
								message = ReadString(inputStream);
								System.out.println("MESSAGE " + mid + " FROM " + user_sender + ":\n" + message + " END");
							} else if (op.equals("SEND_MESS_ACK")) {
								ack = ReadString(inputStream);
								System.out.println("ACK : " + ack);
							}

							socket_escucha.close();
						}
					} catch (Exception e) {
						System.out.println("Excepcion" + e);
					}

				}
			};

			DataInputStream inputStream = new DataInputStream(sc.getInputStream());
			String err = ReadString(inputStream);
			if (err.equals("0")) {
				th_g = th;
				th_g.start();
				System.out.println("CONNECT OK");

			} else if (err.equals("1")) {
				System.out.println("CONNECT FAIL, USER DOES NOT EXIST");
			} else if (err.equals("2")) {
				System.out.println("USER ALREADY CONNECTED");
			} else {
				System.out.println("CONNECT FAIL");
			}
			outputStream.close();
			inputStream.close();
			sc.close();

		} catch (Exception e) {
			System.out.println("CONNECT FAIL");

		}
		return RC.ERROR;
	}

	/**
	 * @param user - User name to disconnect from the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist
	 * @return ERROR if another error occurred
	 */
	// FALTA PARAR EL HILO CREADO EN CONNECT
	static RC disconnect(String user) {
		connected = false;
		try {
			Socket sc = new Socket(_server, _port);
			DataOutputStream outputStream = new DataOutputStream(sc.getOutputStream());
			// Envio de connect
			outputStream.writeBytes("DISCONNECT");
			outputStream.write('\0');
			outputStream.flush();
			// Envio de usuario que recibe el mensaje
			outputStream.writeBytes(user);
			outputStream.write('\0');
			outputStream.flush();

			DataInputStream inputStream = new DataInputStream(sc.getInputStream());
			String err = ReadString(inputStream);
			if (err.equals("0")) {
				th_g.interrupt();
				System.out.println("DISCONNECT OK");

			} else if (err.equals("1")) {
				System.out.println("DISCONNECT FAIL / USER DOES NOT EXIST");
			} else if (err.equals("2")) {
				System.out.println("DISCONNECT FAIL / USER NOT CONNECTED");
			} else {
				System.out.println("DISCONNECT FAIL");
			}
			outputStream.close();
			inputStream.close();
			sc.close();
		} catch (Exception e) {
			System.out.println("DISCONNECT FAIL");
		}
		return RC.ERROR;
	}

	/**
	 * @param user    - Receiver user name
	 * @param message - Message to be sent
	 * 
	 * @return OK if the server had successfully delivered the message
	 * @return USER_ERROR if the user is not connected (the message is queued for
	 *         delivery)
	 * @return ERROR the user does not exist or another error occurred
	 */
	static RC send(String user, String message) {
		try {
			Socket sc = new Socket(_server, _port);
			DataOutputStream outputStream = new DataOutputStream(sc.getOutputStream());
			// Envio de connect
			outputStream.writeBytes("SEND");
			outputStream.write('\0');
			outputStream.flush();
			// Envio de usuario que envia el mensaje
			outputStream.writeBytes(transmitter);
			outputStream.write('\0');
			outputStream.flush();
			// Envio de usuario que recibe el mensaje
			outputStream.writeBytes(user);
			outputStream.write('\0');
			outputStream.flush();
			// Envio del mensaje
			URL url = new URL("http://localhost:8888/rs?wsdl");
			ServicioServiceService service = new ServicioServiceService(url);
			ServicioService port = service.getServicioServicePort();
			message = port.capitalize(message);
			if (message.length() > 255) {
				message.substring(0, 255);
			}
			outputStream.writeBytes(message);
			outputStream.write('\0');
			outputStream.flush();

			DataInputStream inputStream = new DataInputStream(sc.getInputStream());
			String err = ReadString(inputStream);
			if (err.equals("0")) {
				String id = ReadString(inputStream);
				System.out.println("SEND OK - MESSAGE " + id);
			} else if (err.equals("1")) {
				System.out.println("SEND FAIL / USER DOES NOT EXIST");
			} else {
				System.out.println("SEND FAIL");
			}
			outputStream.close();
			inputStream.close();
			sc.close();
		} catch (Exception e) {
			System.out.println("SEND FAIL");
		}
		return RC.ERROR;
	}


	/**
	 * 
	 * /**
	 * 
	 * @brief Command interpreter for the client. It calls the protocol functions.
	 */
	static void shell() {
		boolean exit = false;
		String input;
		String[] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

		while (!exit) {
			try {
				System.out.print("c> ");
				input = in.readLine();
				line = input.split("\\s");

				if (line.length > 0) {
					/*********** REGISTER *************/
					if (line[0].equals("REGISTER")) {
						if (line.length == 2) {
							register(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: REGISTER <userName>");
						}
					}

					/********** UNREGISTER ************/
					else if (line[0].equals("UNREGISTER")) {
						if (line.length == 2) {
							unregister(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: UNREGISTER <userName>");
						}
					}

					/************ CONNECT *************/
					else if (line[0].equals("CONNECT")) {
						if (line.length == 2) {
							connect(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: CONNECT <userName>");
						}
					}

					/********** DISCONNECT ************/
					else if (line[0].equals("DISCONNECT")) {
						if (line.length == 2) {
							disconnect(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: DISCONNECT <userName>");
						}
					}

					/************** SEND **************/
					else if (line[0].equals("SEND")) {
						if (line.length >= 3) {
							// Remove first two words
							String message1 = input.substring(input.indexOf(' ') + 1);
							String message = message1.substring(message1.indexOf(' ') + 1);
							send(line[1], message); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: SEND <userName> <message>");
						}
					}


					/************** QUIT **************/
					else if (line[0].equals("QUIT")) {
						if (line.length == 1) {
							exit = true;
						} else {
							System.out.println("Syntax error. Use: QUIT");
						}
					}

					/************* UNKNOWN ************/
					else {
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}

	/**
	 * @brief Prints program usage
	 */
	static void usage() {
		System.out.println("Usage: java -cp . client -s <server> -p <port>");
	}

	/**
	 * @brief Parses program execution arguments
	 */
	static boolean parseArguments(String[] argv) {
		Getopt g = new Getopt("client", argv, "ds:p:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch (c) {
				// case 'd':
				// _debug = true;
				// break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Integer.parseInt(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}

		if (_server == null)
			return false;

		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		return true;
	}

	/********************* MAIN **********************/

	public static void main(String[] argv) {
		if (!parseArguments(argv)) {
			usage();
			return;
		}
		shell();
		System.out.println("+++ FINISHED +++");
	}
}
