#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int key;
    int original_index;
} Record;

static long long compare_count = 0;
static long long move_count = 0;

static void reset_counters(void) {
    compare_count = 0;
    move_count = 0;
}

static int compare_records(const Record *a, const Record *b) {
    compare_count++;
    return (a->key > b->key) - (a->key < b->key);
}

static void move_record(Record *dest, const Record *src) {
    move_count++;
    *dest = *src;
}

/* 1. 셸 정렬 */
static void shell_sort(Record arr[], int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            Record temp;
            move_record(&temp, &arr[i]);
            int j = i;
            while (j >= gap && compare_records(&arr[j - gap], &temp) > 0) {
                move_record(&arr[j], &arr[j - gap]);
                j -= gap;
            }
            move_record(&arr[j], &temp);
        }
    }
}

/* 2. 병합 정렬 */
static void merge(Record arr[], int l, int m, int r, Record temp[]) {
    int i = l, j = m + 1, k = l;
    while (i <= m && j <= r) {
        if (compare_records(&arr[i], &arr[j]) <= 0) move_record(&temp[k++], &arr[i++]);
        else move_record(&temp[k++], &arr[j++]);
    }
    while (i <= m) move_record(&temp[k++], &arr[i++]);
    while (j <= r) move_record(&temp[k++], &arr[j++]);
    for (i = l; i <= r; i++) move_record(&arr[i], &temp[i]);
}

static void merge_sort_recursive(Record arr[], int l, int r, Record temp[]) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort_recursive(arr, l, m, temp);
        merge_sort_recursive(arr, m + 1, r, temp);
        merge(arr, l, m, r, temp);
    }
}

static void merge_sort(Record arr[], int n) {
    if (n < 2) return;
    Record *temp = (Record *)malloc((size_t)n * sizeof(Record));
    if (!temp) exit(EXIT_FAILURE);
    merge_sort_recursive(arr, 0, n - 1, temp);
    free(temp);
}

/* 3. 완전체 TimSort (Galloping Mode + Natural Run Detection) */
#define MIN_MERGE 32
#define MIN_GALLOP 7
#define MAX_RUN_STACK 85

typedef struct {
    int base;
    int len;
} TimRun;

static int min_run_length(int n) {
    int r = 0;
    while (n >= MIN_MERGE) {
        r |= (n & 1);
        n >>= 1;
    }
    return n + r;
}

/* 자연 Run 탐지 및 역순 구간 제자리 반전 */
static int count_run_and_make_ascending(Record arr[], int lo, int hi) {
    if (lo >= hi - 1) return hi - lo;
    int run_hi = lo + 1;
    if (compare_records(&arr[run_hi], &arr[lo]) < 0) {
        run_hi++;
        while (run_hi < hi && compare_records(&arr[run_hi], &arr[run_hi - 1]) < 0) run_hi++;
        for (int left = lo, right = run_hi - 1; left < right; left++, right--) {
            Record tmp;
            move_record(&tmp, &arr[left]);
            move_record(&arr[left], &arr[right]);
            move_record(&arr[right], &tmp);
        }
    } else {
        run_hi++;
        while (run_hi < hi && compare_records(&arr[run_hi], &arr[run_hi - 1]) >= 0) run_hi++;
    }
    return run_hi - lo;
}

static void binary_insertion_sort(Record arr[], int lo, int hi, int start) {
    for (; start < hi; start++) {
        Record val;
        move_record(&val, &arr[start]);
        int left = lo, right = start;
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (compare_records(&val, &arr[mid]) < 0) right = mid;
            else left = mid + 1;
        }
        for (int i = start; i > left; i--) move_record(&arr[i], &arr[i - 1]);
        move_record(&arr[left], &val);
    }
}

static int lower_bound_record(Record arr[], int lo, int hi, const Record *val) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (compare_records(&arr[mid], val) < 0) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

static int upper_bound_record(Record arr[], int lo, int hi, const Record *val) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (compare_records(&arr[mid], val) <= 0) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

