
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Storage type
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef struct vertex {
  unsigned int idx;
  unsigned int version;
} vertex;


typedef struct cpq_t {
  double *priority;
  vertex **vert;
  uint32_t size;
  uint32_t len;
} cpq_t;


void cpq_free(cpq_t *pq) ;
cpq_t * cpq_create();
void cpq_insert(cpq_t *pq, vertex * obj, double priority);
vertex * cpq_pop(cpq_t *pq);
vertex * cpq_peek(cpq_t *pq);
