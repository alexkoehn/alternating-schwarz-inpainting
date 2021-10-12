#ifndef _ASI_SPARSE_H_
#define _ASI_SPARSE_H_

/* Sparse matrix in compressed diagonal storage (CDS) format */
typedef struct cds_sparse_mat
{
    double **a; // System matrix A
    int *ioff; // Array indicating non-zero diagonals
    int n; // Length of main diagonal of A
    int bandwidth; // Bandwidth of A
} cds_sparse_mat_type;

/* Initialise diffusion matrix given image dimensions */
void cds_init_diffusion_matrix(struct cds_sparse_mat mat, int image_width, 
    int image_height);

/* Sparse matrix vector product */
void cds_matrix_vector_product(const struct cds_sparse_mat mat, const double *x, 
    double *b);

/* Conjuage gradient method */
int cds_conjugate_gradient(const struct cds_sparse_mat mat, const double *b, 
    double *b, int iter, double eps); 

#endif