/* Galloping Mode가 적용된 병합 함수 */
static void tim_sort_merge(Record arr[], int base1, int len1, int base2, int len2, Record temp[]) {
    if (compare_records(&arr[base1 + len1 - 1], &arr[base2]) <= 0) return;

    if (len1 <= len2) {
        for (int i = 0; i < len1; i++) move_record(&temp[i], &arr[base1 + i]);
        int i = 0, j = base2, k = base1;
        int right_end = base2 + len2;
        int left_wins = 0, right_wins = 0;

        while (i < len1 && j < right_end) {
            if (compare_records(&temp[i], &arr[j]) <= 0) {
                move_record(&arr[k++], &temp[i++]);
                left_wins++;
                right_wins = 0;
            } else {
                move_record(&arr[k++], &arr[j++]);
                right_wins++;
                left_wins = 0;
            }

            // Galloping Mode 진입
            if (left_wins >= MIN_GALLOP && i < len1 && j < right_end) {
                int bound = upper_bound_record(temp, i, len1, &arr[j]);
                while (i < bound) move_record(&arr[k++], &temp[i++]);
                left_wins = right_wins = 0;
            } else if (right_wins >= MIN_GALLOP && i < len1 && j < right_end) {
                int bound = lower_bound_record(arr, j, right_end, &temp[i]);
                while (j < bound) move_record(&arr[k++], &arr[j++]);
                left_wins = right_wins = 0;
            }
        }
        while (i < len1) move_record(&arr[k++], &temp[i++]);
    } else {
        for (int i = 0; i < len2; i++) move_record(&temp[i], &arr[base2 + i]);
        int i = base2 - 1, j = len2 - 1, k = base2 + len2 - 1;
        int left_wins = 0, right_wins = 0;

        while (i >= base1 && j >= 0) {
            if (compare_records(&arr[i], &temp[j]) > 0) {
                move_record(&arr[k--], &arr[i--]);
                left_wins++;
                right_wins = 0;
            } else {
                move_record(&arr[k--], &temp[j--]);
                right_wins++;
                left_wins = 0;
            }

            // Galloping Mode 진입
            if (left_wins >= MIN_GALLOP && i >= base1 && j >= 0) {
                int bound = upper_bound_record(arr, base1, i + 1, &temp[j]);
                while (i >= bound) move_record(&arr[k--], &arr[i--]);
                left_wins = right_wins = 0;
            } else if (right_wins >= MIN_GALLOP && i >= base1 && j >= 0) {
                int bound = lower_bound_record(temp, 0, j + 1, &arr[i]);
                while (j >= bound) move_record(&arr[k--], &temp[j--]);
                left_wins = right_wins = 0;
            }
        }
        while (j >= 0) move_record(&arr[k--], &temp[j--]);
    }
}

static void tim_sort_merge_at(Record arr[], TimRun runs[], int *run_count, int index, Record temp[]) {
    int base1 = runs[index].base;
    int len1 = runs[index].len;
    int len2 = runs[index + 1].len;
    tim_sort_merge(arr, base1, len1, base1 + len1, len2, temp);
    runs[index].len += len2;
    for (int i = index + 1; i < *run_count - 1; i++) runs[i] = runs[i + 1];
    (*run_count)--;
}

static void tim_sort_merge_collapse(Record arr[], TimRun runs[], int *run_count, Record temp[]) {
    while (*run_count > 1) {
        int n = *run_count - 2;
        if ((n > 0 && (long long)runs[n - 1].len <= (long long)runs[n].len + runs[n + 1].len) ||
            (n > 1 && (long long)runs[n - 2].len <= (long long)runs[n - 1].len + runs[n].len)) {
            if (runs[n - 1].len < runs[n + 1].len) n--;
            tim_sort_merge_at(arr, runs, run_count, n, temp);
        } else if (runs[n].len <= runs[n + 1].len) {
            tim_sort_merge_at(arr, runs, run_count, n, temp);
        } else {
            break;
        }
    }
}

