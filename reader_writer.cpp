#include<iostream>
#include<fstream>
#include<sstream>
#include<cstring>
#include<pthread.h>
#include<semaphore.h>

using namespace std;

struct Writer {
    
    int id;
    int executionTimes;
    sem_t *rw;
    sem_t *screen;
    string path;

    Writer(int id, int executionTimes, sem_t *rw, sem_t *screen, string path) {

        this->id = id;
        this->executionTimes = executionTimes;
        this->rw = rw;
        this->screen = screen;
        this->path = path;

    } 

};
struct Reader {
    
    int id;
    int executionTimes;
    sem_t *rw;
    sem_t *mutex;
    sem_t *screen;
    int *readerNum;
    string path;

    Reader(int id, int executionTimes, sem_t *rw, sem_t *mutex, sem_t *screen, int *readerNum, string path) {

        this->id = id;
        this->executionTimes = executionTimes;
        this->rw = rw;
        this->mutex = mutex;
        this->screen = screen;
        this->readerNum = readerNum;
        this->path = path;

    }

};
struct Screen {

    string message;
    sem_t *semaphore;

    Screen(string message, sem_t *semaphore) {

        this->message = message;
        this->semaphore = semaphore;

    }

};

string randomString();
void *write(void *args);
void *read(void *args);
void *print(void *args);

int main(int argc, char const *argv[]) {
    
    if(argc < 5) {

        cout << "Not enough arguments" << endl;
        return -1;

    }
    for (size_t i = 1; i != 5; i++) {
        
        for (int j = 0; j != strlen(argv[i]); j++) {

            if(!isdigit(argv[i][j])) {

                cout << "Bad arguments" << endl;
                return -1;

            }

        }
        
    }
    
    const int WRITER_DIM = atoi(argv[1]);
    const int READER_DIM = atoi(argv[2]);
    pthread_t writers[WRITER_DIM];
    pthread_t readers[READER_DIM];
    sem_t *rw = new sem_t;
    sem_t *mutex = new sem_t;
    sem_t *screen = new sem_t;
    int *readerNum = new int(0);
    string path = "file.txt";

    cout << "Program to resolve the readers and writers problem" << endl;
    srand(time(NULL));
    sem_init(rw, 0, 1);
    sem_init(mutex, 0, 1);
    sem_init(screen, 0, 1);
    ofstream(path).close();
    for (size_t i = 0; i != WRITER_DIM; i++) {
        
        Writer *writer = new Writer(i, atoi(argv[3]), rw, screen, path);

        pthread_create(&writers[i], NULL, write, writer);

    }
    for(size_t i = 0; i != READER_DIM; i++) {
        
        Reader *reader = new Reader(i, atoi(argv[4]), rw, mutex, screen, readerNum, path);

        pthread_create(&readers[i], NULL, read, reader);

    }
    for (size_t i = 0; i != WRITER_DIM; i++) {
        
        pthread_join(writers[i], NULL);

    }
    for(size_t i = 0; i != READER_DIM; i++) {

        pthread_join(readers[i], NULL);

    }
    sem_destroy(rw);
    sem_destroy(mutex);
    delete rw;
    delete mutex;
    delete screen;
    delete readerNum;
    return 0;

}

string randomString() {

    int l = (rand() % 10) + 1;
    string s = "";

    for (size_t i = 0; i != l; i++) {
        
        s += 'a' + rand() % 26;

    }
    return s;

}
void *write(void *args) {
    
    Writer *writer = (Writer *)args;
    pthread_t end;

    for(size_t i = 0; i != writer->executionTimes; i++)  {

        string s = randomString();
        ofstream out(writer->path, ofstream::app);
        pthread_t thread;
        Screen *screen;

        sem_wait(writer->rw);
        out << s << endl;
        out.close();
        screen = new Screen("Writer " + to_string(writer->id) + " has written:\t" + s, writer->screen);
        sem_post(writer->rw);
        pthread_create(&thread, NULL, print, screen);
        pthread_join(thread, NULL);
        
    }
    pthread_create(&end, NULL, print, new Screen("Writer " + to_string(writer->id) + " has ended", writer->screen));
    pthread_join(end, NULL);
    delete writer;
    return NULL;
   

}
void *read(void *args) {
    
    Reader *reader = (Reader *)args;
    pthread_t end;
    
    for(size_t i = 0; i != reader->executionTimes; i++) {

        string s;
        ifstream in(reader->path);
        ostringstream stream;
        pthread_t thread;
        Screen *screen;
        
        sem_wait(reader->mutex);
        (*reader->readerNum)++;
        if(*reader->readerNum == 1) {

            sem_wait(reader->rw);

        }
        sem_post(reader->mutex);
        stream << in.rdbuf();
        s = stream.str();
        in.close();
        screen = new Screen("Reader " + to_string(reader->id) + " has read:\t" + to_string(s.size()) + " characters", reader->screen);
        sem_wait(reader->mutex);
        (*reader->readerNum)--;
        if(*reader->readerNum == 0) {

            sem_post(reader->rw);

        }
        sem_post(reader->mutex);
        pthread_create(&thread, NULL, print, screen);
        pthread_join(thread, NULL);
        
    }
    pthread_create(&end, NULL, print, new Screen("Reader " + to_string(reader->id) + " has ended", reader->screen));
    pthread_join(end, NULL);
    delete reader;
    return NULL;

}
void *print(void *args) {

    Screen *screen = (Screen *)args;

    sem_wait(screen->semaphore);
    cout << screen->message << endl;
    sem_post(screen->semaphore);
    delete screen;
    return NULL;

}