#ifndef _ASI_VECTOR_H_
#define _ASI_VECTOR_H_

typedef struct vector
{
    double *data;
    int n;
} vector_type;

/* Create new vector */
int vector_new(vector_type *v, int n);

/* Delete vector */
void vector_delete(vector_type *v);

/* Initialisation functions: fill vector with values */
int vector_init_zero(vector_type v);
int vector_init_value(vector_type v, double value);

#endif
