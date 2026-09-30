from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt

WORKSPACE_DIR = Path(__file__).resolve().parents[2]
CSV_PATH = Path(__file__).resolve().with_name('results.csv')
GRAPH_PATH = Path(__file__).resolve().with_name('sort_comparison_graph.png')

# 1. C언어가 만든 CSV 파일 읽기
try:
    df = pd.read_csv(CSV_PATH)
except FileNotFoundError:
    print(f"오류: '{CSV_PATH}' 파일이 없습니다. C 프로그램을 먼저 실행하세요.")
    exit(1)

# 2. 그래프 스타일 및 한글/폰트 설정
patterns = ['Random', 'Sorted', 'Reversed', 'Duplicates']
pattern_titles = {
    'Random': '1. Random Data',
    'Sorted': '2. Sorted Data',
    'Reversed': '3. Reversed Data',
    'Duplicates': '4. Duplicates Data'
}
colors = {'ShellSort': '#e74c3c', 'MergeSort': '#3498db', 'TimSort': '#2ecc71'}

# 2x2 서브플롯 영역 생성
fig, axes = plt.subplots(2, 2, figsize=(13, 9))
axes = axes.flatten()
bar_width = 0.24
algorithms = ['ShellSort', 'MergeSort', 'TimSort']

# 3. 데이터 패턴별 그래프 그리기
for i, pattern in enumerate(patterns):
    ax = axes[i]
    sub_df = df[df['Pattern'] == pattern]

    n_values = sorted(sub_df['N'].unique())
    x_positions = list(range(len(n_values)))

    for algorithm_index, algo in enumerate(algorithms):
        algo_df = sub_df[sub_df['Algorithm'] == algo]
        compare_counts = [
            algo_df.loc[algo_df['N'] == n, 'CompareCount'].iloc[0]
            for n in n_values
        ]
        ax.bar(
            [x + (algorithm_index - 1) * bar_width for x in x_positions],
            compare_counts,
            width=bar_width,
            label=algo,
            color=colors[algo],
            edgecolor='white',
            linewidth=0.7,
        )

    ax.set_title(pattern_titles[pattern], fontsize=12, fontweight='bold')
    ax.set_xlabel('Data Size (N)')
    ax.set_ylabel('Compare Count (log scale)')

    ax.set_xticks(x_positions)
    ax.set_xticklabels([f'{n:,}' for n in n_values])
    ax.set_yscale('log')
    ax.grid(True, which='major', axis='y', linestyle='--', alpha=0.5)
    ax.set_axisbelow(True)
    ax.legend()

plt.suptitle('Sorting Algorithms Comparison by Input Pattern', fontsize=15, fontweight='bold')
plt.tight_layout()

# 4. 이미지 파일 저장
plt.savefig(GRAPH_PATH, dpi=300)
print(f">> [완료] '{GRAPH_PATH}' 그래프가 생성되었습니다!")
