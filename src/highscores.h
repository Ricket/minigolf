#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#define HIGHSCORES_FILE "highscores.bin"

struct highscoretable;

void clear_highscores(void);
void add_highscore(struct highscoretable *, char *, char *, int);
struct highscoretable *load_highscores(void);
void save_highscores(struct highscoretable *);
void free_highscores(struct highscoretable *);
void show_highscores(void);

#endif
