int conexion_socket();

int init();

int set_value(char *key, char *value1, int value2, float value3);

int get_value(char *key, char *value1, int *value2, float *value3);

int modify_value(char *key, char *value1, int value2, float value3);

int delete_key(char *key);

int exist(char *key);

int num_items();