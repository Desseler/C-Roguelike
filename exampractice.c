#include <stdio.h>
#include <stdlib.h>

//#1

void swap(int *a, int *b)
{
  int tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}


//#2

typedef struct{
  int lol;
  char memes;
}idot_t;

void freeplusplus(void **p){
  free(*p);
  *p = NULL;
  
}

//#3

char *strndup(const char *s, size_t n)
{
  char *r;
  int i;
  if((r = malloc(n + 1)))
    {
      for (i = 0; i < n && *s; i++, s++)
	{
	  r[i] = *s;
	}
      r[i] = '\0';
    }
  
  return r;
}

int main(int argc, char *argv[])
{
  //  int a = 3;
  //int b = 5;
  //swap(&a, &b);
  //printf("%i", a);
  //printf("%i", b);

  /*
  idot_t *f = malloc(sizeof (idot_t));
  printf("%d \n", f);
  freeplusplus(&f);
  printf("%d \n", f);
  */

  char *s = "PogChamp";
  char *k = strndup(s, 3);

  printf("%s \n", s);
  printf("%s \n", k);
  
  return 0;
}
