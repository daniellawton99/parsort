
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

int read_src_file(char *file_path, unsigned long long **num_array, int *array_size)
{
    FILE *fp;
    char *temp_line = NULL;
    ssize_t get_line_result;
    size_t line_len = 0;
    int line_cnt = 0;
    // Get number of lines
    fp = fopen(file_path, "r");
    if (fp == NULL){
        printf("Error in openning file");
        exit(1);
    }
    while ((get_line_result = getline(&temp_line, &line_len, fp)) != -1)
    {
        line_cnt += 1;
    }
    fclose(fp);
    // Read numbers into array
    fp = fopen(file_path, "r");
    unsigned long long *num_array_temp = (unsigned long long*)malloc(line_cnt*sizeof(unsigned long long));
    line_cnt = 0;
    while ((get_line_result = getline(&temp_line, &line_len, fp)) != -1)
    {
        sscanf( temp_line, "%llu", &num_array_temp[line_cnt] );
        line_cnt += 1;
    }
    fclose(fp);
    *num_array = num_array_temp;
    *array_size = line_cnt;
    if (temp_line)
        free(temp_line);
    return 0;
}

void print_output(unsigned long long *num_array, int array_size)
{
    // FILE *fp;
    int i;
    // fp = fopen("sorted.txt", "w");
    for(i=0; i<array_size; i++)
    {
        // fprintf(fp, "%llu\n", num_array[i]);
        printf("%llu\n", num_array[i]);
    }
    //fclose(fp);
}

void merge(unsigned long long num_array[], int left_idx, int middle_idx, int right_idx)
{
    int left_array_size, right_array_size, i, j, k;
    left_array_size = middle_idx - left_idx + 1;
    right_array_size = right_idx - middle_idx;
    unsigned long long left_temp_array[left_array_size], right_temp_array[right_array_size];
    // Initialize the temp arrays
    for (i = 0; i < left_array_size; i++)
        left_temp_array[i] = num_array[left_idx + i];
    for (j = 0; j < right_array_size; j++)
        right_temp_array[j] = num_array[middle_idx + 1 + j];
    // Find minimal between minimums in each array
    i = 0;
    j = 0;
    k = left_idx;
    while (i < left_array_size && j < right_array_size) {
        if (left_temp_array[i] <= right_temp_array[j]) {
            num_array[k] = left_temp_array[i];
            i++;
        }
        else {
            num_array[k] = right_temp_array[j];
            j++;
        }
        k++;
    }
    // Copy the rest once one temp array is empty
    while(i<left_array_size) {
        num_array[k] = left_temp_array[i];
        k+=1;
        i+=1;
    }
    while(j<right_array_size) {
        num_array[k] = right_temp_array[j];
        j+=1;
        k+=1;
    }
}

void merge_sort(unsigned long long num_array[], int left_idx, int right_idx)
{
    int middle_idx;
    if (left_idx < right_idx) {
        middle_idx = left_idx + (right_idx - left_idx) / 2;     // middle up to +/- 1 (not critical)
        #pragma omp parallel sections
        {
           #pragma omp section
           {
              merge_sort(num_array, left_idx, middle_idx);
           }
           #pragma omp section
           {
              merge_sort(num_array, middle_idx + 1, right_idx);
           }
        }
        merge(num_array, left_idx, middle_idx, right_idx);
    }
}

int main(int argc, char *argv[])
{
    unsigned long long *num_array;
    char *file_path;
    int num_processors;
    int array_size;
    struct timeval start_time, end_time;
    int execution_time;
    if(argc != 3){
       printf("missing arguments");
       exit(1);
    }
    // args
    file_path = argv[2];
    num_processors = atoi(argv[1]);
    omp_set_num_threads(num_processors);
    // main flow
    read_src_file(file_path, &num_array, &array_size);
    gettimeofday(&start_time, NULL);
    merge_sort(num_array, 0, array_size-1);
    gettimeofday(&end_time, NULL);
    execution_time = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);
    printf("MergeSort: %i\n", execution_time);
    print_output(num_array, array_size);
    free(num_array);
    return 0;
}
