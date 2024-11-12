#include <stdlib.h>
#include <sys/time.h> /* for gettimeofday system call */
#include <pthread.h>
#include "lab.h"
#define MIN(x, y) ((x) < (y) ? (x) : (y))
/**
 * @brief Standard insertion sort that is faster than merge sort for small array's
 *
 * @param A The array to sort
 * @param p The starting index
 * @param r The ending index
 */
static void insertion_sort(int A[], int p, int r)
{
  int j;

  for (j = p + 1; j <= r; j++)
  {
    int key = A[j];
    int i = j - 1;
    while ((i > p - 1) && (A[i] > key))
    {
      A[i + 1] = A[i];
      i--;
    }
    A[i + 1] = key;
  }
}

/**
 * @brief The function that is called by each thread to sort their chunk
 *
 * @param args see struct parallel_args
 * @return void* always NULL
 */
void *parallel_mergesort(void *args)
{
  struct parallel_args *pargs = (struct parallel_args *)args;
  mergesort_s(pargs->A, pargs->start, pargs->end);
  return NULL;
}

/**
 * @brief Sorts an array of ints into ascending order using multiple
 * threads
 *
 * @param A A pointer to the start of the array
 * @param n The size of the array
 * @param num_threads The number of threads to use.
 */
void mergesort_mt(int *A, int n, int num_thread)
{
  struct parallel_args *args = (struct parallel_args *)malloc(sizeof(struct parallel_args) * num_thread);
  int chunk_size = n / num_thread;
  pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * num_thread);

  for (int i = 0; i < num_thread; i++)
  {
    pthread_mutex_lock(&args[i].lock);
    args[i].A = A;
    args[i].start = i * chunk_size;
    args[i].end = (i == num_thread - 1) ? n - 1 : (i + 1) * chunk_size - 1;
    pthread_create(&threads[i], NULL, parallel_mergesort, (void *)&args[i]);
  }
  for (int i = 0; i < num_thread; i++)
  {
    pthread_join(threads[i], NULL);
  }
  int step_size = chunk_size;
  while (step_size < n)
  {
    for (int i = 0; i < n; i += 2 * step_size)
    {
      int mid = i + step_size - 1;
      int end = MIN(i + 2 * step_size - 1, n - 1);
      if (mid < end)
      {
        merge_s(A, i, mid, end);
      }
    }
    step_size *= 2;
  }

  free(args);
  free(threads);
}

/**
 * @brief Sorts an array of ints into ascending order using the constant
 * INSERTION_SORT_THRESHOLD internally
 *
 * @param A A pointer to the start of the array
 * @param p The starting index
 * @param r The ending index
 */
void mergesort_s(int A[], int p, int r)
{
  if (r - p + 1 <= INSERTION_SORT_THRESHOLD)
  {
    insertion_sort(A, p, r);
  }
  else
  {
    int q = (p + r) / 2;
    mergesort_s(A, p, q);
    mergesort_s(A, q + 1, r);
    merge_s(A, p, q, r);
  }
}

/**
 * @brief Merge two sorted sequences A[p..q] and A[q+1..r] and place merged
 *              output back in array A. Uses extra space proportional to
 *              A[p..r].
 *
 * @param A The array to merge into
 * @param p The starting index of the first half
 * @param q The middle
 * @param r The ending index of the second half
 */
void merge_s(int A[], int p, int q, int r)
{
  int *B = (int *)malloc(sizeof(int) * (r - p + 1));

  int i = p;
  int j = q + 1;
  int k = 0;
  int l;

  /* as long as both lists have unexamined elements */
  /*  this loop keeps executing. */
  while ((i <= q) && (j <= r))
  {
    if (A[i] < A[j])
    {
      B[k] = A[i];
      i++;
    }
    else
    {
      B[k] = A[j];
      j++;
    }
    k++;
  }

  /* now only at most one list has unprocessed elements. */
  if (i <= q)
  {
    /* copy remaining elements from the first list */
    for (l = i; l <= q; l++)
    {
      B[k] = A[l];
      k++;
    }
  }
  else
  {
    /* copy remaining elements from the second list */
    for (l = j; l <= r; l++)
    {
      B[k] = A[l];
      k++;
    }
  }

  /* copy merged output from array B back to array A */
  k = 0;
  for (l = p; l <= r; l++)
  {
    A[l] = B[k];
    k++;
  }

  free(B);
}

double getMilliSeconds()
{
  struct timeval now;
  gettimeofday(&now, (struct timezone *)0);
  return (double)now.tv_sec * 1000.0 + now.tv_usec / 1000.0;
}