struct messageNode {
    char message[256];
    char sender[256];
    unsigned int mid;
    struct messageNode *next;
};
typedef struct messageNode *messageList;

struct userNode {
    char username[256];
    int status, port;
    unsigned int mid;
    char IP[16];
    struct messageNode *l_message;
    struct userNode *next;
};
typedef struct userNode *userList;

struct registerNode {
    char username[256];
    struct registerNode *next;
};
typedef struct registerNode *registerList;




int add_username(userList *l, char *username, int status, char *IP, int port, int mid);

int add_register(registerList *l, char *username);

int add_messsage(userList *l, char *receptor, char* sender, char *message); 

int set_value(userList *l, char *username, int status, char *IP, int port, int mid);

int get_value(userList l, char *username, int *status, char *IP, int *port, int *mid);

int modify_value(userList *l, char *username, int status, char *IP, int port);

int printList(userList l);

void printMessage(messageList l);

int existRegister(registerList l, char *username);

int exist(userList l, char *username);

int delete_username(userList *l, char *username);

int delete_register(registerList *l, char *username);

int delete_message(userList *l, char *username, int mid);

struct userNode *get_user(userList *l, char* username);

struct messageNode *get_message(messageList l);

int num_messages(messageList l, char *username);\

void printListRegister(registerList l);