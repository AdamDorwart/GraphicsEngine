// Adapted from Microsoft GSL library
// https://github.com/Microsoft/GSL

#pragma once
#include "Logger.h"

#if !(defined(THROW_CONTRACT) ^ defined(TERMINATE_CONTRACT) ^ defined(UNENFORCED_CONTRACT))
#define UNENFORCED_CONTRACT 
#endif

#if defined(THROW_CONTRACT)

#define CONTRACT_HASH_DETAIL(x) #x
#define CONTRACT_HASH(x) CONTRACT_HASH_DETAIL(x)

#include <stdexcept>

namespace gsl {
	struct fail_fast : public std::runtime_error 
	{
		explicit fail_fast(char const* const message) : std::runtime_error(message) {}
	};
}

#define Expects(cond) if (!(cond)) \
    throw gsl::fail_fast("Contract: Precondition failure at " __FILE__ ": " CONTRACT_HASH(__LINE__));

#define ExpectsMsg(cond, msg, ...) if (!(cond)) {\
	LogError(msg, ##__VA_ARGS__); \
    throw gsl::fail_fast("Contract: Precondition failure at " __FILE__ ": " CONTRACT_HASH(__LINE__));\
}

#define Ensures(cond)  if (!(cond)) \
	throw gsl::fail_fast("Contract: Postcondition failure at " __FILE__ ": " CONTRACT_HASH(__LINE__));
#define EnsuresMsg(cond, msg, ...)  if (!(cond)) {\
	LogError(msg, ##__VA_ARGS__); \
	throw gsl::fail_fast("Contract: Postcondition failure at " __FILE__ ": " CONTRACT_HASH(__LINE__)); \
}
#elif defined(TERMINATE_CONTRACT)

#define Expects(cond) if (!(cond)) \
	std::terminate();
#define ExpectsMsg(cond, msg, ...) if (!(cond)) {\
	LogError(msg, ##__VA_ARGS__); \
	std::terminate(); \
}

#define Ensures(cond) if (!(cond)) \
	std::terminate();
#define EnsuresMsg(cond, msg, ...) if (!(cond)) {\
	LogError(msg, ##__VA_ARGS__); \
	std::terminate(); \
}

#elif defined(UNENFORCED_CONTRACT)

#define Expects(cond)
#define ExpectsMsg(cond, msg, ...)          
#define Ensures(cond)
#define EnsuresMsg(cond, msg, ...)         

#endif 