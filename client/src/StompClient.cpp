#include <ConnectionHandler.h>
#include <StompProtocol.h>
#include <Kthread.h>
#include <thread>



int main(int argc, char *argv[]) {

	ConnectionHandler* hand = new ConnectionHandler();
	StompProtocol* protocol = new StompProtocol(*hand);
	Kthread k(*hand, *protocol);
	std::thread tk(&Kthread::run, &k);
	while(1){
		if (hand->isConnected()){
			string frame;
			if (!hand->getFrameAscii(frame, '\0')){
				std::cout << "Disconnected. Exiting...\n" << std::endl;
            	break;
        	}
			protocol->processFrame(frame);

		}
	}
	tk.join();
	delete(hand);
	delete(protocol);
	
	return 0;
}