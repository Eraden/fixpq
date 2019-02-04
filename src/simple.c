#include <simple.h>

void open_out(State *state) {
    if (!state->output) {
        return;
    }
    state->out = fopen(state->output, "w+");
    if (state->out == NULL) {
        printf("Cannot open file to write: %s\n", state->output);
        if (state->in) fclose(state->in);
        exit(1);
    }
}

void fix_content(State *state) {
    char *buffer = (char *) malloc(2048);
    size_t len = 2048;
    FILE *tmp = tmpfile();

    while (!feof(state->in) && !ferror(state->in)) {
        size_t read_size = getline(&buffer, &len, state->in);
        // printf(" >> %s", buffer);
        if (read_size == -1)
            break;
        if (strcmp(buffer, "    AS integer") == 0) {
            printf("Found 'AS integer' in line '%s'", buffer);
        } else {
            fwrite(buffer, sizeof(*buffer), read_size, tmp);
        }
    }

    rewind(tmp);
    if (state->dry == 0) open_out(state);

    while (!feof(tmp) && !ferror(tmp)) {
        size_t read_size = getline(&buffer, &len, tmp);
        if (read_size == -1)
            break;
        if (state->dry == 0)
            fwrite(buffer, sizeof(*buffer), read_size, state->out);
    }

    fclose(tmp);
    free(buffer);
}
