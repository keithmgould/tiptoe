#define SAMPLE_RATE   (32000) // 8000 * 4
#define FRAMES_PER_BUFFER  (1280) // 320 * 4

// the size of the various frequency sinusoids
#define PREAMBLE_LOW_FREQUENCY (2400)
#define EDGE_LOW_FREQUENCY     (2600)
#define MIDDLE_LOW_FREQUENCY   (2800)
#define MIDDLE_HIGH_FREQUENCY  (3000)
#define EDGE_HIGH_FREQUENCY    (3200)

// the various frequency modes
#define PREAMBLE_LOW (1)
#define EDGE_LOW     (2)
#define MIDDLE_LOW   (3)
#define MIDDLE_HIGH  (4)
#define EDGE_HIGH    (5)

// semantics
// #define T (true)
// #define F (false)

// as determined with a nail, twine and chalk.
#ifndef M_PI
#define M_PI  (3.14159265)
#endif

// debug mode:
// 0 : show nothing
// 1 : show only data
// 2 : show data and more
#define DEBUG_MODE (0)
