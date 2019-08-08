#ifndef BASE_CONST_H
#define BASE_CONST_H

#define CLI_RED "\033[31m"
#define CLI_GRE "\033[32m"
#define CLI_BRO "\033[33m"
#define CLI_BLU "\033[34m"
#define CLI_PUR "\033[35m"
#define CLI_CYA "\033[36m"
#define CLI_WHI "\033[37m"
#define CLI_NOR "\033[0m"

#define foreach(c, itr) \
  for(__typeof((c).begin()) itr=(c).begin(); itr != (c).end(); ++itr)

#define reverse_foreach(c, itr) \
  for(__typeof((c).rbegin()) itr=(c).rbegin(); itr != (c).rend(); ++itr)

#ifdef DEBUG
#define DEBUG_MSG(str) do { \
    std::cout << CLI_BRO << str << CLI_NOR << std::endl; \
} while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif


#endif
