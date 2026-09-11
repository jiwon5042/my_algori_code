"""실행: make run-py"""

from sort import bubble_sort, insertion_sort, shell_sort

if __name__ == "__main__":
    input_values = [6, 8, 5, 9, 10, 1, 7, 2, 4, 3]
    algorithms = [
        ("버블 정렬", bubble_sort),
        ("삽입 정렬", insertion_sort),
        ("셸 정렬", shell_sort),
    ]

    for name, sort in algorithms:
        values = input_values.copy()
        sort(values)
        print(f"{name}: {' '.join(str(value) for value in values)}")
