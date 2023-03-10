#pragma once

//High-level (easy) picture loading function:
void kpzload(const char *, intptr_t *, int *, int *, int *);
//Low-level PNG/JPG functions:
void kpgetdim(void*, int, int *, int *);
int kprender(void*, int, void *, int, int, int, int, int);

//ZIP functions:
int kzaddstack(const char *);
void kzuninit();
int kzOpen(const char *);
int kzRead(void*, int);
int kzfilelength();
int kzseek(int, int);
int kztell();
int kzgetc();
int kzeof();
void kzclose();

void kzfindfilestart(const char *); //pass wildcard string
int kzfindfile(char *); //you alloc buf, returns 1:found,0:~found