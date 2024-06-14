package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.MyConnections;
import bgu.spl.net.srv.User;



public class StompMessagingProtocolImpl implements StompMessagingProtocol<String>{
    
    //fields:
    private int connectionId;
    private MyConnections<String> connections;
    public Boolean shouldTerminate;
    //private boolean loggedIn;

    //methods:

    //constructor
    public StompMessagingProtocolImpl(){
        shouldTerminate = false;
        //loggedIn = false;
    }

	/**
	 * Used to initiate the current client protocol with it's personal connection ID and the connections implementation
	**/
    public void start(int conId, Connections<String> connections){
        connectionId = conId;
        this.connections = (MyConnections<String>)connections;
    }

    /**
     * process the given message 
     * @param message the received message
     */
    public void process(String message){
        String[] msgArr = message.split("\n");
        String action = msgArr[0];
        if (action.equals("CONNECT")){
            connect(msgArr, connectionId);
            return;
        }
        if (action.equals("SEND")){
            send(msgArr, connectionId);
            return;
        }
        if (action.equals("SUBSCRIBE")){
            subscribe(msgArr, connectionId);
            return;
        }
        if (action.equals("UNSUBSCRIBE")){
            unsubscribe(msgArr, connectionId);
            return;
        }
        if (action.equals("DISCONNECT")){
            disconnect(msgArr, connectionId);
            return;
        }
    }
	
	/**
     * @return true if the connection should be terminated
     */
    public boolean shouldTerminate(){
        return shouldTerminate;
    }

    private void connect(String[] msgArr, int conId){

        String userName = getHeader(msgArr, "login");
        String password = getHeader(msgArr, "passcode");
        if (userName == null || password == null) {
            String error = "ERROR" + "\n" + "message:malformed frame received" + "\n" + "\u0000";
            sendError(error);
            return;
        }
        User curr = connections.getUsersByName().get(userName);
        if (curr == null) { 
            User user = new User(conId, userName, password);
            connections.addUser(user);
            String frame = "CONNECTED" + "\n" + "version:1.2" + "\n" + "\u0000";
            sendMsg(frame);
        }
        ///check what comes first- password or logged in
        else if(curr.isConnected()){
            String frame = "ERROR" + "\n" + "message:User already logged in" + "\n" + "\u0000";
            sendError(frame);
        }
        else if(!curr.getPassword().equals(password)){
            String frame = "ERROR" + "\n" + "message:Wrong password" + "\n" + "\u0000";
            sendError(frame);
        }
        else{
            int prevConId = curr.getConnectionId();
            connections.getUsersByConId().remove(prevConId);
            curr.setConnectionId(conId);
            connections.getUsersByConId().put(conId, curr);
            curr.connect();
            String frame = "CONNECTED" + "\n" + "version:1.2" + "\n" + "\u0000";
            sendMsg(frame);
        }
        
    }

    private void send(String[] msgArr, Integer conId){
        String dest = getHeader(msgArr, "destination");

        if (dest == null){
            String error = "ERROR" + "\n" + "message:malformed frame received" + "\n" + "\u0000";
            sendError(error);
        } else if (!connections.getTopics().containsKey(dest) || !connections.getTopics().get(dest).contains(conId)){
            String error = "ERROR" + "\n" + "message:user is not subscribed to " + dest.substring(1) + "\n" + "\u0000";
            sendError(error);
        } else {

            String frameBody = "";
            for (int i = 2; i < msgArr.length; i++) {
                frameBody = frameBody + msgArr[i] + "\n";
            }
            frameBody = frameBody + "\u0000";
            int msgId = connections.getMsgId();
            for (Integer con : connections.getTopics().get(dest)){
                User user = connections.getUsersByConId().get(con);
                String subId = user.subsByTopic.get(dest).toString();
                String msg = "MESSAGE" +"\n" + "subscription:" + subId + "\n" + "message-id:" + msgId +
                 "\n" + "destination:" + dest + "\n" + frameBody;
                connections.send(con, msg);
            }
        }
    
    }

    private void subscribe(String[] msgArr, int conId){
        String dest = getHeader(msgArr, "destination");
        String sub = getHeader(msgArr, "id");
        String rec = getHeader(msgArr, "receipt");
        if (dest == null || sub == null || rec == null){
            String error = "ERROR" + "\n" + "message:malformed frame received" + "\n" + "\u0000";
            sendError(error);
            return;
        }
        Integer intSub = Integer.valueOf(sub);
        User user = connections.getUsersByConId().get(conId);
        if (!user.isSubscribedTo(dest)){
            connections.subscribe(dest, conId);
            user.subscribe(dest, intSub);
            String frame = "RECEIPT" + "\n" + "receipt-id:" + rec + "\n" + "\u0000";
            sendMsg(frame);
    
        }
        else {
            String frame = "ERROR" + "\n" + "message:User already subscribed to this topic" + "\n" + "\u0000";
            sendError(frame);
        }
    }

    private void unsubscribe(String[] msgArr, int conId){
        String sub = getHeader(msgArr, "id");
        String rec = getHeader(msgArr, "receipt");

        if (sub == null || rec == null){
            String error = "ERROR" + "\n" + "message:malformed frame received" + "\n" + "\u0000";
            sendError(error);
            return;
        }
        Integer intSub = Integer.valueOf(sub);
        User user = connections.getUsersByConId().get(conId);
        String topic = user.unsubscribe(intSub);
        if (topic != null){
            connections.unsubscribe(topic, conId);
            String frame = "RECEIPT" + "\n" + "receipt-id:" + rec + "\n" + "\u0000";
            sendMsg(frame);
        }
        else {
            String frame = "ERROR" + "\n" + "message:User is not subscribed to this topic" + "\n" + "\u0000";
            sendError(frame);
        }
    
    }

    private void disconnect(String[] msgArr, int conId){
        synchronized(connections.getConHands().get(conId)){
        String receipt = getHeader(msgArr, "receipt");
        //Integer intRec = Integer.valueOf(receipt);
        if (receipt== null){
            String error = "ERROR" + "\n" + "message:malformed frame received" + "\n" + "\u0000";
            sendError(error);
            return;
        }
        User user = connections.getUsersByConId().get(conId);
        String frame = "RECEIPT" + "\n" + "receipt-id:" + receipt + "\n" + "\u0000";
        sendMsg(frame);
        user.disconnect();
        connections.disconnect(conId);
        shouldTerminate = true;
        }
    }

    public void close(){
        User user = connections.getUsersByConId().get(connectionId);
        if (user!=null)
            user.disconnect();
        connections.disconnect(connectionId);
        shouldTerminate = true;
    }


    private String getHeader(String[] msgArr, String header){
        for (String s : msgArr){
            String[] curr = s.split(":");
            if (curr[0].equals(header))
                return curr[1];
        }
        return null;
    }

    private void sendError(String msg){
        sendMsg(msg);
        User user = connections.getUsersByConId().get(connectionId);
        if (user != null){
            user.disconnect();
            connections.disconnect(connectionId);
        }
        shouldTerminate = true;
    }

    private void sendMsg(String msg){
        connections.send(connectionId, msg);
    }
    
}
