typedef enum Flag_e {
    FLAG_NoOp,
    FLAG_Input,
    FLAG_Output,
    FLAG_Help
} Flag;

typedef struct State_t {
    char *input;
    char *output;
    enum Flag_e flag;
    FILE *in;
    FILE *out;
} State;

