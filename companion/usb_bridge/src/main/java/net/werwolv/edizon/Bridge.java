package net.werwolv.edizon;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;

public class Bridge {

    private static boolean dataPortConnected = false, cmdPortConnected = false;

    public void startDataSocket(int port) throws IOException {
        while (true) {
            
            ServerSocket serverSocket = new ServerSocket(port);
            Socket clientSocket = serverSocket.accept();

            
            DataInputStream in = new DataInputStream(clientSocket.getInputStream());
            DataOutputStream out = new DataOutputStream(clientSocket.getOutputStream());
            
            byte[] data = null;
            
            dataPortConnected = true;
            System.out.println("New Data connection!");

            try {
                while (cmdPortConnected) {
                    try {
                        data = in.readNBytes(4);
                        data = in.readNBytes(ByteBuffer.wrap(data).getInt());
                    } catch (IOException e) {}
                    
                    out.write(usbSendReceive(data));
                }
            } catch (Exception e) { }

            System.out.println("Data connection closed!");
            dataPortConnected = false;

            clientSocket.close(); 
            serverSocket.close();
        }
    }

    public void startCmdSocket(int port) throws IOException {
        while (true) {
            
            ServerSocket serverSocket = new ServerSocket(port);
            Socket clientSocket = serverSocket.accept();

            
            DataInputStream in = new DataInputStream(clientSocket.getInputStream());
            DataOutputStream out = new DataOutputStream(clientSocket.getOutputStream());
            
            byte[] data = null;
            
            cmdPortConnected = true;
            System.out.println("New cmd connection!");

            try {
                while (dataPortConnected) {
                    try {
                        data = in.readNBytes(4);
                        data = in.readNBytes(ByteBuffer.wrap(data).getInt());
                    } catch (IOException e) {}
                    
                    out.write(usbSendReceive(data));
                }
            } catch (Exception e) { }

            System.out.println("Cmd connection closed!");
            cmdPortConnected = false;

            clientSocket.close(); 
            serverSocket.close();
        }
    }

    public byte[] usbSendReceive(byte[] data) {
        System.out.println("PC -> SWITCH : " + new String(data));
        System.out.println("PC <- SWITCH : " + new String(data));
        return data;
    }

}