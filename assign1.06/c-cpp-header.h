#ifndef C_CPP_HEADER_H
# define C_CPP_HEADER_H

#ifndef __cplusplus
typedef void ostream;
#endif

#ifdef __cplusplus
using namespace std;

extern "C" {
#endif

void cpp_function(const char *s);
ostream *return_cout(void);
void use_cout(ostream *o);

#ifdef __cplusplus
}
#endif

#endif

