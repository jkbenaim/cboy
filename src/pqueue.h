#ifndef _PQUEUE_H_
#define _PQUEUE_H_

typedef struct pqueue_node_s {
	struct pqueue_node_s *next;
	int priority;
	int message;
} pqueue_node_s;
typedef pqueue_node_s pqueue_node;

typedef struct pqueue_root_s {
	struct pqueue_node_s *first;
} pqueue_root_s;
typedef pqueue_root_s pqueue_root;

int pqueue_init(pqueue_root * root);
int pqueue_isEmpty(pqueue_root * root);
int pqueue_peek(pqueue_root * root);
int pqueue_pop(pqueue_root * root);
int pqueue_hasNext(pqueue_root * root);
void pqueue_pass(pqueue_root * root, int time);
int pqueue_insert(pqueue_root * root, int time, int message);

#endif				// _PQUEUE_H_
