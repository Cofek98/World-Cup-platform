package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {

        String stPort = args[0];
        Integer port = Integer.parseInt(stPort);
        String type = args[1];

        if (type.equals("tpc")){
            Server.threadPerClient(port,
            () -> new StompMessagingProtocolImpl(),
            () -> new StompMessageEncoderDecoder()
            ).serve();
        }

        else if(type.equals("reactor")){
            Server.reactor(Runtime.getRuntime().availableProcessors(),
            port,
            () -> new StompMessagingProtocolImpl(),
            () -> new StompMessageEncoderDecoder()
            ).serve();
        }
    }
}
