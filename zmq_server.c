/*-----------------------------------------------------------------------------
/ Barebones zeroMQ server suited for my generic needs.
/ May 2014 <ihsan@kehribar.me> 
/----------------------------------------------------------------------------*/
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
/*---------------------------------------------------------------------------*/
#if 0
#define debugDump(...)
#else
#define debugDump(...) hexDump(__VA_ARGS__)
#endif
/*---------------------------------------------------------------------------*/
#if 0
#define dbg(...)
#else
#define dbg(...) printf(__VA_ARGS__)
#endif
/*---------------------------------------------------------------------------*/
void* sock;
void* context;
/*---------------------------------------------------------------------------*/
void hexDump(char* desc,void* addr,int len);
void handleMessage(uint8_t* rx, uint8_t* tx);
/*---------------------------------------------------------------------------*/
void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        dbg("\n");
        dbg("Goodbye!\n");

        zmq_close(sock);    
        zmq_ctx_destroy(context);
        exit(1);
    }    
}
/*---------------------------------------------------------------------------*/
int main(void)
{    
    uint8_t rxBuffer[128];
    uint8_t txBuffer[128];

    /* Provide a clean exit ... */
    signal(SIGINT,sig_handler);  
    
    /* Create the socket and bind ... */
    context = zmq_ctx_new();
    sock = zmq_socket(context, ZMQ_REP);
    zmq_bind(sock,"tcp://*:5555");

    dbg("Server is running ...\n");

    while(1) 
    {        
        /* Wait until new message gets. This method is blocking. */
        zmq_recv(sock,rxBuffer,128,0);       
        
        /* Put a hex dump of the message if you want */
        debugDump("Request",rxBuffer,128);
    
        /* Take some action and fill the response message buffer */
        handleMessage(rxBuffer,txBuffer);

        /* Don't forget to respond! */        
        zmq_send(sock,txBuffer,128,0);
    }

    return 0;
}
/*---------------------------------------------------------------------------*/
void handleMessage(uint8_t* rx, uint8_t* tx)
{
    /* Parse incoming message and respond & act accordingly */
    switch(rx[0])
    {
        /* Are you there? */
        case 0:
        {
            tx[0] = 0xDE;
            tx[1] = 0xAD;
            tx[2] = 0xBE;
            tx[3] = 0xEF;
            break;
        }
        /* Loopback */
        case 1:
        {
            int i;
            for(i=0;i<128;i++)
            {
                tx[i] = rx[i];
            }
            break;
        }
    }
}
/*---------------------------------------------------------------------------*/
/* taken from: http://stackoverflow.com/a/7776146 */
void hexDump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}
/*---------------------------------------------------------------------------*/