static void tim_sort(Record arr[], int n) {
    if (n < 2) return;
    Record *temp = (Record *)malloc((size_t)n * sizeof(Record));
    if (!temp) exit(EXIT_FAILURE);

    TimRun runs[MAX_RUN_STACK];
    int run_count = 0;
    int min_run = min_run_length(n);
    int lo = 0;

    while (lo < n) {
        int run_len = count_run_and_make_ascending(arr, lo, n);
        int force = (n - lo < min_run) ? (n - lo) : min_run;
        if (run_len < force) {
            binary_insertion_sort(arr, lo, lo + force, lo + run_len);
            run_len = force;
        }

        runs[run_count].base = lo;
        runs[run_count].len = run_len;
        run_count++;

        tim_sort_merge_collapse(arr, runs, &run_count, temp);
        lo += run_len;
    }

    while (run_count > 1) {
        int idx = run_count - 2;
        if (idx > 0 && runs[idx - 1].len < runs[idx + 1].len) idx--;
        tim_sort_merge_at(arr, runs, &run_count, idx, temp);
    }

    free(temp);
}

static int is_stable(const Record arr[], int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i - 1].key == arr[i].key && arr[i - 1].original_index > arr[i].original_index) return 0;
    }
    return 1;
}

static int is_sorted(const Record arr[], int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i - 1].key > arr[i].key) return 0;
    }
    return 1;
}

static int validate_tim_sort(void) {
    Record arr[1025];
    for (int n = 0; n <= 1024; n++) {
        for (int p = 0; p < 4; p++) {
            for (int i = 0; i < n; i++) {
                arr[i].original_index = i;
                if (p == 0) arr[i].key = i;
                else if (p == 1) arr[i].key = n - i;
                else if (p == 2) arr[i].key = (i * 37 + n) % 11;
                else arr[i].key = (i * 7919 + n * 104729) % 1009;
            }
            tim_sort(arr, n);
            if (!is_sorted(arr, n) || !is_stable(arr, n)) return 0;
        }
    }
    return 1;
}

static void generate_data(Record arr[], int n, const char *pattern) {
    for (int i = 0; i < n; i++) {
        arr[i].original_index = i;
        if (strcmp(pattern, "Sorted") == 0) arr[i].key = i;
        else if (strcmp(pattern, "Reversed") == 0) arr[i].key = n - i;
        else if (strcmp(pattern, "Duplicates") == 0) arr[i].key = rand() % 5;
        else arr[i].key = rand() % (n * 10);
    }
}

int main(void) {
    srand(42);
    int sizes[] = {1000, 5000, 10000, 30000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    const char *patterns[] = {"Random", "Sorted", "Reversed", "Duplicates"};
    const char *algorithm_names[] = {"ShellSort", "MergeSort", "TimSort"};
    int num_patterns = 4;

    if (!validate_tim_sort()) {
        fprintf(stderr, "TimSort validation failed!\n");
        return EXIT_FAILURE;
    }

    FILE *csv = fopen("results.csv", "w");
    if (!csv) return EXIT_FAILURE;
    fprintf(csv, "Pattern,N,Algorithm,TimeMS,CompareCount,MoveCount,AuxBytes,IsStable\n");

    for (int p = 0; p < num_patterns; p++) {
        for (int s = 0; s < num_sizes; s++) {
            int n = sizes[s];
            Record *orig = (Record *)malloc(n * sizeof(Record));
            Record *test = (Record *)malloc(n * sizeof(Record));

            generate_data(orig, n, patterns[p]);

            for (int algo = 0; algo < 3; algo++) {
                memcpy(test, orig, n * sizeof(Record));
                reset_counters();

                clock_t start = clock();
                size_t aux_bytes = sizeof(Record);

                if (algo == 0) {
                    shell_sort(test, n);
                } else if (algo == 1) {
                    merge_sort(test, n);
                    aux_bytes = (size_t)n * sizeof(Record);
                } else if (algo == 2) {
                    tim_sort(test, n);
                    aux_bytes = (size_t)n * sizeof(Record);
                }
                clock_t end = clock();

                double time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
                int stable = is_stable(test, n);

                fprintf(csv, "%s,%d,%s,%.3f,%lld,%lld,%zu,%s\n",
                        patterns[p], n, algorithm_names[algo], time_ms,
                        compare_count, move_count, aux_bytes, stable ? "Stable" : "Unstable");
            }
            free(orig);
            free(test);
        }
    }
    fclose(csv);
    printf(">> [완료] results.csv 생성이 완료되었습니다.\n");
    return 0;
}
