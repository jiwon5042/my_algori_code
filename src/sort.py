"""버블 정렬 — 이 저장소가 도는지 확인하는 예제이자, 새 프로젝트의 출발점."""


def bubble_sort(a):
    """a를 제자리에서 오름차순으로 정렬한다."""
    n = len(a)
    for i in range(n - 1):
        swapped = False
        # 한 번 훑을 때마다 가장 큰 값이 뒤로 밀려 자리를 잡는다.
        for j in range(n - 1 - i):
            if a[j] > a[j + 1]:
                a[j], a[j + 1] = a[j + 1], a[j]
                swapped = True
        # 한 바퀴 동안 교환이 없었다면 이미 정렬된 것이다.
        if not swapped:
            return a
    return a


def insertion_sort(a):
    """a를 삽입 정렬로 제자리에서 오름차순 정렬한다."""
    for i in range(1, len(a)):
        key = a[i]
        j = i - 1
        while j >= 0 and a[j] > key:
            a[j + 1] = a[j]
            j -= 1
        a[j + 1] = key
    return a


def shell_sort(a):
    """a를 셸 정렬로 제자리에서 오름차순 정렬한다."""
    gap = len(a) // 2
    while gap > 0:
        for i in range(gap, len(a)):
            key = a[i]
            j = i
            while j >= gap and a[j - gap] > key:
                a[j] = a[j - gap]
                j -= gap
            a[j] = key
        gap //= 2
    return a
