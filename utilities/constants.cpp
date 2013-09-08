// the size of the various frequency sinusoids
#define PREAMBLE_LOW_TABLE_SIZE (14)
#define EDGE_LOW_TABLE_SIZE     (13)
#define MIDDLE_LOW_TABLE_SIZE   (12)
#define MIDDLE_HIGH_TABLE_SIZE  (11)
#define EDGE_HIGH_TABLE_SIZE    (10)

// the various frequency modes
#define PREAMBLE_LOW (1)
#define EDGE_LOW     (2)
#define MIDDLE_LOW   (3)
#define MIDDLE_HIGH  (4)
#define EDGE_HIGH    (5)

// semantics
#define T (true)
#define F (false)

// as determined with a nail, twine and chalk.
#ifndef M_PI
#define M_PI  (3.14159265)
#endif