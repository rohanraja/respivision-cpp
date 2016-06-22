//#define trace(x)
#define trace(x)                 cerr << #x << ": " << x << endl;

// #define trace(x)                 printf("%s: %x\n", #x, x);

#include <time.h>

#define readf(f,v) f.read((char *)&v, sizeof(v))
#define readfs(f,v,s) f.read((char *)v, s)

#define rep(i,n) for(int i=0; i<n; i++)

#define writef(f,v) f.write((char *)&v, sizeof(v))
#define rep(i,n) for(int i=0; i<n; i++)

typedef int size_data;

#define ARGVEC vector<cgtTuple*>


#define SPEED(data)   \
  for (long blockTime = NULL; \
      (blockTime == NULL ? (blockTime = clock()) != NULL : false); \
      printf("\nTime for %s: %.9f ms", data,  (double) (clock() - blockTime) / CLOCKS_PER_SEC)\
      )



#define TIMECH(data)   \
  for (std::chrono::steady_clock::time_point start = std::chrono::steady_clock::time_point::min() ;  \
      (start == std::chrono::steady_clock::time_point::min() ? (start = std::chrono::steady_clock::now() ) != std::chrono::steady_clock::time_point::min()  : false); \
      printf("\nTime for %s: %.3f ms\n", data,   \
      std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count() ) \
      )
