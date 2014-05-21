/*-----------------------------------------------------------------------------
/ Barebones zeroMQ client suited for my generic needs.
/ May 2014 <ihsan@kehribar.me> 
/----------------------------------------------------------------------------*/
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
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
int waitUntilGoodConnection(void* sock);
void hexDump(char* desc,void* addr,int len);
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
    int i;  
    uint8_t rxBuffer[128];
    uint8_t txBuffer[128];
    
    /* Provide a clean exit ... */
    /* this seems unneccsary, since zeromq handles sigint internally? */ 
    // signal(SIGINT,sig_handler);  
        
    context = zmq_ctx_new();
    sock = zmq_socket(context,ZMQ_REQ);
    zmq_connect(sock,"tcp://localhost:5555");
    
    waitUntilGoodConnection(sock);

    for(i=1;i<128;i++)
    {
        txBuffer[i] = i-1;
    }

    while(1)
    {
        /* Sleep 1000 ms */
        zclock_sleep(1000);

        /* Loopback command */
        txBuffer[0] = 1;

        /* Send a dummy message */
        zmq_send(sock,txBuffer,128,0);    

        /* Block until we get any response */
        zmq_recv(sock,rxBuffer,128,0);

        /* Hex dump if you want */
        debugDump("Response",rxBuffer,128);
    }
    
    return 0;
}
/*---------------------------------------------------------------------------*/
int waitUntilGoodConnection(void* socket)
{
    uint8_t tx[128];
    uint8_t rx[128];

    tx[0] = 0x00;

    /* Send 'are you alive?' message ... */
    zmq_send(sock, tx, 128, 0);    

    /* Block until we get any response */
    zmq_recv(sock, rx, 128, 0);

    debugDump("Response",rx,128);

    /* Check the response is valid or not */
    if((rx[0] == 0xDE) && (rx[1] == 0xAD) && (rx[2] == 0xBE) && (rx[3] == 0xEF))
    {
        /* OK */
        dbg("Connection OK!\n");
        return 0;
    }
    else
    {
        /* Problem? */
        dbg("Connection Problem?\n");
        return -1;
    }
}
/*---------------------------------------------------------------------------*/
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