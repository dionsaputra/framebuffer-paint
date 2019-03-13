#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// thread
#include <iostream>
#include <cstdlib>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 1

void *keyListener(void *threadid){
    // thread identity
    long tid;
    tid = (long)threadid;

    // Setup input mode
    struct termios oldSettings, newSettings;

    tcgetattr( fileno( stdin ), &oldSettings );
    newSettings = oldSettings; 
    newSettings.c_lflag &= (~ICANON & ~ECHO); // one key mode

    fd_set set;
    struct timeval tv;

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    FD_ZERO( &set );
    FD_SET( fileno( stdin ), &set );

    int res = select( fileno( stdin )+1, &set, NULL, NULL, &tv );

    // tcsetattr( fileno( stdin ), TCSANOW, &newSettings ); // change to one key mode

    // Receive command
    string inputCommand;
    while(1){
        cout << "$";
        cin >> inputCommand;
        // switch
        switch(inputCommand){
            case "select":
                cout << "----list----" << endl;
                for (auto itr = wireframes.begin(); itr!=wireframes.end();itr++){
                    if (currentWireframe == itr->first){
                        cout << itr->first << " Selected" << endl;    
                    }else{
                        cout << itr->first << endl;
                    }
                } 
                cout << "------------" << endl;
                cin >> currentWireframe;
                cout << currentWireframe <<" Selected" << endl;
            case "save":
                cout << "filename: ";
                cin >> filename;
                parser.save(wireframes,filename);
                cout << "saved" << endl;
            case "current":
                cout << currentWireframe << endl;
            case "scroll":
                tcsetattr( fileno( stdin ), TCSANOW, &newSettings );
                while ( 1 ){
                    if( res > 0 ){
                        char c;
                        read( fileno( stdin ), &c, 1 );
                        printf( "Input available %c %d\n",c,c);
                        if(c=='i'){
                            // Change settings
                            tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );    
                            break;
                        }
                    }
                    else if( res < 0 )
                    {
                        perror( "select error" );
                        break;
                    }
                    else
                    {
                        printf( "Select timeout\n" );
                    }
                }
            default:
                cout << "Command tidak ditemukan"
        }
    }


    

    tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );
    return 0;
}

// int main () {
//    pthread_t threads[NUM_THREADS];
//    int rc;
 
//     rc = pthread_create(&threads[0], NULL, keyListener, (void *)0);
      
//     if (rc) {
//         cout << "Error:unable to create thread," << rc << endl;
//         exit(-1);
//     }

//     while(1){

//     }
   
//    pthread_exit(NULL);
// }