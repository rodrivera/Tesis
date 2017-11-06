
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

typedef long int i64;
typedef unsigned long int u64;

typedef dword pb;
#define logD 5
//typedef u64 pb;
//#define logD 6
#define D (1<<logD)


//typedef byte sb_type;
typedef word sb_type;

#ifdef INDEX64
  typedef i64 mb_type;
#else
  typedef dword mb_type;
#endif
